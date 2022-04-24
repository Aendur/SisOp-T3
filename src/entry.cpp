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

// unsigned char  LDIR_Ord;
// unsigned char  LDIR_Name1[10];
// unsigned char  LDIR_Attr;
// unsigned char  LDIR_Type;
// unsigned char  LDIR_Chksum;
// unsigned char  LDIR_Name2[12];
// unsigned short LDIR_FstClusLO;
// unsigned char  LDIR_Name3[4];


// #define ASH_ELEA "\033[%d;53H\033[2m| %X \033[0m"
// #define ASH_EBAR "\033[%d;53H\033[2m+---+-------------+-----+-----+-----+-------+-------+-------+-------+-------+-------+-------+------------+\033[0m\033[0K\033[25;1H"
// #define ASH_ELAB "\033[%d;53H\033[2m|   |    Name     | ATT | NTR | Cms | CTime | CDate | ADate | ClusH | WTime | WDate | ClusL | Size       |\033[0m\033[0K"
// #define ASH_ELIN "\033[%d;53H\033[2m| 0 | .X.MSDOS5.0 |  00 |  02 | 16  | 6280  | 2     | 0     | 63488 | 0     | 63    | 255   | 128        |\033[0m\033[0K"

// #define ASH_LLEA "\033[%d;53H\033[2m| %X \033[0m"
// #define ASH_LBAR "\033[%d;53H\033[2m+---+-----+------------+-----+-----+-----+-------------+-------+------+\033[0m\033[0K\033[25;1H"
// #define ASH_LLAB "\033[%d;53H\033[2m|   | Ord |   Name1    | ATT | Typ | CKS | Name2       | ClusL | Nam3 |\033[0m\033[0K"
// #define ASH_LLIN "\033[%d;53H\033[2m| 1 | E5  |  .I.n.f.o. | 0F  | 00  | 72  | r.m.a.t.i.o.| 0     | n... |\033[0m\033[0K"

// #define ASH_SLEA "\033[%d;53H\033[2m| %X \033[0m"
// #define ASH_SBAR "\033[%d;53H\033[2m+---+-------------+-----+-------+-------+------------+\033[0m\033[0K\033[25;1H"
// #define ASH_SLAB "\033[%d;53H\033[2m|   |    NAME     | ATT | ClusH | ClusL | Size       |\033[0m\033[0K"
// #define ASH_SLIN "\033[%d;53H\033[2m| 0 | RRaA....... |  00 | 0     | 0     | 0          |\033[0m\033[0K"


#define ASH_VBAR "\033[2m|\033[0m"
#define ASH_LEAD "\033[%d;36H\033[2m| %X \033[0m"
#define ASH_CBAR "\033[%d;36H\033[2m+---+----+------------+----+----+----+-------+-------+-------+-------+-------+-------+-------+------------+\033[0m\033[0K\033[25;1H"
#define ASH_CLAS "\033[%d;36H\033[2m| S |      name       |attr|NTRs|mili| Ctime | Cdate | Adate | clusH | Wtime | Wdate | clusL |    size    |\033[0m\033[0K"
#define ASH_CLAL "\033[%d;36H\033[2m| L |ordn|    name1   |attr|type|cksm|                     name2                     | clusL |    name3   |\033[0m\033[0K"
#define ASH_EBAR "\033[%d;36H\033[2m+---+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+\033[0m\033[0K\033[25;1H"
#define ASH_EDEC "\033[%d;36H\033[2m| d |  0 |  1 |  2 |  3 |  4 |  5 |  6 |  7 |  8 |  9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 | 18 | 19 | 20 | 21 | 22 | 23 | 24 | 25 | 26 | 27 | 28 | 29 | 30 | 31 |\033[0m\033[0K\033[25;1H"
#define ASH_EHEX "\033[%d;36H\033[2m| h | 00 | 01 | 02 | 03 | 04 | 05 | 06 | 07 | 08 | 09 | 0A | 0B | 0C | 0D | 0E | 0F | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 | 18 | 19 | 1A | 1B | 1C | 1D | 1E | 1F |\033[0m\033[0K\033[25;1H"
#define ASH_ELAS "\033[%d;36H\033[2m| S |                         name                         |attr|NTRs|mili|  Ctime  |  Cdate  |  Adate  |  clusH  |  Wtime  |  Wdate  |  clusL  |       size        |\033[0m\033[0K"
#define ASH_ELAL "\033[%d;36H\033[2m| L |ordn|                      name1                      |attr|type|cksm|                           Name2                           |  clusL  |       name3       |\033[0m\033[0K"


void print_str(const unsigned char * str, int len, int width, const char * spc, int lpad, int rpad) {
	printf(ASH_VBAR "%*s", lpad, "");
	int i = 0;
	for (i = 0; i < len-1; ++i) {
		printf("%s%s", colorize_char(str[i], "~", width), spc);
	}
	printf("%s%*s" ASH_VBAR, colorize_char(str[i], "~", width), rpad, "");
}

