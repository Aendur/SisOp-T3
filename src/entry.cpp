#include "entry.h"
#include "utility.h"
#include <cstdio>
#include <cstring>
//	unsigned char  DIR_Name[11];
//	unsigned char  DIR_Attr;
//	unsigned char  DIR_NTRes;
//	unsigned char  DIR_CrtTimeTenth;
//	unsigned short DIR_CrtTime;
//	unsigned short DIR_CrtDate;
//	unsigned short DIR_LstAccDate;
//	unsigned short DIR_FstClusHI;
//	unsigned short DIR_WrtTime;
//	unsigned short DIR_WrtDate;
//	unsigned short DIR_FstClusLO;
//	unsigned long  DIR_FileSize;

//#define ASH_VBAR "\033[2m|\033[0m"
#define ASH_VBAR "\033[2m|\033[0m"

#define ASH_ELEA "\033[%d;53H\033[2m| %X \033[0m"
#define ASH_EBAR "\033[%d;53H\033[2m+---+-------------+------+------+-----+-------+-------+-------+-------+-------+-------+-------+------------+\033[0m\033[0K\033[25;1H"
#define ASH_ELAB "\033[%d;53H\033[2m|   |    NAME     | ATTR | NTR  | Cms | CTime | CDate | ADate | ClusH | WTime | WDate | ClusL | Size       |\033[0m\033[0K"
#define ASH_ELIN "\033[%d;53H\033[2m| 0 | .X.MSDOS5.0 | 0x00 | 0x02 | 16  | 6280  | 2     | 0     | 63488 | 0     | 63    | 255   | 128        |\033[0m\033[0K"

#define ASH_SLEA "\033[%d;53H\033[2m| %X \033[0m"
#define ASH_SBAR "\033[%d;53H\033[2m+---+-------------+------+-------+-------+------------+\033[0m\033[0K\033[25;1H"
#define ASH_SLAB "\033[%d;53H\033[2m|   |    NAME     | ATTR | ClusH | ClusL | Size       |\033[0m\033[0K"
#define ASH_SLIN "\033[%d;53H\033[2m| 0 | RRaA....... | 0x00 | 0     | 0     | 0          |\033[0m\033[0K"

void print_str(const unsigned char * str, int len) {
	for (int i = 0; i < len; ++i) {
		char c = str[i];
		printf("%s", colorize_char(c, '.'));
	}
	//printf(" " ASH_VBAR " ");
}



void entry::print(void*addr, bool extended) {
	entry * entries = (entry*) addr;
	const int Y0 = 27;
	int i = 0;
	if (extended) {
		printf(ASH_EBAR, Y0-3);
		printf(ASH_ELAB, Y0-2);
		printf(ASH_EBAR, Y0-1);
		for (i = 0; i < 16; ++i) {
			printf(ASH_ELEA, i + Y0, i);
			entries[i].print(extended);
		}
		printf(ASH_EBAR, i+Y0);
	} else {
		printf(ASH_SBAR, Y0-3);
		printf(ASH_SLAB, Y0-2);
		printf(ASH_SBAR, Y0-1);
		for (i = 0; i < 16; ++i) {
			printf(ASH_SLEA, i + Y0, i);
			entries[i].print(extended);
		}
		printf(ASH_SBAR, i+Y0);
	}
}

void entry::print(bool extended) const {
	printf(ASH_VBAR " ");
	print_str(DIR_Name, 11);
	if (extended) {
		printf(" "     ASH_VBAR
			" 0x%02X " ASH_VBAR // DIR_Attr
			" 0x%02X " ASH_VBAR // DIR_NTRes
			" %-3u "   ASH_VBAR // DIR_CrtTimeTenth
			" %-5u "   ASH_VBAR // DIR_CrtTime
			" %-5u "   ASH_VBAR // DIR_CrtDate
			" %-5u "   ASH_VBAR // DIR_LstAccDate
			" %-5u "   ASH_VBAR // DIR_FstClusHI
			" %-5u "   ASH_VBAR // DIR_WrtTime
			" %-5u "   ASH_VBAR // DIR_WrtDate
			" %-5u "   ASH_VBAR // DIR_FstClusLO
			" %-10lu " ASH_VBAR // DIR_FileSize
			, DIR_Attr, DIR_NTRes, DIR_CrtTimeTenth, DIR_CrtTime, DIR_CrtDate, DIR_LstAccDate, DIR_FstClusHI, DIR_WrtTime, DIR_WrtDate, DIR_FstClusLO, DIR_FileSize
		);
	} else {
		printf(" "     ASH_VBAR
			" 0x%02X " ASH_VBAR // DIR_Attr
			" %-5u "   ASH_VBAR // DIR_FstClusHI
			" %-5u "   ASH_VBAR // DIR_FstClusLO
			" %-10lu " ASH_VBAR // DIR_FileSize
			, DIR_Attr, DIR_FstClusHI, DIR_FstClusLO, DIR_FileSize
		);
	}
	printf("\033[0K");
}



