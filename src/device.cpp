#include "device.h"
#include "layout.h"
#include <Windows.h>

#include <set>
#include <stdexcept>
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
	if (_device != INVALID_HANDLE_VALUE) {
		fprintf(_log, "device already open\n");
	} else {
		_last_open_drive = drive;

		WCHAR drive_path[] = L"\\\\.\\X:";
		drive_path[4] = drive;
		_device = CreateFileW(
			drive_path,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			0, //FILE_ATTRIBUTE_NORMAL,
			NULL
		);
		if (_device == INVALID_HANDLE_VALUE) {
			fprintf(_log, "device open error\n");
		} else {
			get_geometry();
			// dismount_drive();
			// lock_drive();
		}
	}
}

void Device::reopen_drive(void) {
	CloseHandle(_device);
	WCHAR drive_path[] = L"\\\\.\\X:";
	drive_path[4] = _last_open_drive;
	_device = CreateFileW(
		drive_path,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		0, //FILE_ATTRIBUTE_NORMAL,
		NULL
	);
	if (_device == INVALID_HANDLE_VALUE) { fprintf(_log, "device reopen error\n"); }
	seek(_offset, false);
}

void Device::dismount_drive(void)  {
	DWORD nbytes;
	BOOL status = DeviceIoControl (
		_device, FSCTL_DISMOUNT_VOLUME,
		NULL, 0,
		NULL, 0,
		&nbytes,
		(LPOVERLAPPED) NULL
	);
	if (status == FALSE) {
		print_error(L"\n\n\ndevice dismount error", GetLastError());
		throw std::runtime_error("device error");
	}
}

void Device::lock_drive(void)  {
	DWORD nbytes;
	BOOL locked = FALSE;
	//while (!locked) {
		locked = DeviceIoControl (
			_device, FSCTL_LOCK_VOLUME,
			NULL, 0, NULL, 0, &nbytes,
			(LPOVERLAPPED) NULL
		);
		if (locked == FALSE) {
			print_error(L"\n\n\ndevice lock error", GetLastError());
			throw std::runtime_error("device error");
		}

	//}
}

void Device::unlock_drive(void)  {
	DWORD nbytes;
	BOOL status = DeviceIoControl (
		_device, FSCTL_UNLOCK_VOLUME,
		NULL, 0,
		NULL, 0,
		&nbytes,
		(LPOVERLAPPED) NULL
	);
	if (status == FALSE) {
		print_error(L"\n\n\ndevice lock error", GetLastError());
		throw std::runtime_error("device error");
	}
}


void Device::close_drive(void) {
	if (this->_device == INVALID_HANDLE_VALUE) {
		fprintf(_log, "device not open\n");
	} else {
		delete[] _buffer[0];
		delete[] _buffer[1];
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
			_buffer[0] = new BYTE[_geometry.BytesPerSector];
			_buffer[1] = new BYTE[_geometry.BytesPerSector];
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

void Device::read(void) {
	for (int i = 0; i < 2; ++i) {
		BOOL status = ReadFile(_device, _buffer[i], _geometry.BytesPerSector, &_read_nbytes, NULL);
		if (status == FALSE) {
			print_error(L"\n\n\n\n\n\ndevice::read", GetLastError());
		}
	}

	// sets offset;
	seek(0, true);
}

void Device::seek(LONGLONG offset, bool relative) {
	LARGE_INTEGER lin;
	lin.QuadPart = offset;

	DWORD error = NO_ERROR;
	lin.LowPart = relative
		? SetFilePointer(_device, lin.LowPart, &lin.HighPart, FILE_CURRENT)
		: SetFilePointer(_device, lin.LowPart, &lin.HighPart, FILE_BEGIN);

	if (lin.LowPart == INVALID_SET_FILE_POINTER) { error = GetLastError(); }

	if (error == NO_ERROR) {
		_offset = lin.QuadPart;
	} else {
		print_error(L"\n\n\n\n\n\ndevice::seek", error);
	}
}

void Device::write(PBYTE buffer) {
	lock_drive();
	BOOL status = WriteFile(_device, buffer, _geometry.BytesPerSector, &_write_nbytes, NULL);
	if (status == FALSE) { print_error(LAYOUT_WFREE L"device::write(1)", GetLastError()); }
	seek(0, true);
	unlock_drive();
}

void Device::write(PBYTE buffers[2]) {
	lock_drive();
	BOOL status = WriteFile(_device, buffers[0], _geometry.BytesPerSector, &_write_nbytes, NULL);
	if (status == FALSE) { print_error(LAYOUT_WFREE L"device::write(1)", GetLastError()); }
	else {
		status = WriteFile(_device, buffers[1], _geometry.BytesPerSector, &_write_nbytes, NULL);
		if (status == FALSE) { print_error(LAYOUT_WFREE L"device::write(2)", GetLastError()); }
	}
	seek(0, true);
	//if (status == FALSE) { throw std::runtime_error("unable to write on device"); }
	unlock_drive();
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

