#include "device.h"

#include <Windows.h>

using std::vector;

vector<char> Device::get_drives(void) {
	DWORD drives = GetLogicalDrives();
	
	vector<char> result;
	char letter = 'A';
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
	fwprintf(_out, L"Capacity          %llu\n", capacity);
	fwprintf(_out, L"NBytes            %d\n", _geom_nbytes);
}

void Device::open_device(char drive) {
	if (this->_device != INVALID_HANDLE_VALUE) {
		fprintf(_log, "device already open\n");
	} else {
		this->_device = CreateFileW(
			L"\\\\.\\g:",
			GENERIC_READ, // | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			0, //FILE_ATTRIBUTE_NORMAL,
			NULL
		);
		if (this->_device == INVALID_HANDLE_VALUE) { fprintf(_log, "device open error\n"); }
	}

}

void Device::close_device(void) {
	if (this->_device == INVALID_HANDLE_VALUE) { fprintf(_log, "device open error\n"); }
	else {
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
		}
	}
}

/*
int main() {
	fat32 sector0;
	status = ReadFile(device, &sector0, sizeof(sector0), &nbytes, NULL);
	print_buffer((PBYTE) &sector0, sizeof(sector0), nbytes);
	char str[10];
	snprintf(str, 9, (const char*) sector0.BS_FilSysType());
	printf("'%s'\n", str);
	printf("0x%02X  ", sector0.SigByte1());
	printf("0x%02X\n", sector0.SigByte2());
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



	// check INVALID_SET_FILE_POINTER
	DWORD sfp = SetFilePointer(device, FDS_offset - 512, NULL, FILE_BEGIN);
	if (sfp == INVALID_SET_FILE_POINTER) {
		if (!status) fprintf(stderr, "device seek error\n");
	}

	int npages = 2;
	PBYTE buffer = new BYTE[geom.BytesPerSector];
	while (status && npages-- > 0) {
		status = ReadFile(device, buffer, geom.BytesPerSector, &nbytes, NULL);
		if (!status) fprintf(stderr, "device read error\n");
		else         print_buffer(buffer, geom.BytesPerSector, nbytes);
	}
	delete[] buffer;

	CloseHandle(device);

	printf("entry size: %llu\n", sizeof(entry));
	return 0;
}
*/