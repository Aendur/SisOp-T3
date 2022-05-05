#ifndef GHOST_SHIP_H
#define GHOST_SHIP_H


struct fat32;
struct EntryMetadata;
class Device;
class TermUI;

#include <stack>
#include <string>
#include <map>
#include "seqfile.h"
#include "record_book.h"

class GhostShip {
	Device * _device;
	fat32 * _sector0;
	TermUI * _term;
	unsigned int * _FAT1;

	RecordBook _record_book;
	SeqMessage _first_msg;

	bool check_consistency(const EntryMetadata& ghost_entry);
	bool check_requirements(const EntryMetadata& ghost_entry);
	bool check_validity(const EntryMetadata& ghost_entry);
	 
	SeqFile::Header search_cluster(unsigned long N, SeqMessage out);
	unsigned long next_empty(unsigned long N) const;
	void sort_cargo(unsigned long N, unsigned long N2, int fatn);

public:
	GhostShip(Device * d, fat32 * s0, TermUI * t, unsigned int * fat);
	bool embark(const EntryMetadata& ghost_entry);
	bool launch(void);
	bool dock(void);
	bool disembark(void);

};

#endif
