#ifndef ENTRY_SPEC_H
#define ENTRY_SPEC_H

struct entry_short {
	unsigned char  Name[11];
	unsigned char  Attr;
	unsigned char  NTRes;
	unsigned char  CrtTimeTenth;
	unsigned short CrtTime;
	unsigned short CrtDate;
	unsigned short LstAccDate;
	unsigned short FstClusHI;
	unsigned short WrtTime;
	unsigned short WrtDate;
	unsigned short FstClusLO;
	unsigned long  FileSize;
};

struct entry_long {
	unsigned char  Ord;
	unsigned char  Name1[10];
	unsigned char  Attr;
	unsigned char  Type;
	unsigned char  Chksum;
	unsigned char  Name2[12];
	unsigned short FstClusLO;
	unsigned char  Name3[4];
};


struct entry {
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
		entry_short DIR;
		entry_long  LDIR;
	};

	inline bool is_long(void) const { return (DIR.Attr & ATTR_LONG_NAME) == ATTR_LONG_NAME; }
	inline bool is_dir (void) const { return (DIR.Attr & ATTR_DIRECTORY) == ATTR_DIRECTORY; }
	inline bool is_file(void) const { return (DIR.Attr & ATTR_ARCHIVE  ) == ATTR_ARCHIVE  ; }

	inline entry() {}
	inline entry(entry*src);
};


entry::entry(entry * src) {
	memcpy(this, src, sizeof(entry));
}


// #define ENTRY_ATTR_READ_ONLY 0x01
// #define ENTRY_ATTR_HIDDEN    0x02
// #define ENTRY_ATTR_SYSTEM    0x04
// #define ENTRY_ATTR_VOLUME_ID 0x08
// #define ENTRY_ATTR_DIRECTORY 0x10
// #define ENTRY_ATTR_ARCHIVE   0x20
// #define ENTRY_ATTR_LONG_NAME ENTRY_ATTR_READ_ONLY | ENTRY_ATTR_HIDDEN | ENTRY_ATTR_SYSTEM | ENTRY_ATTR_VOLUME_ID

#endif
