#include "device.h"
#include <Windows.h>

#include <set>
using std::set;

void print_error(const wchar_t *, DWORD);

bool Device::check_drive(WCHAR drive) {
	DWORD drives = GetLogicalDrives();
	
	set<WCHAR> result;
	set<WCHAR> RESULT;
	WCHAR letter = L'A';
	for (int i = 0; i < sizeof(DWORD) * 8; ++i) {
		if (drives & 0x01) {
			WCHAR LETTER = letter + 0x20;
			result.insert(letter);
			RESULT.insert(letter);
			RESULT.insert(LETTER);
		}
		++letter;
		drives >>= 1;
	}
	
	if (RESULT.find(drive) == RESULT.end()) {
		printf("Available drives: ");
		for (WCHAR dri : result) { wprintf(L"%c: ", dri); }
		printf("\n");
		return false;
	} else {
		return true;
	}
}

Device::~Device(void) {
	this->close_drive();
}

void Device::open_drive(wchar_t drive) {
	if (this->_device != INVALID_HANDLE_VALUE) {
		fprintf(_log, "device already open\n");
	} else {
		WCHAR drive_path[] = L"\\\\.\\X:";
		drive_path[4] = drive;
		/*for(int i = 0; i < 7; ++i) {
			fwprintf(_log, L"%c\n", drive_path[i]);
		}*/

		this->_device = CreateFileW(
			drive_path,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			0, //FILE_ATTRIBUTE_NORMAL,
			NULL
		);
		if (this->_device == INVALID_HANDLE_VALUE) {
			fprintf(_log, "device open error\n");
		} else {
			get_geometry();
		}
	}
}

void Device::close_drive(void) {
	if (this->_device == INVALID_HANDLE_VALUE) {
		fprintf(_log, "device not open\n");
	} else {
		delete[] _buffer;
		CloseHandle(this->_device);
		this->_device = INVALID_HANDLE_VALUE;
	}
}

void Device::get_geometry(void) {
	if (this->_device == INVALID_HANDLE_VALUE) { fprintf(_log, "device open error\n"); }
	else {
		BOOL status = DeviceIoControl(
			_device, IOCTL_DISK_GET_DRIVE_GEOMETRY,
			NULL, 0,
			&_geometry, sizeof(_geometry),
			&_geom_nbytes, (LPOVERLAPPED) NULL
		);
		if (!status) {
			fprintf(_log, "device io ctl geometry error\n");
			close_drive();
		} else {
			_buffer = new BYTE[_geometry.BytesPerSector];
			_offset = 0;
			_capacity = (LONGLONG) (
				_geometry.Cylinders.LowPart *
				_geometry.TracksPerCylinder *
				_geometry.SectorsPerTrack   *
				_geometry.BytesPerSector
			);
		}
	}
}

const PBYTE Device::read(void) {
	BOOL status = ReadFile(_device, _buffer, _geometry.BytesPerSector, &_read_nbytes, NULL);
	if (!status) {
		fprintf(stderr, "device read error\n");
		return NULL;
	} else {
		_offset += _geometry.BytesPerSector;
		return _buffer;
	}
}
//0x00000000FFFFFE00
void Device::seek(LONGLONG offset, bool relative) {
	LARGE_INTEGER lin;
	lin.QuadPart = offset;

	DWORD error = NO_ERROR;
	DWORD status = relative
		? SetFilePointer(_device, lin.LowPart, &lin.HighPart, FILE_CURRENT)
		: SetFilePointer(_device, lin.LowPart, &lin.HighPart, FILE_BEGIN);

	if (status == INVALID_SET_FILE_POINTER) { error = GetLastError(); }

	if (error == NO_ERROR) {
		_offset = relative
			? _offset + offset
			: offset;
	} else {
		print_error(L"\n\n\n\n\n\ndevice::seek", error);
	}
}

void print_error(const wchar_t * msg, DWORD error) {
	LPVOID msg_buffer;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		error,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &msg_buffer,
		0, NULL
	);

	wprintf(L"%s : %hs\n", msg, (LPTSTR) msg_buffer);
	LocalFree(msg_buffer);
}