// void entry::print(int x, int y, bool extended, bool labels) const {
// 	if (labels) {
// 			printf("\033[%d;%dH" ASH_HBAR2                                             , y++, x);
// 			printf("\033[%d;%dH" ASH_VBAR " Name      " ASH_VBAR " "                   , y++, x); print_str(DIR_Name, 11);
// 			printf("\033[%d;%dH" ASH_VBAR " Attr      " ASH_VBAR " 0x%02X%8s" ASH_VBAR , y++, x, DIR_Attr, "");
// 		if (extended) {
// 			printf("\033[%d;%dH" ASH_VBAR " NTRes     " ASH_VBAR " 0x%02X%8s" ASH_VBAR , y++, x, DIR_NTRes, "");     // extended
// 			printf("\033[%d;%dH" ASH_VBAR " CrtTimeTen" ASH_VBAR " %-11u " ASH_VBAR    , y++, x, DIR_CrtTimeTenth);   // extended
// 			printf("\033[%d;%dH" ASH_VBAR " CrtTime   " ASH_VBAR " %-11u " ASH_VBAR    , y++, x, DIR_CrtTime);        // extended
// 			printf("\033[%d;%dH" ASH_VBAR " CrtDate   " ASH_VBAR " %-11u " ASH_VBAR    , y++, x, DIR_CrtDate);        // extended
// 			printf("\033[%d;%dH" ASH_VBAR " LstAccDate" ASH_VBAR " %-11u " ASH_VBAR    , y++, x, DIR_LstAccDate);     // extended
// 		}   printf("\033[%d;%dH" ASH_VBAR " FstClusHI " ASH_VBAR " %-11u " ASH_VBAR    , y++, x, DIR_FstClusHI);
// 		if (extended) {
// 			printf("\033[%d;%dH" ASH_VBAR " WrtTime   " ASH_VBAR " %-11u " ASH_VBAR    , y++, x, DIR_WrtTime);        // extended
// 			printf("\033[%d;%dH" ASH_VBAR " WrtDate   " ASH_VBAR " %-11u " ASH_VBAR    , y++, x, DIR_WrtDate);        // extended
// 		}   printf("\033[%d;%dH" ASH_VBAR " FstClusLO " ASH_VBAR " %-11u " ASH_VBAR    , y++, x, DIR_FstClusLO);
// 			printf("\033[%d;%dH" ASH_VBAR " FileSize  " ASH_VBAR " %-11lu " ASH_VBAR   , y++, x, DIR_FileSize);
// 			printf("\033[%d;%dH" ASH_HBAR2                                             , y++, x);
// 	} else {
// 			printf("\033[%d;%dH" ASH_HBAR1           , y++, x);
// 			printf("\033[%d;%dH" ASH_VBAR " "                   , y++, x); print_str(DIR_Name, 11);
// 			printf("\033[%d;%dH" ASH_VBAR " 0x%02X%8s" ASH_VBAR , y++, x, DIR_Attr, "");
// 		if (extended) {
// 			printf("\033[%d;%dH" ASH_VBAR " 0x%02X%8s" ASH_VBAR , y++, x, DIR_NTRes, "");     // extended
// 			printf("\033[%d;%dH" ASH_VBAR " %-11u " ASH_VBAR    , y++, x, DIR_CrtTimeTenth);   // extended
// 			printf("\033[%d;%dH" ASH_VBAR " %-11u " ASH_VBAR    , y++, x, DIR_CrtTime);        // extended
// 			printf("\033[%d;%dH" ASH_VBAR " %-11u " ASH_VBAR    , y++, x, DIR_CrtDate);        // extended
// 			printf("\033[%d;%dH" ASH_VBAR " %-11u " ASH_VBAR    , y++, x, DIR_LstAccDate);     // extended
// 		}   printf("\033[%d;%dH" ASH_VBAR " %-11u " ASH_VBAR    , y++, x, DIR_FstClusHI);
// 		if (extended) {
// 			printf("\033[%d;%dH" ASH_VBAR " %-11u " ASH_VBAR    , y++, x, DIR_WrtTime);        // extended
// 			printf("\033[%d;%dH" ASH_VBAR " %-11u " ASH_VBAR    , y++, x, DIR_WrtDate);        // extended
// 		}   printf("\033[%d;%dH" ASH_VBAR " %-11u " ASH_VBAR    , y++, x, DIR_FstClusLO);
// 			printf("\033[%d;%dH" ASH_VBAR " %-11lu " ASH_VBAR   , y++, x, DIR_FileSize);
// 			printf("\033[%d;%dH" ASH_HBAR1           , y++, x);
// 	}
// }

// enum attr {
// 	ATTR_READ_ONLY  = 0x01,
// 	ATTR_HIDDEN     = 0x02,
// 	ATTR_SYSTEM     = 0x04,
// 	ATTR_VOLUME_ID  = 0x08,
// 	ATTR_DIRECTORY  = 0x10,
// 	ATTR_ARCHIVE    = 0x20,
// 	ATTR_LONG_NAME  = ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID,
// };
