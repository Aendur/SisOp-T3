#ifndef DEVICE_H
#define DEVICE_H


#include <Windows.h>
#include <vector>

class Device {
private:
	FILE * _out = stdout;
	FILE * _log = stderr;
	HANDLE _device = INVALID_HANDLE_VALUE;
	DISK_GEOMETRY _geometry;
	DWORD _geom_nbytes;
public:
	static std::vector<char> get_drives(void);
	void open_device(char drive);
	void close_device(void);

	void get_geometry(void);
	void print_geometry(void) const;
	void read_fat32(void);
};

#endif