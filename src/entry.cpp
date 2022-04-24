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

#define ASH_VBAR "\033[2m|\033[0m"
#define ASH_HBAR1 "\033[2m+-------------+\033[0m"
#define ASH_HBAR2 "\033[2m+-----------+-------------+\033[0m"

void print_str(const unsigned char * str, int len) {
	for (int i = 0; i < len; ++i) {
		char c = str[i];
		printf("%s", colorize_char(c, '.'));
	}
	printf(" " ASH_VBAR " ");
}

void entry::print(int x, int y, bool extended, bool labels) const {
	if (labels) {
			printf("\033[%d;%dH" ASH_HBAR2                                             , y++, x);
			printf("\033[%d;%dH" ASH_VBAR " Name      " ASH_VBAR " "                   , y++, x); print_str(DIR_Name, 11);
			printf("\033[%d;%dH" ASH_VBAR " Attr      " ASH_VBAR " 0x%02X%8s" ASH_VBAR , y++, x, DIR_Attr, "");
		if (extended) {
			printf("\033[%d;%dH" ASH_VBAR " NTRes     " ASH_VBAR " 0x%02X%8s" ASH_VBAR , y++, x, DIR_NTRes, "");     // extended
			printf("\033[%d;%dH" ASH_VBAR " CrtTimeTen" ASH_VBAR " %-11u " ASH_VBAR    , y++, x, DIR_CrtTimeTenth);   // extended
			printf("\033[%d;%dH" ASH_VBAR " CrtTime   " ASH_VBAR " %-11u " ASH_VBAR    , y++, x, DIR_CrtTime);        // extended
			printf("\033[%d;%dH" ASH_VBAR " CrtDate   " ASH_VBAR " %-11u " ASH_VBAR    , y++, x, DIR_CrtDate);        // extended
			printf("\033[%d;%dH" ASH_VBAR " LstAccDate" ASH_VBAR " %-11u " ASH_VBAR    , y++, x, DIR_LstAccDate);     // extended
		}   printf("\033[%d;%dH" ASH_VBAR " FstClusHI " ASH_VBAR " %-11u " ASH_VBAR    , y++, x, DIR_FstClusHI);
		if (extended) {
			printf("\033[%d;%dH" ASH_VBAR " WrtTime   " ASH_VBAR " %-11u " ASH_VBAR    , y++, x, DIR_WrtTime);        // extended
			printf("\033[%d;%dH" ASH_VBAR " WrtDate   " ASH_VBAR " %-11u " ASH_VBAR    , y++, x, DIR_WrtDate);        // extended
		}   printf("\033[%d;%dH" ASH_VBAR " FstClusLO " ASH_VBAR " %-11u " ASH_VBAR    , y++, x, DIR_FstClusLO);
			printf("\033[%d;%dH" ASH_VBAR " FileSize  " ASH_VBAR " %-11lu " ASH_VBAR   , y++, x, DIR_FileSize);
			printf("\033[%d;%dH" ASH_HBAR2                                             , y++, x);
	} else {
			printf("\033[%d;%dH" ASH_HBAR1           , y++, x);
			printf("\033[%d;%dH" ASH_VBAR " "                   , y++, x); print_str(DIR_Name, 11);
			printf("\033[%d;%dH" ASH_VBAR " 0x%02X%8s" ASH_VBAR , y++, x, DIR_Attr, "");
		if (extended) {
			printf("\033[%d;%dH" ASH_VBAR " 0x%02X%8s" ASH_VBAR , y++, x, DIR_NTRes, "");     // extended
			printf("\033[%d;%dH" ASH_VBAR " %-11u " ASH_VBAR    , y++, x, DIR_CrtTimeTenth);   // extended
			printf("\033[%d;%dH" ASH_VBAR " %-11u " ASH_VBAR    , y++, x, DIR_CrtTime);        // extended
			printf("\033[%d;%dH" ASH_VBAR " %-11u " ASH_VBAR    , y++, x, DIR_CrtDate);        // extended
			printf("\033[%d;%dH" ASH_VBAR " %-11u " ASH_VBAR    , y++, x, DIR_LstAccDate);     // extended
		}   printf("\033[%d;%dH" ASH_VBAR " %-11u " ASH_VBAR    , y++, x, DIR_FstClusHI);
		if (extended) {
			printf("\033[%d;%dH" ASH_VBAR " %-11u " ASH_VBAR    , y++, x, DIR_WrtTime);        // extended
			printf("\033[%d;%dH" ASH_VBAR " %-11u " ASH_VBAR    , y++, x, DIR_WrtDate);        // extended
		}   printf("\033[%d;%dH" ASH_VBAR " %-11u " ASH_VBAR    , y++, x, DIR_FstClusLO);
			printf("\033[%d;%dH" ASH_VBAR " %-11lu " ASH_VBAR   , y++, x, DIR_FileSize);
			printf("\033[%d;%dH" ASH_HBAR1           , y++, x);

	}
}

// enum attr {
// 	ATTR_READ_ONLY  = 0x01,
// 	ATTR_HIDDEN     = 0x02,
// 	ATTR_SYSTEM     = 0x04,
// 	ATTR_VOLUME_ID  = 0x08,
// 	ATTR_DIRECTORY  = 0x10,
// 	ATTR_ARCHIVE    = 0x20,
// 	ATTR_LONG_NAME  = ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID,
// };