void entry::print(void*addr, bool extended) {
	entry * entries = (entry*) addr;
	const int Y0 = 29;
	int i = 0;
	if (extended) {
		printf(ASH_EBAR, Y0-5);
		printf(ASH_ELAS, Y0-4);
		printf(ASH_EBAR, Y0-3);
		printf(ASH_ELAL, Y0-2);
		printf(ASH_EBAR, Y0-1);
		for (i = 0; i < 16; ++i) {
			printf(ASH_LEAD, i + Y0, i);
			if (entries[i].is_long()) entries[i].print_long(extended);
			else                      entries[i].print_short(extended);
		}
		printf(ASH_EBAR, i + Y0);
	} else {
		printf(ASH_CBAR, Y0-5);
		printf(ASH_CLAS, Y0-4);
		printf(ASH_CBAR, Y0-3);
		printf(ASH_CLAL, Y0-2);
		printf(ASH_CBAR, Y0-1);
		for (i = 0; i < 16; ++i) {
			printf(ASH_LEAD, i + Y0, i);
			if (entries[i].is_long()) entries[i].print_long(extended);
			else                      entries[i].print_short(extended);
		}
		printf(ASH_CBAR, i + Y0);
	}
}

void entry::print_short(bool extended) const {
	if (extended) {
		print_str(dir.ds.DIR_Name, 11, 3, "  ", 0, 1);
		printf( " %02X "   ASH_VBAR // DIR_Attr
				" %02X "   ASH_VBAR // DIR_NTRes
				" %02X "   ASH_VBAR // DIR_CrtTimeTenth
				"  %-5u  "   ASH_VBAR // DIR_CrtTime
				"  %-5u  "   ASH_VBAR // DIR_CrtDate
				"  %-5u  "   ASH_VBAR // DIR_LstAccDate
				"  %-5u  "   ASH_VBAR // DIR_FstClusHI
				"  %-5u  "   ASH_VBAR // DIR_WrtTime
				"  %-5u  "   ASH_VBAR // DIR_WrtDate
				"  %-5u  "   ASH_VBAR // DIR_FstClusLO
				"  %-11lu      " ASH_VBAR // DIR_FileSize
				, dir.ds.DIR_Attr, dir.ds.DIR_NTRes, dir.ds.DIR_CrtTimeTenth, dir.ds.DIR_CrtTime
				, dir.ds.DIR_CrtDate, dir.ds.DIR_LstAccDate, dir.ds.DIR_FstClusHI, dir.ds.DIR_WrtTime
				, dir.ds.DIR_WrtDate, dir.ds.DIR_FstClusLO, dir.ds.DIR_FileSize
		);
	} else {
		print_str(dir.ds.DIR_Name, 11, 1, "", 3, 3);
		printf( " %02X "   ASH_VBAR // DIR_Attr
				" %02X "   ASH_VBAR // DIR_NTRes
				" %02X "   ASH_VBAR // DIR_CrtTimeTenth
				" %-5u "   ASH_VBAR // DIR_CrtTime
				" %-5u "   ASH_VBAR // DIR_CrtDate
				" %-5u "   ASH_VBAR // DIR_LstAccDate
				" %-5u "   ASH_VBAR // DIR_FstClusHI
				" %-5u "   ASH_VBAR // DIR_WrtTime
				" %-5u "   ASH_VBAR // DIR_WrtDate
				" %-5u "   ASH_VBAR // DIR_FstClusLO
				" %-10lu " ASH_VBAR // DIR_FileSize
				, dir.ds.DIR_Attr, dir.ds.DIR_NTRes, dir.ds.DIR_CrtTimeTenth, dir.ds.DIR_CrtTime
				, dir.ds.DIR_CrtDate, dir.ds.DIR_LstAccDate, dir.ds.DIR_FstClusHI, dir.ds.DIR_WrtTime
				, dir.ds.DIR_WrtDate, dir.ds.DIR_FstClusLO, dir.ds.DIR_FileSize
		);
	}
	printf("\033[0K");
}


void entry::print_long(bool extended) const {
	if (extended) {
		printf(ASH_VBAR " %02X ", dir.dl.LDIR_Ord);
		print_str(dir.dl.LDIR_Name1, 10, 3, "  ", 0, 1);
		printf( " %02X "  ASH_VBAR // dir.dl.LDIR_Attr
				" %02X "  ASH_VBAR // dir.dl.LDIR_Type
				" %02X "           // dir.dl.LDIR_Chksum
				, dir.dl.LDIR_Attr, dir.dl.LDIR_Type, dir.dl.LDIR_Chksum
		);
		print_str(dir.dl.LDIR_Name2, 12, 3, "  ", 0, 1);
		printf("  %-5u  ", dir.dl.LDIR_FstClusLO);
		print_str(dir.dl.LDIR_Name3,  4, 3, "  ", 0, 1);
	} else {
		printf(ASH_VBAR " %02X ", dir.dl.LDIR_Ord);
		print_str(dir.dl.LDIR_Name1, 10, 1, "", 1, 1);
		printf( " %02X "  ASH_VBAR // dir.dl.LDIR_Attr
				" %02X "  ASH_VBAR // dir.dl.LDIR_Type
				" %02X "           // dir.dl.LDIR_Chksum
				, dir.dl.LDIR_Attr, dir.dl.LDIR_Type, dir.dl.LDIR_Chksum
		);
		print_str(dir.dl.LDIR_Name2, 12, 2, "  ", 0, 1);
		printf(" %-5u ", dir.dl.LDIR_FstClusLO);
		print_str(dir.dl.LDIR_Name3,  4, 2, " ", 0, 1);
	}
	printf("\033[0K");
}


