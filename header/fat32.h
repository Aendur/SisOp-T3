#ifndef FAT32_SPEC_H
#define FAT32_SPEC_H

//typedef unsigned char byte;


class fat32 {
private:
	// buffer
	unsigned char sector[512];

public:
	// common
	const unsigned char* BS_jmpBoot    (void) const { return                    &sector[0]   ; } // Offset=0 Size=3
	const unsigned char* BS_OEMName    (void) const { return                    &sector[3]   ; } // Offset=3 Size=8
	const unsigned short BPB_BytsPerSec(void) const { return *((unsigned short*)&sector[11]) ; } // Offset=11 Size=2
	const unsigned char  BPB_SecPerClus(void) const { return                     sector[13]  ; } // Offset=13 Size=1
	const unsigned short BPB_RsvdSecCnt(void) const { return *((unsigned short*)&sector[14]) ; } // Offset=14 Size=2
	const unsigned char  BPB_NumFATs   (void) const { return                     sector[16]  ; } // Offset=16 Size=1
	const unsigned short BPB_RootEntCnt(void) const { return *((unsigned short*)&sector[17]) ; } // Offset=17 Size=2
	const unsigned short BPB_TotSec16  (void) const { return *((unsigned short*)&sector[19]) ; } // Offset=19 Size=2
	const unsigned char  BPB_Media     (void) const { return                     sector[21]  ; } // Offset=21 Size=1
	const unsigned short BPB_FATSz16   (void) const { return *((unsigned short*)&sector[22]) ; } // Offset=22 Size=2
	const unsigned short BPB_SecPerTrk (void) const { return *((unsigned short*)&sector[24]) ; } // Offset=24 Size=2
	const unsigned short BPB_NumHeads  (void) const { return *((unsigned short*)&sector[26]) ; } // Offset=26 Size=2
	const unsigned int   BPB_HiddSec   (void) const { return *((unsigned int*)  &sector[28]) ; } // Offset=28 Size=4
	const unsigned int   BPB_TotSec32  (void) const { return *((unsigned int*)  &sector[32]) ; } // Offset=32 Size=4
	// FAT32 specific
	const unsigned int   BPB_FATSz32   (void) const { return *((unsigned int*)  &sector[36]) ; } // Offset=36 Size=4
	const unsigned short BPB_ExtFlags  (void) const { return *((unsigned short*)&sector[40]) ; } // Offset=40 Size=2
	const unsigned short BPB_FSVer     (void) const { return *((unsigned short*)&sector[42]) ; } // Offset=42 Size=2
	const unsigned int   BPB_RootClus  (void) const { return *((unsigned int*)  &sector[44]) ; } // Offset=44 Size=4
	const unsigned short BPB_FSInfo    (void) const { return *((unsigned short*)&sector[48]) ; } // Offset=48 Size=2
	const unsigned short BPB_BkBootSec (void) const { return *((unsigned short*)&sector[50]) ; } // Offset=50 Size=2
	const unsigned char* BPB_Reserved  (void) const { return                    &sector[52]  ; } // Offset=52 Size=12
	const unsigned char  BS_DrvNum     (void) const { return                     sector[64]  ; } // Offset=64 Size=1
	const unsigned char  BS_Reserved1  (void) const { return                     sector[65]  ; } // Offset=65 Size=1
	const unsigned char  BS_BootSig    (void) const { return                     sector[66]  ; } // Offset=66 Size=1
	const unsigned int   BS_VolID      (void) const { return *((unsigned int*)  &sector[67]) ; } // Offset=67 Size=4
	const unsigned char* BS_VolLab     (void) const { return                    &sector[71]  ; } // Offset=71 Size=11
	const unsigned char* BS_FilSysType (void) const { return                    &sector[82]  ; } // Offset=82 Size=8
	const unsigned char  SigByte1      (void) const { return                     sector[510] ; } // Offset=510 Size=1
	const unsigned char  SigByte2      (void) const { return                     sector[511] ; } // Offset=511 Size=2
};

//char .* Size=(([2-9])|(1[1-9]+))

#endif
