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
	unsigned long long sector;
	unsigned char data[512];
};

enum VoyageRecordType {
	CONTAINER_DIRECTORY_SECTOR,
};

typedef std::map<VoyageRecordType, VoyageRecord> RecordBook;

class GhostShip {
	Device * _device;
	fat32 * _sector0;
	TermUI * _term;
	//VoyageReport _report;
	std::stack<std::string> _messages;

	RecordBook _record_book;
	SeqFile::Header _first_header;
	char _first_msg[4][256];


	bool check_consistency(const EntryMetadata& ghost_entry);
	bool check_requirements(const EntryMetadata& ghost_entry);
	bool check_validity(const EntryMetadata& ghost_entry);
public:
	GhostShip(Device * d, fat32 * s0, TermUI * t) : _device(d), _sector0(s0), _term(t) {}
	bool embark(const EntryMetadata& ghost_entry);
	void launch(unsigned int * FAT);
};

#endif
