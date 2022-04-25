#include "page.h"
#include "device.h"
#include "utility.h"
#include <cstdio>
#include <cstring>

#define PAGE_BAR "\033[2m+---+------------+-------------------------------------------------------------------------------------------------------+------------------------------------+\033[0m"
#define PAGE_HDR "\033[2m|   |   Offset   |  00 01 02 03 04 05 06 07  08 09 0A 0B 0C 0D 0E 0F   10 11 12 13 14 15 16 17  18 19 1A 1B 1C 1D 1E 1F  |                Text                |\033[0m"
#define PAGE_FTR "\033[2m|   | %-10llu |  %-50s Cluster: %-15llu Sector: %-15llu  |                                    |\033[0m"
#define PAGE_ADR "\033[2m| %X |  %08llX  |  \033[0m"
#define PAGE_SEP0 " "
#define PAGE_SEP1 "  "
#define PAGE_SEP2 "\033[2m |  \033[0m"
#define PAGE_LMARGIN "  \033[%d;%dH"
#define PAGE_RMARGIN "  \033[2m|\033[0m"
#define PAGE_LE   "\033[0K"

void Page::init(DWORD sl, DWORD cl, int x, int y) {
	if (!_initialized) {
		_sector_length = sl;
		_clustr_length = cl;
		_X0 = x;
		_Y0 = y;
		_mode[View::SECTOR] = 0;
		_mode[View::ENTRIES] = 0;
		_initialized = true;
	}
}


//// Data line BEGIN
void Page::print_hex_block(PBYTE line, int i0, int i1) const {
	for (int i =  i0; i < i1; ++i) {
		bool selected = i == _selected;
		switch(_mode.at(_view) % 3) {
		case 2:
			printf("%s ", colorize_char(line[i], 2, selected));
			break;
		case 1:
			printf("%s ", colorize_char(line[i], " .", 2, selected));
			break;
		case 0:
		default:
			if (selected) {
				printf("\033[7m%02X\033\0m ", line[i]);
			} else {
				printf("%02X ", line[i]);
			}
			break;
		}
	}
}

void Page::print_hex(PBYTE line, int len) const {
	static const int q1 = 0x08;
	static const int q2 = 0x10;
	static const int q3 = 0x18;
	print_hex_block(line,  0,  q1); printf(PAGE_SEP0);
	print_hex_block(line, q1,  q2); printf(PAGE_SEP1);
	print_hex_block(line, q2,  q3); printf(PAGE_SEP0);
	print_hex_block(line, q3, len); printf(PAGE_SEP2);
}

void Page::print_sector_str(PBYTE line, int len) const {
	for (int i = 0x00; i < len; ++i) {
		char c = line[i];
		printf("%s", colorize_char(c, '.', i == _selected));
	}
	printf(PAGE_RMARGIN PAGE_LE);
}

#define LINE_WIDTH 0x20
void Page::print_sector(void) const {
	PBYTE end = _buffer + _sector_length;
	PBYTE pos = _buffer;
	printf(PAGE_LMARGIN PAGE_BAR PAGE_LE, _Y0 + 0, _X0);
	printf(PAGE_LMARGIN PAGE_HDR PAGE_LE, _Y0 + 1, _X0);
	printf(PAGE_LMARGIN PAGE_BAR PAGE_LE, _Y0 + 2, _X0);
	printf(PAGE_LMARGIN PAGE_FTR PAGE_LE, _Y0 + 3, _X0,
		_offset , size_to_string(_offset, true),
		_offset / _clustr_length,
		_offset / _sector_length
	);
	printf(PAGE_LMARGIN PAGE_BAR PAGE_LE, _Y0 + 4, _X0);
	
	int nline = 0;
	while(pos < end) {
		printf(PAGE_LMARGIN PAGE_ADR, _Y0 + 5 + nline, _X0, nline, _offset + pos - _buffer);
		print_hex(pos, LINE_WIDTH);
		print_sector_str(pos, LINE_WIDTH);
		pos += LINE_WIDTH;
		++nline;
	}
	printf(PAGE_LMARGIN PAGE_BAR PAGE_LE, _Y0 + 5 + nline, _X0);
}
//// Data line END

void Page::print(void) const {
	if (_view == View::SECTOR) {
		print_sector();
	} else if (_view == View::ENTRIES) {
		print_entry();
	}
}



/////////////////////  ENTRY ////////////////
#include "entry.h"

