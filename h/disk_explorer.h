#ifndef DISK_EXPLORER_H
#define DISK_EXPLORER_H

#include <vector>
//#include <string>

class DiskExplorer {
public:
	static std::vector<char> list_drives(void);
};

#endif