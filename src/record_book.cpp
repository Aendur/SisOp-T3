#include "ghost_ship.h"
#include "entry_metadata.h"
#include "seqfile.h"
#include "longshort.h"

#include "device.h"
#include "popup.h"
#include "term_ui.h"
#include "fat32.h"
#include "page.h"

VoyageRecord::VoyageRecord(unsigned long clus, unsigned long sec, const void* memsrc) : cluster(clus), sector(sec) {
	memcpy(data, memsrc, 512);
}



//////////////////////////////////////////////////
//  EMBARK - prepare the entry to be recovered  //
//////////////////////////////////////////////////

bool GhostShip::embark(const EntryMetadata& ghost_entry) {
	static const int LX = 75;
	static const int LY = 15;
	static const int LW = 58;
	Popup failed(_term);
	failed
		.build([&] (void) { printf("%-*s", LW, "Failed to embark file"); })
		.build([&] (void) { printf("%-*s", LW, ""); });

	if (!check_consistency(ghost_entry)) {
		failed
			.build([&] (void) { printf("%-*s", LW, _messages.top().c_str()); })
			.show(LX,LY,LW);
		return false;
	}
	if (!check_requirements(ghost_entry)) {
		failed
			.build([&] (void) { printf("%-*s", LW, _messages.top().c_str()); })
			.show(LX,LY,LW);
		return false;
	}
	if (!check_validity(ghost_entry)) {
		failed
			.build([&] (void) { printf("%-*s", LW, _messages.top().c_str()); })
			.show(LX,LY,LW);
		return false;
	}

	entry & new_entry = ((entry*)(_record_book.begin()->second.data))[ghost_entry.position];

	Popup(_term)
		.build([&] (void) { printf("%-*s", LW+7, "\033[1mReady to launch!\033[m"); })
		.build([&] (void) { printf("%-*s", LW, ""); })
		.build([&] (void) { printf("%-*s", LW, "Reference entry:"); })
		.build([&] (void) { printf("%s"  , ghost_entry.data.get_str());  })
		.build([&] (void) { printf("%*s" , LW, ""); })
		.build([&] (void) { printf("%-*s", LW, "Recovered entry:"); })
		.build([&] (void) { printf("%*s" , LW, new_entry.get_str()); })
		.build([&] (void) { printf("%*s" , LW, ""); })
		.build([&] (void) { printf("%-*s", LW+10, _first_msg[0]); })
		.build([&] (void) { printf("%-*s", LW+10, _first_msg[1]); })
		.build([&] (void) { printf("%-*s", LW+10, _first_msg[2]); })
		.build([&] (void) { printf("%-*s", LW+10, _first_msg[3]); })
	.show(LX,LY,LW);


	return true;
}


bool GhostShip::check_consistency(const EntryMetadata& ghost_entry) {
	unsigned long actual_sector = (unsigned long) _sector0->first_sector_of_cluster(ghost_entry.cluster) + ghost_entry.sector;
	long long offset = actual_sector * _sector0->BPB_BytsPerSec();

	_device->seek(offset, false);
	_device->read();

	//_record_book.emplace(ghost_entry.cluster, actual_sector, _device->buffer(0));
	_record_book[actual_sector] = { ghost_entry.cluster, actual_sector, _device->buffer(0) };
	entry* entries = (entry*)(_record_book[actual_sector].data);

	bool same_addr1 = entries[ghost_entry.position].DIR.FstClusLO == ghost_entry.data.DIR.FstClusLO;
	bool same_addr2 = entries[ghost_entry.position].DIR.FstClusHI == ghost_entry.data.DIR.FstClusHI;
	bool same_size  = entries[ghost_entry.position].DIR.FileSize  == ghost_entry.data.DIR.FileSize;
	bool consistent = same_addr1 && same_addr2 && same_size;
	if (!consistent) {
		_messages.push("Inconsistent data");
		return false;
	} else {
		((char*)(&entries[ghost_entry.position]))[0] = '~';
		return true;
	}
}

bool GhostShip::check_requirements(const EntryMetadata& ghost_entry) {
	bool requirements =
		ghost_entry.data.is_ghost() &&
		!ghost_entry.data.is_dir() &&
		!ghost_entry.data.is_long() &&
		!ghost_entry.data.is_empty();
	if (!requirements) {
		_messages.push("Requirements not met");
		return false;
	} else {
		return true;
	}
}

#define STRING2(x) #x
#define STRING(x) STRING2(x)

