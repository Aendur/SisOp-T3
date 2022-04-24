#include "fat32.h"
#include <cstdio>

void fat32::print(void) const {
	printf("BPB_FATSz16    : %u\n", BPB_FATSz16());
	printf("BPB_FATSz32    : %u\n", BPB_FATSz32());
	printf("BPB_NumFATs    : %u\n", BPB_NumFATs());
	printf("BPB_RsvdSecCnt : %u\n", BPB_RsvdSecCnt());
	printf("Bytes/sector   : %u\n", BPB_BytsPerSec());
	printf("Sectors/cluster: %u\n", BPB_SecPerClus());
	printf("BPB_RootClus   : %d\n", BPB_RootClus());
}

