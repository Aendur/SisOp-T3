#ifndef DEVICE_H
#define DEVICE_H


#include <Windows.h>
#include <cstdio>

class Device {
private:
	FILE * _out = stdout;
	FILE * _log = stderr;
	HANDLE _device = INVALID_HANDLE_VALUE;
	DISK_GEOMETRY _geometry;
	PBYTE _buffer[2];
	DWORD _geom_nbytes;
	DWORD _read_nbytes;
	DWORD _write_nbytes;

	LONGLONG _offset = -1;
	LONGLONG _capacity = -1;

	void get_geometry(void);

public:
	~Device(void);

	static bool check_drive(WCHAR drive);
	void open_drive(WCHAR drive);
	void close_drive(void);
	
	void read(void);
	void seek(LONGLONG offset, bool relative);
	void write(PBYTE buffers[2]);

	inline const DISK_GEOMETRY & geometry(void) const { return _geometry; }
	inline LONGLONG offset(void) const { return _offset; }
	inline LONGLONG capacity(void) const { return _capacity; }
	const PBYTE buffer(int i) const { return i <= 0 ? _buffer[0] : _buffer[1]; }
	const PBYTE * buffers(void) const { return _buffer; }
};

#endif