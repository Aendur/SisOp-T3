#include "record_book.h"
#include "term_ui.h"
#include "popup.h"
#include "fat32.h"
#include "device.h"

bool RecordBook::add_record(unsigned long clus, unsigned long sec, const void* memsrc) {
	if (!_records.contains(sec)) {
		_records[sec].cluster = clus;
		_records[sec].sector = sec;
		memcpy(_records[sec].data, memsrc, 512);
		return true;
	} else {
		return false;
	}
}

void RecordBook::register_cargo(const SeqFile::Header & cargo) {
	if (!_registered) {
		memcpy(&_first_header, &cargo, sizeof(SeqFile::Header));
		_registered = true;
	} else {
		throw std::logic_error("Can only register once");
	}
}

bool RecordBook::link_cluster(unsigned long part, unsigned long cluster) {
	if (!_cluster_chain.contains(part)) {
		_cluster_chain[part] = cluster;
		return true;
	} else {
		return false;
	}
}

void RecordBook::alter_fat_record(unsigned long sector, unsigned long position, unsigned long value) {
	unsigned int* table = (unsigned int *) _records[sector].data;
	table[position] = value;
}


void RecordBook::show_chain(void) const {
	static const int LW = 32;
	Popup msg(_term);
	msg.build([&] (void) { printf("Found \033[1m%-3llu\033[0m of \033[1m%3lu\033[0m parts          "  , _cluster_chain.size(), get_total_clusters()); });

	for (const auto & [key, val] : _cluster_chain) {
		msg.build([&] (void) { printf("Part \033[1m%-3d\033[0m found in cluster \033[1m%-6d\033[0m", key, val); } );
	}

	msg.show(80, 12, LW);
}

unsigned long RecordBook::get_total_parts(void) const {
	unsigned long total_parts = 1 + (_first_header.file_size / _sector0->BPB_BytsPerSec());
	return total_parts;
}

unsigned long RecordBook::get_total_clusters(void) const {
	unsigned long total_clusters = 1 + (get_total_parts() / _sector0->BPB_SecPerClus());
	return total_clusters;
}

RecordBook::SelectedOption RecordBook::review_records(void) const {
	std::map<SelectedOption, std::string> options = {
		{ DISCARD , "Discard changes"},
		{ WRITE   , "Write changes"  },
	};
	SelectedOption highlighted_option = DISCARD;

	const auto last_record = --_records.end();
	auto selected = _records.begin();
	bool show_dif_sector = false;
	
	_term->clear_screen();
	print_record(selected->second, show_dif_sector);
	print_options(options, highlighted_option);

	SelectedOption selected_option = NONE;
	while(selected_option == NONE) {
	 	KeyCode key = _term->read();

		switch(key) {
			case TERMUI_KEY_TAB        : show_dif_sector = !show_dif_sector; break;
			case TERMUI_KEY_ARROW_LEFT : highlighted_option = DISCARD; break;
			case TERMUI_KEY_ARROW_RIGHT: highlighted_option = WRITE  ; break;
			case TERMUI_KEY_PGUP       : if (selected != _records.begin()) --selected; break;
			case TERMUI_KEY_PGDOWN     : if (selected != last_record) ++selected; break;
			case TERMUI_KEY_RETURN     : selected_option = highlighted_option; break;
			case TERMUI_KEY_ESC        : selected_option = DISCARD; break;
			default: break;
		}
		
		_term->clear_screen();
		print_record(selected->second, show_dif_sector);
		print_options(options, highlighted_option);
	}

	_term->clear_screen();
	return selected_option;
}

#define COLOR_RED "\033[31;1m"
#define COLOR_GRN "\033[32;1m"
#define COLOR_ASH "\033[m"
#define COLOR_GRY "\033[2m"


void RecordBook::print_record (const VoyageRecord & record, bool show_dif) const {
	static const unsigned long fat_start = _sector0->BPB_RsvdSecCnt();
	static const unsigned long fat1_end = fat_start + _sector0->BPB_FATSz32();
	static const unsigned long fat2_end = fat1_end + _sector0->BPB_FATSz32();

	_device->seek(record.sector * _sector0->BPB_BytsPerSec(), false);
	_device->read();

	int nfat = -1;
	if      (fat_start <= record.sector && record.sector < fat1_end) { nfat = 1; }
	else if (fat1_end  <= record.sector && record.sector < fat2_end) { nfat = 2; }
	if (nfat == -1) {
		print_record_data(_device->buffer(0), record, show_dif);
	} else {
		print_record_fat(nfat, (unsigned int*)_device->buffer(0), record, show_dif);
	}
}

