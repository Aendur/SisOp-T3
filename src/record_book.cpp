#include "record_book.h"
#include "term_ui.h"
#include "popup.h"
#include "fat32.h"

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
	static const int LW = 30;
	Popup msg(_term);
	msg.build([&] (void) { printf("Found %llu of %lu parts"  , _cluster_chain.size(), get_total_clusters()); });

	for (const auto & [key, val] : _cluster_chain) {
		msg.build([&] (void) { printf("Part %d found in cluster %d", key, val); } );
	}

	msg.show(70, 15, LW);
}

unsigned long RecordBook::get_total_parts(void) const {
	unsigned long total_parts = 1 + (_first_header.file_size / _sector0->BPB_BytsPerSec());
	return total_parts;
}

unsigned long RecordBook::get_total_clusters(void) const {
	unsigned long total_clusters = 1 + (get_total_parts() / _sector0->BPB_SecPerClus());
	return total_clusters;
}
