#ifndef ENTRY_SPEC_H
#define ENTRY_SPEC_H

// #define ENTRY_ATTR_READ_ONLY 0x01
// #define ENTRY_ATTR_HIDDEN    0x02
// #define ENTRY_ATTR_SYSTEM    0x04
// #define ENTRY_ATTR_VOLUME_ID 0x08
// #define ENTRY_ATTR_DIRECTORY 0x10
// #define ENTRY_ATTR_ARCHIVE   0x20
// #define ENTRY_ATTR_LONG_NAME ENTRY_ATTR_READ_ONLY | ENTRY_ATTR_HIDDEN | ENTRY_ATTR_SYSTEM | ENTRY_ATTR_VOLUME_ID

struct entry_short {
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
	unsigned long  DIR_FileSize;
};

struct entry_long {
	unsigned char  LDIR_Ord;
	unsigned char  LDIR_Name1[10];
	unsigned char  LDIR_Attr;
	unsigned char  LDIR_Type;
	unsigned char  LDIR_Chksum;
	unsigned char  LDIR_Name2[12];
	unsigned short LDIR_FstClusLO;
	unsigned char  LDIR_Name3[4];
};


class entry {
public:
	enum attr {
		ATTR_READ_ONLY  = 0x01,
		ATTR_HIDDEN     = 0x02,
		ATTR_SYSTEM     = 0x04,
		ATTR_VOLUME_ID  = 0x08,
		ATTR_DIRECTORY  = 0x10,
		ATTR_ARCHIVE    = 0x20,
		ATTR_LONG_NAME  = ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID,
	};

	union {
		entry_short ds;
		entry_long  dl;
	} dir;

	static void print(void * addr, bool extended);

private:
	void print_short(bool extended) const;
	void print_long(bool extended) const;

	inline bool is_long(void) const { return (dir.ds.DIR_Attr & ATTR_LONG_NAME) == ATTR_LONG_NAME; }
	inline bool is_dir(void) const  { return (dir.ds.DIR_Attr & ATTR_DIRECTORY) == ATTR_DIRECTORY; }
};

#endif
