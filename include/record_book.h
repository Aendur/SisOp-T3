#ifndef RECORD_BOOK_H
#define RECORD_BOOK_H

#include <map>
#include <string>
#include <stack>
#include "seqfile.h"

struct fat32;
class TermUI;

struct VoyageRecord {
	unsigned long cluster;
	unsigned long sector;
	unsigned char data[512];
};

class RecordBook {
private:
	bool _registered = false;
	TermUI * _term = nullptr;
	fat32 * _sector0 = nullptr;
	std::stack<std::string> _messages;
	std::map<unsigned long, VoyageRecord> _records;
	std::map<unsigned long, unsigned long> _cluster_chain;
	SeqFile::Header _first_header;

public:
	//RecordBook(void);
	void init(TermUI * t, fat32 * s0) { _term = t; _sector0 = s0; }

	inline void add_message (const std::string & msg) { _messages.push(msg); }
	inline const std::string & last_message (void) const { return _messages.top(); }

	bool add_record (unsigned long clus, unsigned long sec, const void* memsrc);
	inline const VoyageRecord & get_record (unsigned long sec) const { return _records.at(sec); }
	inline const std::map<unsigned long, VoyageRecord> & get_records (void) const { return _records; }
	inline const VoyageRecord & first_record (void) const { return _records.begin()->second; };
	void alter_fat_record(unsigned long sector, unsigned long position, unsigned long value);

	void register_cargo(const SeqFile::Header & cargo); 
	inline const SeqFile::Header & first_header(void) const { return _first_header; };

	bool link_cluster(unsigned long part, unsigned long cluster);
	inline unsigned long get_cluster(unsigned long part) const { return _cluster_chain.at(part); }
	inline size_t get_chain_size(void) const { return _cluster_chain.size(); }

	void show_chain(void) const;

	unsigned long get_total_parts(void) const;
	unsigned long get_total_clusters(void) const;
};

#endif


