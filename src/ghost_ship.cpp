#include "ghost_ship.h"
#include "entry_metadata.h"
#include "seqfile.h"
#include "longshort.h"

#include "device.h"
#include "popup.h"
#include "term_ui.h"
#include "fat32.h"
#include "page.h"

GhostShip::GhostShip(Device * d, fat32 * s0, TermUI * t, unsigned int * fat) : _device(d), _sector0(s0), _term(t), _FAT1(fat) {
	_record_book.init(_term, _sector0);
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
			.build([&] (void) { printf("%-*s", LW, _record_book.last_message().c_str()); })
			.show(LX,LY,LW);
		return false;
	}
	if (!check_requirements(ghost_entry)) {
		failed
			.build([&] (void) { printf("%-*s", LW, _record_book.last_message().c_str()); })
			.show(LX,LY,LW);
		return false;
	}
	if (!check_validity(ghost_entry)) {
		failed
			.build([&] (void) { printf("%-*s", LW, _record_book.last_message().c_str()); })
			.show(LX,LY,LW);
		return false;
	}

	entry & new_entry = ((entry*)(_record_book.first_record().data))[ghost_entry.position];

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
	_record_book.add_record(ghost_entry.cluster, actual_sector, _device->buffer(0));
	entry* entries = (entry*)(_record_book.get_record(actual_sector).data);

	bool same_addr1 = entries[ghost_entry.position].DIR.FstClusLO == ghost_entry.data.DIR.FstClusLO;
	bool same_addr2 = entries[ghost_entry.position].DIR.FstClusHI == ghost_entry.data.DIR.FstClusHI;
	bool same_size  = entries[ghost_entry.position].DIR.FileSize  == ghost_entry.data.DIR.FileSize;
	bool consistent = same_addr1 && same_addr2 && same_size;
	if (!consistent) {
		_record_book.add_message("Inconsistent data");
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
		_record_book.add_message("Requirements not met");
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

	_record_book.register_cargo(search_cluster(cluster.full, _first_msg));
	const auto & first_header = _record_book.first_header();
	if (!first_header.valid || first_header.part != 0) {
		_record_book.add_message("Invalid file format");
		return false;
	} else {
		_record_book.link_cluster(first_header.part, cluster.full);
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
	unsigned long initial_cluster = _record_book.get_cluster(0);
	unsigned long current_cluster = next_empty(initial_cluster);
	unsigned long num_clusters = _record_book.get_total_clusters();

	while (current_cluster != initial_cluster && _record_book.get_chain_size() < num_clusters) {
		auto chest = search_cluster(current_cluster, nullptr);

		bool same_size = chest.file_size == _record_book.first_header().file_size;
		bool same_uid1 = chest.id1       == _record_book.first_header().id1;
		bool same_uid2 = chest.id2       == _record_book.first_header().id2;
		if (chest.valid && same_size && same_uid1 && same_uid2) {
			_record_book.link_cluster(chest.part, current_cluster);
		}

		// Popup(_term)
		// 	.build([&] (void) { printf("Found parts %llu of %lu"  , _record_book.get_chain_size(), num_clusters); } )
		// 	.build([&] (void) { printf("Starting  cluster %-*d", LW-18, initial_cluster); } )
		// 	.build([&] (void) { printf("Searching cluster %-*d", LW-18, current_cluster); } )
		// 	.show(70, 15, LW, false);
		
		current_cluster = next_empty(current_cluster);
	}

	if (current_cluster == initial_cluster) {
		_record_book.add_message("Did not find all parts");
		return false;
	} else {
		_record_book.show_chain();
		return true;
	}

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
	unsigned long total_clusters = _record_book.get_total_clusters();

	unsigned long part = 0;
	unsigned long next = _sector0->BPB_SecPerClus();
	unsigned long N1 = _record_book.get_cluster(part);
	unsigned long N2 = _record_book.get_cluster(next);
	_term->clear_screen();
	try {
		for (unsigned long i = 0; i < total_clusters - 1; ++i) {
			part  =  i      * _sector0->BPB_SecPerClus();
			next  = (i + 1) * _sector0->BPB_SecPerClus();
			N1 = _record_book.get_cluster(part);
			N2 = _record_book.get_cluster(next);
			printf("PART %lu:\n", part);
			sort_cargo(N1, N2, 0);
			sort_cargo(N1, N2, 1);
		}
		printf("PART %lu:\n", next);
		sort_cargo(N2, FAT_EOC, 0);
		sort_cargo(N2, FAT_EOC, 1);
	} catch (std::exception &) {
		_record_book.add_message("Error altering the FAT");
		return false;
	}
	return true;
}

void GhostShip::sort_cargo(unsigned long N1, unsigned long N2, int fatn) {
	ULONG    fat_sector = (ULONG) _sector0->fat_sec_num(N1, fatn);
	LONGLONG fat_offset = _sector0->fat_ent_off(N1);
	int fat_element = (int) (fat_offset / sizeof(unsigned long));

	_device->seek(fat_sector * _sector0->BPB_BytsPerSec(), false);
	_device->read();
	bool newentry = _record_book.add_record(0, fat_sector, _device->buffer(0));

	_record_book.alter_fat_record(fat_sector, fat_element, N2);


	printf("FAT%d - SECTOR %lu ENTRY %d     CLUSTER %08X -> %08X - %s\n", fatn+1, fat_sector, fat_element, N1, N2, newentry ? "NEW" : "OLD");
	//compare_fat_sectors((unsigned long*)_device->buffer(0), (unsigned long*)(&_record_book.get_record(fat_sector).data));
	//_term->read();
}


/////////////////////////////////////////////////////////
//  DISEMBARK - review and commit changes to the disk  //
/////////////////////////////////////////////////////////
void compare_fat_sectors(unsigned long * sec1, unsigned long * sec2);
void compare_byte_sectors(const unsigned char * ref_sector, const unsigned char * dif_sector, bool show_ref);

bool GhostShip::disembark(void) {
	unsigned long fat_start = _sector0->BPB_RsvdSecCnt();
	unsigned long fat1_end = fat_start + _sector0->BPB_FATSz32();
	unsigned long fat2_end = fat1_end + _sector0->BPB_FATSz32();

	// printf("\033[1;1H\033[0J");
	// std::vector<unsigned int> selections;
	// int current_selection = 0;
	// for (auto & [sector, record] : _record_book) { selections.push_back(sector); }

	// KeyCode key = TERMUI_KEY_UNDEFINED;
	// enum SelectedOption {
	// 	NONE,
	// 	WRITE,
	// 	DISCARD
	// };
	
	// SelectedOption selected_option = NONE;
	// while(selected_option == NONE) {
	// 	KeyCode key = _term->read();

	// 	switch(key) {
	// 		case TERMUI_KEY_TAB        : break;
	// 		case TERMUI_KEY_ARROW_LEFT : break;
	// 		case TERMUI_KEY_ARROW_RIGHT: break;
	// 		case TERMUI_KEY_RETURN     : break;
	// 		default: break;
	// 	}


	// }


	for (const auto & [sector, record] : _record_book.get_records()) {
		printf("\033[1;1H\033[0J");
		_device->seek(sector * _sector0->BPB_BytsPerSec(), false);
		_device->read();
		printf("\n");
		printf("SECTOR %d - ", sector);
		if      (fat_start <= sector && sector < fat1_end) { printf("FAT1"); }
		else if (fat1_end  <= sector && sector < fat2_end) { printf("FAT2"); }
		else                                               { printf("DATA"); }
		compare_byte_sectors(_device->buffer(0), record.data, false);
		printf("\n");
		printf("\n");
		//_term->read();

		_device->seek(sector * _sector0->BPB_BytsPerSec(), false);
		_device->write((PBYTE)record.data);
	}
	//if (_term->read() == TERMUI_KEY_TAB) { record_book_page.switch_buff(); }
	return true;
}


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


void compare_byte_sectors(const unsigned char * ref_sector, const unsigned char * dif_sector, bool show_ref) {
	(void) show_ref;
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

