#include "device.h"
#include "utility.h"

#include <Windows.h>

using std::vector;

vector<wchar_t> Device::get_drives(void) {
	DWORD drives = GetLogicalDrives();
	
	vector<wchar_t> result;
	wchar_t letter = L'A';
	for (int i = 0; i < sizeof(DWORD) * 2; ++i) {
		if (drives & 0x01) { result.push_back(letter); } ++letter;
		if (drives & 0x02) { result.push_back(letter); } ++letter;
		if (drives & 0x04) { result.push_back(letter); } ++letter;
		if (drives & 0x08) { result.push_back(letter); } ++letter;
		drives >>= 4;
	}

	return result;
}

void Device::print_geometry(void) const {
	unsigned long long capacity =
		_geometry.Cylinders.LowPart *
		_geometry.TracksPerCylinder *
		_geometry.SectorsPerTrack *
		_geometry.BytesPerSector;

	fwprintf(_out, L"MediaType         %d\n", /*MEDIA_TYPE*/ _geometry.MediaType);
	fwprintf(_out, L"Cylinders (Quad)  %lld\n", /*LARGE_INTEGER*/ _geometry.Cylinders.QuadPart);
	fwprintf(_out, L"Cylinders (High)  %d\n", /*LARGE_INTEGER*/ _geometry.Cylinders.HighPart);
	fwprintf(_out, L"Cylinders (Low)   %d\n", /*LARGE_INTEGER*/ _geometry.Cylinders.LowPart);
	fwprintf(_out, L"TracksPerCylinder %d\n", /*DWORD*/ _geometry.TracksPerCylinder);
	fwprintf(_out, L"SectorsPerTrack   %d\n", /*DWORD*/ _geometry.SectorsPerTrack);
	fwprintf(_out, L"BytesPerSector    %d\n", /*DWORD*/ _geometry.BytesPerSector);
	wchar_t sts[STS_MAX_FORMAT_SIZE];
	fwprintf(_out, L"Capacity          %llu B - %s\n", capacity, size_to_wstring(sts, capacity, true));
	fwprintf(_out, L"NBytes            %d\n", _geom_nbytes);
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
			GENERIC_READ, // | GENERIC_WRITE,
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
		}
	}
}

void Device::read(void) {
	//BOOL status = ReadFile(_device, &sector0, sizeof(sector0), &nbytes, NULL);
	BOOL status = ReadFile(_device, _buffer, _geometry.BytesPerSector, &_read_nbytes, NULL);
	if (!status) fprintf(stderr, "device read error\n");
}

void Device::seek(LONG offset_lo) {
	//DWORD sfp = SetFilePointer(_device, offset_lo, NULL, FILE_BEGIN);
	DWORD sfp = SetFilePointer(_device, offset_lo, NULL, FILE_CURRENT);
	if (sfp == INVALID_SET_FILE_POINTER) {
		fprintf(stderr, "device seek error\n");
	} else {
		_offset += offset_lo;
	}
}

/*
int main() {
	fat32 sector0;

	unsigned long cluster_size = sector0.BPB_BytsPerSec() * sector0.BPB_SecPerClus();
	
	printf("BPB_FATSz16:     %u\n", sector0.BPB_FATSz16());
	printf("BPB_FATSz32:     %u\n", sector0.BPB_FATSz32());
	printf("BPB_NumFATs:     %u\n", sector0.BPB_NumFATs());
	printf("BPB_RsvdSecCnt:  %u\n", sector0.BPB_RsvdSecCnt());
	printf("Bytes/sector:    %u\n", sector0.BPB_BytsPerSec());
	printf("Sectors/cluster: %u\n", sector0.BPB_SecPerClus());
	printf("Cluster size   : %u\n", cluster_size);
	size_t FirstDataSector = sector0.BPB_RsvdSecCnt() + sector0.BPB_FATSz32() * sector0.BPB_NumFATs();
	size_t FDS_offset = FirstDataSector * sector0.BPB_BytsPerSec();
	printf("FirstDataSector: %llu\n", FirstDataSector);
	printf("FDS offset     : %llu\n", FDS_offset);

	// size_t FirstSectorofCluster = ((N - 2) * sector0.BPB_SecPerClus()) + FirstDataSector
	// size_t FirstSectorofCluster = ((N - 2) * sector0.BPB_SecPerClus()) + FirstDataSector
	// SEEK
	//FDS_offset

	return 0;
}
*/