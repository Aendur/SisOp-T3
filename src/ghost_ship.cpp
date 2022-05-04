#include "ghost_ship.h"
#include "entry_metadata.h"
#include "seqfile.h"
#include "longshort.h"

#include "fat32.h"
#include "device.h"
#include "popup.h"

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

void GhostShip::launch(unsigned int * FAT) {
	(void) FAT;
}

bool GhostShip::check_consistency(const EntryMetadata& ghost_entry) {
	unsigned long long actual_sector = _sector0->first_sector_of_cluster(ghost_entry.cluster) + ghost_entry.sector;
	long long offset = actual_sector * _sector0->BPB_BytsPerSec();

	_device->seek(offset, false);
	_device->read();

	_record_book[CONTAINER_DIRECTORY_SECTOR].sector = actual_sector;
	memcpy(_record_book[CONTAINER_DIRECTORY_SECTOR].data, _device->buffer(0), 512);
	
	entry* entries = (entry*) _record_book[CONTAINER_DIRECTORY_SECTOR].data;

	bool same_addr1 = entries[ghost_entry.position].DIR.FstClusLO == ghost_entry.data.DIR.FstClusLO;
	bool same_addr2 = entries[ghost_entry.position].DIR.FstClusHI == ghost_entry.data.DIR.FstClusHI;
	bool same_size  = entries[ghost_entry.position].DIR.FileSize  == ghost_entry.data.DIR.FileSize;
	bool consistent = same_addr1 && same_addr2 && same_size;
	if (!consistent) {
		_messages.push("Inconsistent data");
		return false;
	} else {
		((char*)(&entries[ghost_entry.position]))[0] = '~';
		//const_cast<EntryMetadata&>(ghost_entry).data.DIR.Name[0] = '~';
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
	// THIS FUNCTION SHOULD MAKE SURE TO LOOK CHECK IF ENTRY IS OPEN IN THE FAT
	#pragma message( __FILE__ "(" STRING(__LINE__) "): \033[33;1mshould make sure to check if entry is open in the FAT\033[m" )
	
	longshort cluster;
	cluster.half.lower = ghost_entry.data.DIR.FstClusLO;
	cluster.half.upper = ghost_entry.data.DIR.FstClusHI;
	
	long long offset = _sector0->first_sector_of_cluster(cluster.full) * _sector0->BPB_BytsPerSec();
	_device->seek(offset, false);
	_device->read();

	const unsigned char * file_addr = _device->buffer(0);
	_first_header = SeqFile::check_file(file_addr, _first_msg);
	if (!_first_header.valid) {
		_messages.push("Invalid file format");
		return false;
	} else {
		// we already set _first_header
		return true;
	}
}



