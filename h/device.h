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

	ULONGLONG _offset = 0;

	void get_geometry(void);

public:
	~Device(void);

	static std::vector<wchar_t> get_drives(void);
	void open_drive(WCHAR drive);
	void close_drive(void);
	
	const PBYTE read(void);
	void seek(LONG offset_lo);

	void print_geometry(void) const;
	void read_fat32(void);
	
	inline const DISK_GEOMETRY & geometry(void) const { return _geometry; }
	inline ULONGLONG offset(void) const { return _offset; }
	const PBYTE buffer(void) const { return _buffer; }
};

#endif