bool GhostShip::check_validity(const EntryMetadata& ghost_entry) {
	#pragma message( __FILE__ "(" STRING(__LINE__) "): \033[33;1mshould make sure to check if entry is open in the FAT\033[m" )
	
	ulongshort cluster;
	cluster.half.lower = ghost_entry.data.DIR.FstClusLO;
	cluster.half.upper = ghost_entry.data.DIR.FstClusHI;

	_first_header = search_cluster(cluster.full, _first_msg);
	if (!_first_header.valid || _first_header.part != 0) {
		_messages.push("Invalid file format");
		return false;
	} else {
		_cluster_chain[_first_header.part] = cluster.full;
		return true;
	}
}


SeqFile::Header GhostShip::search_cluster(unsigned long N, char out[4][256]) {
	long long offset = _sector0->first_sector_of_cluster(N) * _sector0->BPB_BytsPerSec();
	_device->seek(offset, false);
	_device->read();
	
	const unsigned char * file_addr = _device->buffer(0);
	return SeqFile::check_file(file_addr, out);
}



/////////////////////////////////////////////////
//  LAUNCH - find file pieces around the disk  //
/////////////////////////////////////////////////

bool GhostShip::launch(void) {
	static const int LW = 30;
	unsigned long initial_cluster = _cluster_chain.at(0);
	unsigned long current_cluster = next_empty(initial_cluster);

	unsigned long num_clusters = 1 + (get_total_parts() / _sector0->BPB_SecPerClus());

	while (current_cluster != initial_cluster && _cluster_chain.size() < num_clusters) {
		auto chest = search_cluster(current_cluster, nullptr);
		if (chest.valid) {
			_cluster_chain[chest.part] = current_cluster;
		}

		Popup(_term)
			.build([&] (void) { printf("Found parts %llu of %lu"  , _cluster_chain.size(), num_clusters); } )
			.build([&] (void) { printf("Starting  cluster %-*d", LW-18, initial_cluster); } )
			.build([&] (void) { printf("Searching cluster %-*d", LW-18, current_cluster); } )
			.show(70, 15, LW, false);
		
		current_cluster = next_empty(current_cluster);
	}

	if (current_cluster == initial_cluster) {
		_messages.push("Did not find all parts");
		return false;
	} else {
		Popup msg(_term);
		msg.build([&] (void) { printf("Found %llu of %lu parts"  , _cluster_chain.size(), num_clusters); });

		for (const auto & [key, val] : _cluster_chain) {
			msg.build([&] (void) { printf("Part %d found in cluster %d", key, val); } );
		}

		msg.show(70, 15, LW);
		return true;
	}

}

unsigned long GhostShip::get_total_parts(void) const {
	unsigned long total_parts = 1 + (_first_header.file_size / _sector0->BPB_BytsPerSec());
	return total_parts;
}

/// Attempts to find the next empty cluster, starting from N.
/// Goes back to sector 0 if we reach the end of the FAT.
/// \returns Returns the next empty cluster that is greater than N, or N if no other empty cluster is found
/// \param N The cluster to start searching from
unsigned long GhostShip::next_empty(unsigned long N) const {
	unsigned long current = N;
	// start looking from N
	while (++current < _sector0->n_fat_entries() && _FAT1[current] != 0);

	// if we reach the end of the FAT, continue looking from the begining
	if (current < _sector0->n_fat_entries()) {
		return current;
	} else {
		current = 0;
		while (++current < N && _FAT1[current] != 0);
		return current;
	}
}



//////////////////////////////////////////////
//  DOCK - write changes to the FAT buffer  //
//////////////////////////////////////////////

bool GhostShip::dock(void) {
	unsigned long total_parts = get_total_parts();
	unsigned long total_clusters = 1 + (total_parts / _sector0->BPB_SecPerClus());

	unsigned long part = 0;
	unsigned long next = _sector0->BPB_SecPerClus();
	unsigned long N1 = _cluster_chain.at(part);
	unsigned long N2 = _cluster_chain.at(next);
	try {
		for (unsigned long i = 0; i < total_clusters - 1; ++i) {
			part  =  i      * _sector0->BPB_SecPerClus();
			next  = (i + 1) * _sector0->BPB_SecPerClus();
			N1 = _cluster_chain.at(part);
			N2 = _cluster_chain.at(next);
			sort_cargo(N1, N2, 0);
			sort_cargo(N1, N2, 1);
		}
		sort_cargo(N2, FAT_EOC, 0);
		sort_cargo(N2, FAT_EOC, 1);
	} catch (std::exception &) {
		_messages.emplace("Error altering the FAT");
		return false;
	}
	return true;
}

