#ifndef ENTRY_SPEC_H
#define ENTRY_SPEC_H

#define ENTRY_ATTR_READ_ONLY 0x01
#define ENTRY_ATTR_HIDDEN    0x02
#define ENTRY_ATTR_SYSTEM    0x04
#define ENTRY_ATTR_VOLUME_ID 0x08
#define ENTRY_ATTR_DIRECTORY 0x10
#define ENTRY_ATTR_ARCHIVE   0x20
#define ENTRY_ATTR_LONG_NAME ENTRY_ATTR_READ_ONLY | ENTRY_ATTR_HIDDEN | ENTRY_ATTR_SYSTEM | ENTRY_ATTR_VOLUME_ID

class entry {
private:
	// buffer
	//unsigned char sector[512];

public:
	unsigned char  DIR_Name[11];
	unsigned char  DIR_Attr;
	unsigned char  DIR_NTRes;
	unsigned char  DIR_CrtTimeTenth;
	unsigned short DIR_CrtTime;
	unsigned short DIR_CrtDate;
	unsigned short DIR_LstAccDate;
	unsigned short DIR_FstClusHI;
	unsigned short DIR_WrtTime;
	unsigned short DIR_WrtDate;
	unsigned short DIR_FstClusLO;
	unsigned int   DIR_FileSize;
};

//char .* Size=(([2-9])|(1[1-9]+))

#endif
