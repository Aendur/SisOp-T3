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
	PBYTE _buffer;
	DWORD _geom_nbytes;
	DWORD _read_nbytes;

	unsigned long long _offset = 0;

	void get_geometry(void);

public:
	static std::vector<wchar_t> get_drives(void);
	void open_drive(WCHAR drive);
	void close_drive(void);
	
	void read(void);
	void seek(LONG offset_lo);

	void print_geometry(void) const;
	void read_fat32(void);
};

#endif