#define ENTRY_ASH_VBAR "\033[2m|\033[0m"
#define ENTRY_ASH_LEAD "\033[%d;%dH\033[2m| %X \033[0m"
#define ENTRY_ASH_CBAR "\033[%d;%dH\033[2m+---+----+------------+----+----+----+-------+-------+-------+-------+-------+-------+-------+------------+\033[0m\033[0K\033[25;1H"
#define ENTRY_ASH_CLAS "\033[%d;%dH\033[2m| S |      name       |attr|NTRs|mili| Ctime | Cdate | Adate | clusH | Wtime | Wdate | clusL |    size    |\033[0m\033[0K"
#define ENTRY_ASH_CLAL "\033[%d;%dH\033[2m| L |ordn|    name1   |attr|type|cksm|                     name2                     | clusL |    name3   |\033[0m\033[0K"
#define ENTRY_ASH_EBAR "\033[%d;%dH\033[2m+---+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+\033[0m\033[0K\033[25;1H"
#define ENTRY_ASH_EDEC "\033[%d;%dH\033[2m| d |  0 |  1 |  2 |  3 |  4 |  5 |  6 |  7 |  8 |  9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 | 18 | 19 | 20 | 21 | 22 | 23 | 24 | 25 | 26 | 27 | 28 | 29 | 30 | 31 |\033[0m\033[0K\033[25;1H"
#define ENTRY_ASH_EHEX "\033[%d;%dH\033[2m| h | 00 | 01 | 02 | 03 | 04 | 05 | 06 | 07 | 08 | 09 | 0A | 0B | 0C | 0D | 0E | 0F | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 | 18 | 19 | 1A | 1B | 1C | 1D | 1E | 1F |\033[0m\033[0K\033[25;1H"
#define ENTRY_ASH_ELAS "\033[%d;%dH\033[2m| S |                         name                         |attr|NTRs|mili|  Ctime  |  Cdate  |  Adate  |  clusH  |  Wtime  |  Wdate  |  clusL  |       size        |\033[0m\033[0K"
#define ENTRY_ASH_ELAL "\033[%d;%dH\033[2m| L |ordn|                      name1                      |attr|type|cksm|                           Name2                           |  clusL  |       name3       |\033[0m\033[0K"


void print_entry_str(const unsigned char * str, int len, int width, const char * spc, int lpad, int rpad) {
	printf(ENTRY_ASH_VBAR "%*s", lpad, "");
	int i = 0;
	for (i = 0; i < len-1; ++i) {
		printf("%s%s", colorize_char(str[i], "~", width, false), spc);
	}
	printf("%s%*s" ENTRY_ASH_VBAR, colorize_char(str[i], "~", width, false), rpad, "");
}

void Page::print_entry(void) const {
	entry * entries = (entry*) _buffer;
	int i = 0;
	bool extended = (_mode.at(_view) % 2) == 0;
	if (extended) {
		printf(ENTRY_ASH_EBAR, _Y0 + 0, _X0);
		printf(ENTRY_ASH_ELAS, _Y0 + 1, _X0);
		printf(ENTRY_ASH_EBAR, _Y0 + 2, _X0);
		printf(ENTRY_ASH_ELAL, _Y0 + 3, _X0);
		printf(ENTRY_ASH_EBAR, _Y0 + 4, _X0);
		for (i = 0; i < 16; ++i) {
			printf(ENTRY_ASH_LEAD, i + _Y0 + 5, _X0, i);
			if (entries[i].is_long()) print_long(entries[i], extended);
			else                      print_short(entries[i], extended);
		}
		printf(ENTRY_ASH_EBAR, i + _Y0 + 5, _X0);
	} else {
		printf(ENTRY_ASH_CBAR, _Y0 + 0, _X0);
		printf(ENTRY_ASH_CLAS, _Y0 + 1, _X0);
		printf(ENTRY_ASH_CBAR, _Y0 + 2, _X0);
		printf(ENTRY_ASH_CLAL, _Y0 + 3, _X0);
		printf(ENTRY_ASH_CBAR, _Y0 + 4, _X0);
		for (i = 0; i < 16; ++i) {
			printf(ENTRY_ASH_LEAD, i + _Y0 + 5, _X0, i);
			if (entries[i].is_long()) print_long(entries[i], extended);
			else                      print_short(entries[i], extended);
		}
		printf(ENTRY_ASH_CBAR, i + _Y0 + 5, _X0);
	}
}

