#ifndef FAT32_SPEC_H
#define FAT32_SPEC_H


// #include <winnt.h>
#include <Windows.h>

enum FAT_Code : unsigned int {
	FAT_RESERVED1 = 0x0FFFFFF8,
	FAT_RESERVED2 = 0x7FFFFFFF,
	FAT_EOC       = 0x0FFFFFFF,
	FAT_EMPTY     = 0x00000000,
};

struct fat32 {
private:
	// buffer
	unsigned char sector[512];

public:
	// common
	inline const unsigned char* BS_jmpBoot    (void) const { return                    &sector[0]   ; } // Offset=0 Size=3
	inline const unsigned char* BS_OEMName    (void) const { return                    &sector[3]   ; } // Offset=3 Size=8
	inline const unsigned short BPB_BytsPerSec(void) const { return *((unsigned short*)&sector[11]) ; } // Offset=11 Size=2
	inline const unsigned char  BPB_SecPerClus(void) const { return                     sector[13]  ; } // Offset=13 Size=1
	inline const unsigned short BPB_RsvdSecCnt(void) const { return *((unsigned short*)&sector[14]) ; } // Offset=14 Size=2
	inline const unsigned char  BPB_NumFATs   (void) const { return                     sector[16]  ; } // Offset=16 Size=1
	inline const unsigned short BPB_RootEntCnt(void) const { return *((unsigned short*)&sector[17]) ; } // Offset=17 Size=2
	inline const unsigned short BPB_TotSec16  (void) const { return *((unsigned short*)&sector[19]) ; } // Offset=19 Size=2
	inline const unsigned char  BPB_Media     (void) const { return                     sector[21]  ; } // Offset=21 Size=1
	inline const unsigned short BPB_FATSz16   (void) const { return *((unsigned short*)&sector[22]) ; } // Offset=22 Size=2
	inline const unsigned short BPB_SecPerTrk (void) const { return *((unsigned short*)&sector[24]) ; } // Offset=24 Size=2
	inline const unsigned short BPB_NumHeads  (void) const { return *((unsigned short*)&sector[26]) ; } // Offset=26 Size=2
	inline const unsigned int   BPB_HiddSec   (void) const { return *((unsigned int*)  &sector[28]) ; } // Offset=28 Size=4
	inline const unsigned int   BPB_TotSec32  (void) const { return *((unsigned int*)  &sector[32]) ; } // Offset=32 Size=4
	// FAT32 specific
	inline const unsigned int   BPB_FATSz32   (void) const { return *((unsigned int*)  &sector[36]) ; } // Offset=36 Size=4
	inline const unsigned short BPB_ExtFlags  (void) const { return *((unsigned short*)&sector[40]) ; } // Offset=40 Size=2
	inline const unsigned short BPB_FSVer     (void) const { return *((unsigned short*)&sector[42]) ; } // Offset=42 Size=2
	inline const unsigned int   BPB_RootClus  (void) const { return *((unsigned int*)  &sector[44]) ; } // Offset=44 Size=4
	inline const unsigned short BPB_FSInfo    (void) const { return *((unsigned short*)&sector[48]) ; } // Offset=48 Size=2
	inline const unsigned short BPB_BkBootSec (void) const { return *((unsigned short*)&sector[50]) ; } // Offset=50 Size=2
	inline const unsigned char* BPB_Reserved  (void) const { return                    &sector[52]  ; } // Offset=52 Size=12
	inline const unsigned char  BS_DrvNum     (void) const { return                     sector[64]  ; } // Offset=64 Size=1
	inline const unsigned char  BS_Reserved1  (void) const { return                     sector[65]  ; } // Offset=65 Size=1
	inline const unsigned char  BS_BootSig    (void) const { return                     sector[66]  ; } // Offset=66 Size=1
	inline const unsigned int   BS_VolID      (void) const { return *((unsigned int*)  &sector[67]) ; } // Offset=67 Size=4
	inline const unsigned char* BS_VolLab     (void) const { return                    &sector[71]  ; } // Offset=71 Size=11
	inline const unsigned char* BS_FilSysType (void) const { return                    &sector[82]  ; } // Offset=82 Size=8
	inline const unsigned char  SigByte1      (void) const { return                     sector[510] ; } // Offset=510 Size=1
	inline const unsigned char  SigByte2      (void) const { return                     sector[511] ; } // Offset=511 Size=2
	
	inline ULONG cluster_size(void) const;
	inline ULONG first_data_sector(void) const;
	inline ULONGLONG fds_offset(void) const;
	inline ULONGLONG first_sector_of_cluster(ULONGLONG N) const;
	inline ULONGLONG fat_sec_num(ULONGLONG N, int nfat) const;
	inline ULONGLONG fat_ent_off(ULONGLONG N) const;
	inline ULONG n_fat_entries(void) const;
	inline ULONG n_clusters(void) const;
};

ULONG fat32::cluster_size(void) const {
	return this->BPB_BytsPerSec() * this->BPB_SecPerClus();
}
ULONG fat32::first_data_sector(void) const {
	return this->BPB_RsvdSecCnt() + this->BPB_FATSz32() * this->BPB_NumFATs();
}
ULONGLONG fat32::fds_offset(void) const {
	return first_data_sector() * this->BPB_BytsPerSec();
}
ULONGLONG fat32::first_sector_of_cluster(ULONGLONG N) const {
	return ((N - 2) * this->BPB_SecPerClus()) + first_data_sector();
}
ULONGLONG fat32::fat_sec_num(ULONGLONG N, int nfat) const {
	unsigned int fatsz = this->BPB_FATSz32();
	ULONGLONG fat_offset = N * 4;
	return this->BPB_RsvdSecCnt() + (nfat * fatsz) + (fat_offset / this->BPB_BytsPerSec());
}
ULONGLONG fat32::fat_ent_off(ULONGLONG N) const {
	//unsigned int fatsz = this->BPB_FATSz32();
	ULONGLONG fat_offset = N * 4;
	return fat_offset % this->BPB_BytsPerSec();
}
ULONG fat32::n_fat_entries(void) const {
	return BPB_FATSz32() * ((size_t) BPB_BytsPerSec() / sizeof(ULONG));
}

ULONG fat32::n_clusters(void) const {
	return (BPB_TotSec32() - first_data_sector()) / BPB_SecPerClus();
}


#endif