void GhostShip::sort_cargo(unsigned long N1, unsigned long N2, int fatn) {
	ULONG    fat_sector = (ULONG) _sector0->fat_sec_num(N1, fatn);
	LONGLONG fat_offset = _sector0->fat_ent_off(N1);
	int fat_element = (int) (fat_offset / sizeof(unsigned long));
	bool contains = true;

	_device->seek(fat_sector * _sector0->BPB_BytsPerSec(), false);
	_device->read();
	
	if (!_record_book.contains(fat_sector)) {
		_record_book[fat_sector] = { 0, fat_sector, _device->buffer(0) };
		contains = false;
	}

	unsigned long next = N2;
	unsigned int* table = (unsigned int *) _record_book[fat_sector].data;
	table[fat_element] = next;

	//printf("\033[1;1H\033[0J");
	//printf("\n\n\nFAT%d - SECTOR %lu ENTRY %d     PART %02lu - CLUSTER %08X -> %08X - %s\n\n", fatn+1, fat_sector, fat_element, part, N1, next, contains ? "OLD" : "NEW");
	//compare_fat_sectors((unsigned long*)_device->buffer(0), (unsigned long*)(&_record_book[fat_sector].data));
	//_term->read();
}



/////////////////////////////////////////////////////////
//  DISEMBARK - review and commit changes to the disk  //
/////////////////////////////////////////////////////////
void compare_fat_sectors(unsigned long * sec1, unsigned long * sec2) {
	static const int LW = 4;
	static const int NL = 128 / LW;
	for (int i = 0; i <  NL; ++i) {
		for (int j = 0; j < LW; ++j) {
			int index = i * LW + j;
			if (sec1[index] == sec2[index]) {
				printf("%-4d %08X  %-8s     ", index, sec1[index], "EQ");
			} else {
				printf("%-4d %08X->%08X     ", index, sec1[index], sec2[index]);
			}
		}
		printf("\n");
	}
	printf("\n");
}


void compare_byte_sectors(unsigned char * ref_sector, unsigned char * dif_sector, bool show_ref) {
	static const int LW = 16;
	static const int NL = 512 / LW;
	for (int i = 0; i <  NL; ++i) {
		for (int j = 0; j < LW; ++j) {
			int index = i * LW + j;
			if (ref_sector[index] == dif_sector[index]) {
				printf("|   %02X   ", ref_sector[index]);
			} else {
				printf("| %02X->%02X ", ref_sector[index], dif_sector[index]);
			}
		}
		printf("|\n");
	}
	printf("\n");
}

#include <vector>

bool GhostShip::disembark(void) {
	unsigned long fat_start = _sector0->BPB_RsvdSecCnt();
	unsigned long fat1_end = fat_start + _sector0->BPB_FATSz32();
	unsigned long fat2_end = fat1_end + _sector0->BPB_FATSz32();

	printf("\033[1;1H\033[0J");
	std::vector<unsigned int> selections;
	int current_selection = 0;
	for (auto & [sector, record] : _record_book) { selections.push_back(sector); }

	KeyCode key = TERMUI_KEY_UNDEFINED;
	enum SelectedOption {
		NONE,
		WRITE,
		DISCARD
	};
	
	SelectedOption selected_option = NONE;
	while(selected_option == NONE) {
		KeyCode key = _term->read();

		switch(key) {
			case TERMUI_KEY_TAB        : break;
			case TERMUI_KEY_ARROW_LEFT : break;
			case TERMUI_KEY_ARROW_RIGHT: break;
			case TERMUI_KEY_RETURN     : break;
			default: break;
		}


	}

	for (auto & [sector, record] : _record_book) {
		_device->seek(sector * _sector0->BPB_BytsPerSec(), false);
		_device->read();

		printf("\n");
		printf("SECTOR %d - ", sector);
		if      (fat_start <= sector && sector < fat1_end) { printf("FAT1"); }
		else if (fat1_end  <= sector && sector < fat2_end) { printf("FAT2"); }
		else                                               { printf("DATA"); }
		printf("\n");
		printf("\n");
	}
	if (_term->read() == TERMUI_KEY_TAB) { record_book_page.switch_buff(); }
	return true;
}

