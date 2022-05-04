#include "ghost_ship.h"
#include "entry_metadata.h"
#include "seqfile.h"
#include "longshort.h"

#include "device.h"
#include "popup.h"
#include "term_ui.h"

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
	unsigned long initial_cluster = _cluster_chain[0];
	unsigned long current_cluster = next_empty(initial_cluster);

	unsigned long total_parts = get_total_parts();
	unsigned long num_parts = 1;

	while (current_cluster != initial_cluster && num_parts < total_parts) {
		auto chest = search_cluster(current_cluster, nullptr);
		if (chest.valid) {
			_cluster_chain[chest.part] = current_cluster;
			++num_parts;
		}

		Popup(_term)
			.build([&] (void) { printf("Found parts %d of %d"  , num_parts, total_parts); } )
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
		msg.build([&] (void) { printf("Found %d of %d parts"  , num_parts, total_parts); });

		for (const auto & [key, val] : _cluster_chain) {
			msg.build([&] (void) { printf("Part %d found in cluster %d", key, val); } );
		}

		msg.show(70, 15, LW);
		return true;
	}

}

unsigned long GhostShip::get_total_parts(void) const {
	unsigned long total_parts = 1 + (_first_header.file_size / _sector0->cluster_size());
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
	for (const auto & [part, N] : _cluster_chain) {
		sort_cargo(part, N, 0);
		sort_cargo(part, N, 1);
	}
	
	return true;
}

void print_fat_sector(unsigned long * sec) {
	for (int i = 0; i <  8; ++i) {
		for (int j = 0; j < 16; ++j) {
			printf("%08x   ", sec[i * 16 + j]);
		}
		printf("\n");
	}
	printf("\n");
}

void GhostShip::sort_cargo(unsigned long part, unsigned long N, int fatn) {
	ULONG fat_sector = (ULONG) _sector0->fat_sec_num(N, fatn);
	LONGLONG fat_offset = _sector0->fat_ent_off(N);

	if (!_record_book.contains(fat_sector)) {
		_device->seek(fat_sector * _sector0->BPB_BytsPerSec(), false);
		_device->read();
		_record_book[fat_sector] = { 0, fat_sector, _device->buffer(0) };
	}
	*(unsigned long*)(&_record_book[fat_sector].data[fat_offset]) = _cluster_chain[part+1];

	printf("\033[1;1H\033[0J");
	print_fat_sector((unsigned long*)_device->buffer(0));
	print_fat_sector((unsigned long*)(&_record_book[fat_sector].data));
	_term->read();
}