#define LMARGIN  "\033[%d;36H"
#define VBAR    COLOR_GRY "|" COLOR_ASH
#define BAR_FAT  "\033[2m+-----------------------------------------------------------------------------------+\033[m"
#define SPC_FAT  "\033[2m|                                                                                   |\033[m"
#define BAR_DATA "\033[2m+----------------------------------------------------------------------------------------------------------+\033[m"
#define SPC_DATA "\033[2m|                                                                                                          |\033[m"
void RecordBook::print_record_fat (int nfat, const unsigned int * ref_sector, const VoyageRecord & record, bool show_dif) const {
	static const int LW = 8;
	static const int NL = 128 / LW;

	int Y = 2;
	printf(LMARGIN BAR_FAT, Y++);
	printf(LMARGIN VBAR COLOR_GRY "  FAT%d SECTOR %-69lu" COLOR_ASH VBAR, Y++, nfat, record.sector);
	printf(LMARGIN BAR_FAT, Y++);
	const unsigned int * dif_sector = (unsigned int*) record.data;

	for (int i = 0; i <  NL; ++i) {
		printf(LMARGIN VBAR " ", Y++);
		for (int j = 0; j < LW; ++j) {
			int index = i * LW + j;
			if (ref_sector[index] == dif_sector[index]) {
				printf(" %08X ", ref_sector[index]);
			} else {
				if (show_dif) {
					printf(COLOR_GRN " %08X " COLOR_ASH, dif_sector[index]);
				} else {
					printf(COLOR_RED " %08X " COLOR_ASH, ref_sector[index]);
				}
			}
			if (j % 4 == 3) { printf(" "); }
		}
		printf(VBAR);
	}
	printf(LMARGIN BAR_FAT, Y++);
	printf(LMARGIN SPC_FAT, Y++);
	printf(LMARGIN SPC_FAT, Y++);
	printf(LMARGIN SPC_FAT, Y++);
	printf(LMARGIN BAR_FAT, Y++);
}

void RecordBook::print_record_data (const unsigned char * ref_sector, const VoyageRecord & record, bool show_dif) const {
	static const int LW = 32;
	static const int NL = 512 / LW;

	int Y = 2;
	printf(LMARGIN BAR_DATA, Y++);
	printf(LMARGIN VBAR COLOR_GRY "  DATA SECTOR %-10d CLUSTER %-73d" COLOR_ASH VBAR, Y++, record.sector, record.cluster);
	printf(LMARGIN BAR_DATA, Y++);
	const unsigned char * dif_sector = record.data;

	for (int i = 0; i <  NL; ++i) {
		printf(LMARGIN VBAR "  ", Y++);
		for (int j = 0; j < LW; ++j) {
			int index = i * LW + j;
			if (ref_sector[index] == dif_sector[index]) {
				printf("%02X ", ref_sector[index]);
			} else {
				if (show_dif) {
					printf(COLOR_GRN "%02X " COLOR_ASH, dif_sector[index]);
				} else {
					printf(COLOR_RED "%02X " COLOR_ASH, ref_sector[index]);
				}
			}
			if (j % 4 == 3) { printf(" "); }
		}
		printf(VBAR);
	}
	printf(LMARGIN BAR_DATA, Y++);
	printf(LMARGIN SPC_DATA, Y++);
	printf(LMARGIN SPC_DATA, Y++);
	printf(LMARGIN SPC_DATA, Y++);
	printf(LMARGIN BAR_DATA, Y++);
	
}

void RecordBook::print_options(const std::map<SelectedOption, std::string> & options, SelectedOption highlighted) const {
	printf("\033[23;55H");
	for (const auto & [opt, str] : options) {
		if(opt == highlighted) {
			printf("     \033[7m%s\033[27m     ", str.c_str());
		} else {
			printf("     %s     ", str.c_str());
		}
	}

	
	printf("\033[2;1H");
	printf("--- REVIEW OPTIONS ---\n");
	printf("PGUP  : prev sector\n");
	printf("PGDOWN: next sector\n");
	printf("TAB   : toggle view changesl\n");
	printf("ARROWS: select option\n");
	printf("RETURN: confirm selection\n");
	printf("ESC   : cancel\n");
}