void Page::print_short(const entry& ref, bool extended) const {
	if (extended) {
		print_entry_str(ref.dir.ds.DIR_Name, 11, 3, "  ", 0, 1);
		printf( " %02X "         ENTRY_ASH_VBAR // DIR_Attr
				" %02X "         ENTRY_ASH_VBAR // DIR_NTRes
				" %02X "         ENTRY_ASH_VBAR // DIR_CrtTimeTenth
				"  %-5u  "       ENTRY_ASH_VBAR // DIR_CrtTime
				"  %-5u  "       ENTRY_ASH_VBAR // DIR_CrtDate
				"  %-5u  "       ENTRY_ASH_VBAR // DIR_LstAccDate
				"  %-5u  "       ENTRY_ASH_VBAR // DIR_FstClusHI
				"  %-5u  "       ENTRY_ASH_VBAR // DIR_WrtTime
				"  %-5u  "       ENTRY_ASH_VBAR // DIR_WrtDate
				"  %-5u  "       ENTRY_ASH_VBAR // DIR_FstClusLO
				"  %-11lu      " ENTRY_ASH_VBAR // DIR_FileSize
				, ref.dir.ds.DIR_Attr, ref.dir.ds.DIR_NTRes, ref.dir.ds.DIR_CrtTimeTenth, ref.dir.ds.DIR_CrtTime
				, ref.dir.ds.DIR_CrtDate, ref.dir.ds.DIR_LstAccDate, ref.dir.ds.DIR_FstClusHI, ref.dir.ds.DIR_WrtTime
				, ref.dir.ds.DIR_WrtDate, ref.dir.ds.DIR_FstClusLO, ref.dir.ds.DIR_FileSize
		);
	} else {
		print_entry_str(ref.dir.ds.DIR_Name, 11, 1, "", 3, 3);
		printf( " %02X "   ENTRY_ASH_VBAR // DIR_Attr
				" %02X "   ENTRY_ASH_VBAR // DIR_NTRes
				" %02X "   ENTRY_ASH_VBAR // DIR_CrtTimeTenth
				" %-5u "   ENTRY_ASH_VBAR // DIR_CrtTime
				" %-5u "   ENTRY_ASH_VBAR // DIR_CrtDate
				" %-5u "   ENTRY_ASH_VBAR // DIR_LstAccDate
				" %-5u "   ENTRY_ASH_VBAR // DIR_FstClusHI
				" %-5u "   ENTRY_ASH_VBAR // DIR_WrtTime
				" %-5u "   ENTRY_ASH_VBAR // DIR_WrtDate
				" %-5u "   ENTRY_ASH_VBAR // DIR_FstClusLO
				" %-10lu " ENTRY_ASH_VBAR // DIR_FileSize
				, ref.dir.ds.DIR_Attr, ref.dir.ds.DIR_NTRes, ref.dir.ds.DIR_CrtTimeTenth, ref.dir.ds.DIR_CrtTime
				, ref.dir.ds.DIR_CrtDate, ref.dir.ds.DIR_LstAccDate, ref.dir.ds.DIR_FstClusHI, ref.dir.ds.DIR_WrtTime
				, ref.dir.ds.DIR_WrtDate, ref.dir.ds.DIR_FstClusLO, ref.dir.ds.DIR_FileSize
		);
	}
	printf("\033[0K");
}


void Page::print_long(const entry& ref, bool extended) const {
	if (extended) {
		printf(ENTRY_ASH_VBAR " %02X ", ref.dir.dl.LDIR_Ord);
		print_entry_str(ref.dir.dl.LDIR_Name1, 10, 3, "  ", 0, 1);
		printf( " %02X "  ENTRY_ASH_VBAR // ref.dir.dl.LDIR_Attr
				" %02X "  ENTRY_ASH_VBAR // ref.dir.dl.LDIR_Type
				" %02X "           // ref.dir.dl.LDIR_Chksum
				, ref.dir.dl.LDIR_Attr, ref.dir.dl.LDIR_Type, ref.dir.dl.LDIR_Chksum
		);
		print_entry_str(ref.dir.dl.LDIR_Name2, 12, 3, "  ", 0, 1);
		printf("  %-5u  ", ref.dir.dl.LDIR_FstClusLO);
		print_entry_str(ref.dir.dl.LDIR_Name3,  4, 3, "  ", 0, 1);
	} else {
		printf(ENTRY_ASH_VBAR " %02X ", ref.dir.dl.LDIR_Ord);
		print_entry_str(ref.dir.dl.LDIR_Name1, 10, 1, "", 1, 1);
		printf( " %02X "  ENTRY_ASH_VBAR // ref.dir.dl.LDIR_Attr
				" %02X "  ENTRY_ASH_VBAR // ref.dir.dl.LDIR_Type
				" %02X "           // ref.dir.dl.LDIR_Chksum
				, ref.dir.dl.LDIR_Attr, ref.dir.dl.LDIR_Type, ref.dir.dl.LDIR_Chksum
		);
		print_entry_str(ref.dir.dl.LDIR_Name2, 12, 2, "  ", 0, 1);
		printf(" %-5u ", ref.dir.dl.LDIR_FstClusLO);
		print_entry_str(ref.dir.dl.LDIR_Name3,  4, 2, " ", 0, 1);
	}
	printf("\033[0K");
}




