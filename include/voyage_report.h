#ifndef VOYAGE_REPORT_H
#define VOYAGE_REPORT_H

#include <stack>
#include <string>

struct EntryMetadata;

class VoyageReport {
private:
	std::stack<std::string> _messages;
public:
	// ~VoyageReport(void);
	void add_message(const std::string & msg);

	void record_entry_data(const EntryMetadata & entry, const void * buffer);

};

#endif


