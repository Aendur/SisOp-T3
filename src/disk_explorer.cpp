#include "disk_explorer.h"

#include <Windows.h>

using std::vector;

vector<char> DiskExplorer::get_drives(void) {
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

//////////////////////////////
#include <cstdio>
#include <Windows.h>
#include "fat32.h"
#include "entry.h"


void print_geometry(const DISK_GEOMETRY * geom, DWORD nbytes) {
	unsigned long long capacity =
		geom->Cylinders.LowPart *
		geom->TracksPerCylinder *
		geom->SectorsPerTrack *
		geom->BytesPerSector;

	wprintf(L"MediaType         %d\n", /*MEDIA_TYPE*/ geom->MediaType);
	wprintf(L"Cylinders (Quad)  %lld\n", /*LARGE_INTEGER*/ geom->Cylinders.QuadPart);
	wprintf(L"Cylinders (High)  %d\n", /*LARGE_INTEGER*/ geom->Cylinders.HighPart);
	wprintf(L"Cylinders (Low)   %d\n", /*LARGE_INTEGER*/ geom->Cylinders.LowPart);
	wprintf(L"TracksPerCylinder %d\n", /*DWORD*/ geom->TracksPerCylinder);
	wprintf(L"SectorsPerTrack   %d\n", /*DWORD*/ geom->SectorsPerTrack);
	wprintf(L"BytesPerSector    %d\n", /*DWORD*/ geom->BytesPerSector);
	wprintf(L"Capacity          %llu\n", capacity);
	wprintf(L"NBytes            %d\n", nbytes);
}

void print_hdr(void) {
	printf("\033[2m");
	printf("-----------|-------------------------------------------------------------------------------------------------------|----------------------------------\n");
	printf("  Address  |  00 01 02 03 04 05 06 07  08 09 0A 0B 0C 0D 0E 0F   10 11 12 13 14 15 16 17  18 19 1A 1B 1C 1D 1E 1F  |                Text\n");
	printf("-----------|-------------------------------------------------------------------------------------------------------|----------------------------------\n");
}

void print_adr(long long offset) {
	printf("\033[2m  %07llx  |  \033[0m", offset);
}

void print_hex(PBYTE line, int len) {
	static const int q1 = 0x08;
	static const int q2 = 0x10;
	static const int q3 = 0x18;
	for (int i =  0; i < q1; ++i) { printf("%02x ", line[i]); }
	for (int i = q1; i < q2; ++i) { printf(" %02x", line[i]); }
	printf("   ");
	for (int i = q2; i < q3; ++i)  { printf("%02x ", line[i]); }
	for (int i = q3; i < len; ++i) { printf(" %02x", line[i]); }
	printf("\033[2m  |  \033[0m");
}

void print_str(PBYTE line, int len) {
	for (int i = 0x00; i < len; ++i) {
		char c = line[i];
		if (0x20 <= c && c <= 0x7E) {
			printf("%c", c);
		} else if (c == 0) {
			printf("\033[31;2m.\033[0m");
		} else {
			printf("\033[31;1m.\033[0m");
		}
	}
	printf("\n");
}

#define LINE_WIDTH 0x20
void print_buffer(PBYTE buffer, DWORD len, DWORD nbytes) {
	PBYTE end = buffer + len;
	PBYTE pos = buffer;
	printf("buffer length: %d\n", len);
	printf("read bytes: %d\n", nbytes);
	
	print_hdr();
	while(pos < end) {
		print_adr(pos - buffer);
		print_hex(pos, LINE_WIDTH);
		print_str(pos, LINE_WIDTH);
		pos += LINE_WIDTH;
	}
}

int main() {
	
	HANDLE device = CreateFileW(
		L"\\\\.\\g:",
		GENERIC_READ, // | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		0, //FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (device == INVALID_HANDLE_VALUE) {
		fprintf(stderr, "device open error\n");
		return 0;
	}

	DISK_GEOMETRY geom;
	DWORD nbytes;
	BOOL status;
	status = DeviceIoControl(
		device, IOCTL_DISK_GET_DRIVE_GEOMETRY,
		NULL, 0,
		&geom, sizeof(geom),
		&nbytes, (LPOVERLAPPED) NULL
	);
	if (status) {
		print_geometry(&geom, nbytes);
	} else {
		CloseHandle(device);
		fprintf(stderr, "device io ctl error\n");
		return 0;
	}

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