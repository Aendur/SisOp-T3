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

struct VoyageRecord {
	unsigned long cluster;
	unsigned long sector;
	unsigned char data[512];
	VoyageRecord(void) {};
	VoyageRecord(unsigned long clus, unsigned long sec, const void* memsrc);
};

typedef std::map<unsigned long, VoyageRecord> RecordBook;

class GhostShip {
	Device * _device;
	fat32 * _sector0;
	TermUI * _term;
	unsigned int * _FAT1;
	unsigned int * _FAT2;
	//VoyageReport _report;
	std::stack<std::string> _messages;

	RecordBook _record_book;
	SeqFile::Header _first_header;
	char _first_msg[4][256];
	std::map<unsigned long, unsigned long> _cluster_chain;

	bool check_consistency(const EntryMetadata& ghost_entry);
	bool check_requirements(const EntryMetadata& ghost_entry);
	bool check_validity(const EntryMetadata& ghost_entry);

	unsigned long get_total_parts(void) const;
	 
	SeqFile::Header search_cluster(unsigned long N, char out[4][256]);
	unsigned long next_empty(unsigned long N) const;
	void sort_cargo(unsigned long part, unsigned long N, int fatn);
public:
	GhostShip(Device * d, fat32 * s0, TermUI * t, unsigned int * F1, unsigned int * F2) : _device(d), _sector0(s0), _term(t), _FAT1(F1), _FAT2(F2) {}
	bool embark(const EntryMetadata& ghost_entry);
	bool launch(void);
	bool dock(void);

};

#endif
