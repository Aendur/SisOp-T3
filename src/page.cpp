#include "page.h"
#include "device.h"
#include "utility.h"
#include <cstdio>
#include <cstring>

#define PAGE_BAR "\033[2m+---+------------+-------------------------------------------------------------------------------------------------------------+------------------------------------+\033[0m"
#define PAGE_HDR "\033[2m|   |   Offset   |  00 01 02 03  04 05 06 07   08 09 0A 0B  0C 0D 0E 0F   10 11 12 13  14 15 16 17   18 19 1A 1B  1C 1D 1E 1F  |                Text                |\033[0m"
#define PAGE_FTR "\033[2m|   | %-10llu |  %-53s Cluster: %-17lld Sector: %-16llu  | %s                            |\033[0m"
#define PAGE_ADR "\033[2m| %X |  %08llX  |  \033[0m"
#define PAGE_SEP0 " "
#define PAGE_SEP1 "  "
#define PAGE_SEP2 "   "
#define PAGE_SEP3 "\033[2m |  \033[0m"
#define PAGE_LMARGIN "  \033[%d;%dH"
#define PAGE_RMARGIN "  \033[2m|\033[0m"
#define PAGE_LE   "\033[0K"

void Page::init(DWORD sl, DWORD cl, LONGLONG fds, int x, int y) {
	if (!_initialized) {
		_sector_length = sl;
		_clustr_length = cl;
		_fds_offset = fds;
		_X0 = x;
		_Y0 = y;
		//_mode[View::SECTOR] = 0;
		//_mode[View::ENTRIES] = 0;
		_initialized = true;
	}
}

void Page::set(const PBYTE buffers[2], ULONGLONG offset) {
	_buffers[0] = buffers[0];
	_buffers[1] = buffers[1];
	_buffer = _buffers[_selected_buffer];
	_offset_end = offset;
}

//// Data line BEGIN
void Page::print_hex_block(PBYTE line, int i0, int i1) const {
	ColorizeOptions opts;
	opts.negative = false;
	opts.width = 2;
	opts.margin_right = 1;
	switch(_view) {
	case View::SECTORS_ASC:
		opts.chr_hex = false;
		opts.ctl_str = NULL;
		break;
	case View::SECTORS_HEX:
	default:
		opts.chr_hex = true;
		opts.ctl_str = NULL;
		break;
	}

	for (int i =  i0; i < i1; ++i) {
		opts.negative = actual_selected_byte((int)(line - _buffer + i));
		opts.byte = line[i];
		printf("%s", colorize_byte(opts));
	}
}

void Page::print_hex(PBYTE line) const {
	print_hex_block(line,  0,  4); printf(PAGE_SEP0);
	print_hex_block(line,  4,  8); printf(PAGE_SEP1);
	print_hex_block(line,  8, 12); printf(PAGE_SEP0);
	print_hex_block(line, 12, 16); printf(PAGE_SEP1);
	print_hex_block(line, 16, 20); printf(PAGE_SEP0);
	print_hex_block(line, 20, 24); printf(PAGE_SEP1);
	print_hex_block(line, 24, 28); printf(PAGE_SEP0);
	print_hex_block(line, 28, 32); printf(PAGE_SEP3);
}

void Page::print_int_block(UINT32 value, bool selected) const {
	const char * attr1 = selected ? "\033[7m" : "";
	const char * attr2 = selected ? "\033[27m" : "";
	switch(value) {
		case 0x0FFFFFF8: printf("%s%11s%s", attr1, "Reserved.1", attr2); break;
		case 0xFFFFFFFF: printf("%s%11s%s", attr1, "Reserved.2", attr2); break;
		case 0x0FFFFFFF: printf("%s%11s%s", attr1, "EOC", attr2); break;
		case 0x00000000: printf("%s%11s%s", attr1, "---", attr2); break;
		default:         printf("%s%11u%s", attr1, value, attr2); break;
	}
	printf(" ");
}
void Page::print_int(PBYTE line) const {
	int i0 = (int)(line - _buffer) / sizeof(UINT32);
	int i1 = _selected / sizeof(UINT32) - i0;

	int i = 0;
	print_int_block( ((UINT32*)line)[i], i1 == i ); printf(PAGE_SEP0); ++i;
	print_int_block( ((UINT32*)line)[i], i1 == i ); printf(PAGE_SEP1); ++i;
	print_int_block( ((UINT32*)line)[i], i1 == i ); printf(PAGE_SEP0); ++i;
	print_int_block( ((UINT32*)line)[i], i1 == i ); printf(PAGE_SEP1); ++i;
	print_int_block( ((UINT32*)line)[i], i1 == i ); printf(PAGE_SEP0); ++i;
	print_int_block( ((UINT32*)line)[i], i1 == i ); printf(PAGE_SEP1); ++i;
	print_int_block( ((UINT32*)line)[i], i1 == i ); printf(PAGE_SEP0); ++i;
	print_int_block( ((UINT32*)line)[i], i1 == i ); printf(PAGE_SEP3); ++i;
}

void Page::print_sector_str(PBYTE line, int len) const {
	ColorizeOptions opts;
	opts.chr_hex = false;
	opts.ctl_str = ".";
	opts.width = 1;

	for (int i = 0x00; i < len; ++i) {
		opts.negative = actual_selected_byte((int)(line - _buffer + i));
		opts.byte = line[i];
		printf("%s", colorize_byte(opts));
	}
	printf(PAGE_RMARGIN PAGE_LE);
}

#define LINE_WIDTH 0x20
static const char * EDITSTR = "\033[0;33;1mEDITING\033[0;2m";
static const char * VIEWSTR = "\033[0;32;1mVIEWING\033[0;2m";
void Page::print_sector(void) const {
	PBYTE end = _buffer + _sector_length;
	PBYTE pos = _buffer;
	LONGLONG current_sector = (LONGLONG) offset_start() - _fds_offset;
	current_sector = 1 + (current_sector >= 0) + current_sector / (LONGLONG) _clustr_length;

	printf(PAGE_LMARGIN PAGE_BAR PAGE_LE, _Y0 + 0, _X0);
	printf(PAGE_LMARGIN PAGE_HDR PAGE_LE, _Y0 + 1, _X0);
	printf(PAGE_LMARGIN PAGE_BAR PAGE_LE, _Y0 + 2, _X0);
	int nline = 0;
	while(pos < end) {
		printf(PAGE_LMARGIN PAGE_ADR, _Y0 + 3 + nline, _X0, nline, offset_start() + pos - _buffer);
		if (_view == View::SECTORS_HEX) print_hex(pos);
		else                            print_int(pos);
		print_sector_str(pos, LINE_WIDTH);
		pos += LINE_WIDTH;
		++nline;
	}
	printf(PAGE_LMARGIN PAGE_BAR PAGE_LE, _Y0 + 3 + nline, _X0);
	printf(PAGE_LMARGIN PAGE_FTR PAGE_LE, _Y0 + 4 + nline, _X0,
		offset_start() , size_to_string(offset_start(), true),
		current_sector,
		offset_start() / _sector_length,
		(_editing == true) ? EDITSTR : VIEWSTR
	);
	printf(PAGE_LMARGIN PAGE_BAR PAGE_LE, _Y0 + 5 + nline, _X0);
}

//// Data line END

void Page::print(void) const {
	switch (_view) {
		case View::SECTORS_ASC:
		case View::SECTORS_HEX:
		case View::SECTORS_INT: print_sector(); break;
		case View::ENTRIES_SHO:
		case View::ENTRIES_LON: print_entry(); break;
		default: break;
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
#define ENTRY_ASH_ELAL "\033[%d;%dH\033[2m| L |ordn|                      name1                      |attr|type|cksm|                           name2                           |  clusL  |       name3       |\033[0m\033[0K"
#define ENTRY_ASH_STAT "\033[%d;%dH%s"

// 0 <= i < 512
bool Page::actual_selected_byte(int i) const {
	return _selected == (int)(i + _sector_length * _selected_buffer);
}

//0 <= i < 16
bool Page::actual_selected_entry(int i) const {
	int i0 = i * 32 + _sector_length * _selected_buffer;
	int i1 = i0 + 32;
	return (i0 <= _selected && _selected < i1);
}

void Page::print_entry(void) const {
	entry * entries = (entry*) _buffer;
	int i = 0;
	//bool extended = (_mode.at(_view) % 2) == 0;
	bool extended = _view == View::ENTRIES_LON;
	if (extended) {
		printf(ENTRY_ASH_EBAR, _Y0 + 0, _X0);
		printf(ENTRY_ASH_ELAL, _Y0 + 1, _X0);
		printf(ENTRY_ASH_EBAR, _Y0 + 2, _X0);
		for (i = 0; i < 16; ++i) {
			bool selected = actual_selected_entry(i);
			printf(ENTRY_ASH_LEAD, i + _Y0 + 3, _X0, i);
			if (entries[i].is_long()) print_long(entries[i], extended, selected);
			else                      print_short(entries[i], extended, selected);
		}
		printf(ENTRY_ASH_EBAR, i + _Y0 + 3, _X0);
		printf(ENTRY_ASH_ELAS, i + _Y0 + 4, _X0);
		printf(ENTRY_ASH_EBAR, i + _Y0 + 5, _X0);
	} else {
		printf(ENTRY_ASH_CBAR, _Y0 + 0, _X0);
		printf(ENTRY_ASH_CLAL, _Y0 + 1, _X0);
		printf(ENTRY_ASH_CBAR, _Y0 + 2, _X0);
		for (i = 0; i < 16; ++i) {
			bool selected = actual_selected_entry(i);
			printf(ENTRY_ASH_LEAD, i + _Y0 + 3, _X0, i);
			if (entries[i].is_long()) print_long(entries[i], extended, selected);
			else                      print_short(entries[i], extended, selected);
		}
		printf(ENTRY_ASH_CBAR, i + _Y0 + 3, _X0);
		printf(ENTRY_ASH_CLAS, i + _Y0 + 4, _X0);
		printf(ENTRY_ASH_CBAR, i + _Y0 + 5, _X0);
	}
}


void print_colorized_str(const unsigned char * str, int len, ColorizeOptions * opts, int lm, int rm) {
	printf(ENTRY_ASH_VBAR);
	printf("%*s", lm, "");
	for (int i = 0; i < len-1; ++i) {
		opts->byte = str[i];
		printf("%s", colorize_byte(*opts));
	}
	opts->padding_right = 0;
	opts->byte = str[len-1];
	printf("%s", colorize_byte(*opts));
	printf("%*s", rm, "");
	printf(ENTRY_ASH_VBAR);
}

void Page::print_short(const entry& ref, bool extended, bool selected) const {
	ColorizeOptions opts;	
	opts.chr_hex = false;
	opts.ctl_str = "~";
	opts.negative = false;
	opts.width = 1;
	opts.margin_left = 0;
	opts.margin_right = 0;
	opts.underline = selected;
	const char * attr1 = selected ? "\033[4m" : "";
	const char * attr2 = selected ? "\033[24m" : "";

	if (extended) {
		opts.padding_left = 2;
		opts.padding_right = 2;
		print_colorized_str(ref.dir.ds.DIR_Name, 11, &opts, 0, 1);
		printf(" %s%02X%s "        ENTRY_ASH_VBAR, attr1, ref.dir.ds.DIR_Attr, attr2);
		printf(" %s%02X%s "        ENTRY_ASH_VBAR, attr1, ref.dir.ds.DIR_NTRes, attr2);
		printf(" %s%02X%s "        ENTRY_ASH_VBAR, attr1, ref.dir.ds.DIR_CrtTimeTenth, attr2);
		printf(" %s%02X   %02X%s " ENTRY_ASH_VBAR, attr1, ((unsigned char*)&ref.dir.ds.DIR_CrtTime)[0]    , ((unsigned char*)&ref.dir.ds.DIR_CrtTime)[1], attr2);
		printf(" %s%02X   %02X%s " ENTRY_ASH_VBAR, attr1, ((unsigned char*)&ref.dir.ds.DIR_CrtDate)[0]    , ((unsigned char*)&ref.dir.ds.DIR_CrtDate)[1], attr2);
		printf(" %s%02X   %02X%s " ENTRY_ASH_VBAR, attr1, ((unsigned char*)&ref.dir.ds.DIR_LstAccDate)[0] , ((unsigned char*)&ref.dir.ds.DIR_LstAccDate)[1], attr2);
		printf(" %s%02X   %02X%s " ENTRY_ASH_VBAR, attr1, ((unsigned char*)&ref.dir.ds.DIR_FstClusHI)[0]  , ((unsigned char*)&ref.dir.ds.DIR_FstClusHI)[1], attr2);
		printf(" %s%02X   %02X%s " ENTRY_ASH_VBAR, attr1, ((unsigned char*)&ref.dir.ds.DIR_WrtTime)[0]    , ((unsigned char*)&ref.dir.ds.DIR_WrtTime)[1], attr2);
		printf(" %s%02X   %02X%s " ENTRY_ASH_VBAR, attr1, ((unsigned char*)&ref.dir.ds.DIR_WrtDate)[0]    , ((unsigned char*)&ref.dir.ds.DIR_WrtDate)[1], attr2);
		printf(" %s%02X   %02X%s " ENTRY_ASH_VBAR, attr1, ((unsigned char*)&ref.dir.ds.DIR_FstClusLO)[0]  , ((unsigned char*)&ref.dir.ds.DIR_FstClusLO)[1], attr2);
		printf(" %s%02X   %02X   %02X   %02X%s " ENTRY_ASH_VBAR, attr1
				, ((unsigned char*)&ref.dir.ds.DIR_FileSize)[0]
				, ((unsigned char*)&ref.dir.ds.DIR_FileSize)[1]
				, ((unsigned char*)&ref.dir.ds.DIR_FileSize)[2]
				, ((unsigned char*)&ref.dir.ds.DIR_FileSize)[3], attr2
		);
	} else {
		opts.padding_left = 0;
		opts.padding_right = 0;
		print_colorized_str(ref.dir.ds.DIR_Name, 11, &opts, 3, 3);
		printf(" %s%02X%s "   ENTRY_ASH_VBAR, attr1, ref.dir.ds.DIR_Attr, attr2); // DIR_Attr
		printf(" %s%02X%s "   ENTRY_ASH_VBAR, attr1, ref.dir.ds.DIR_NTRes, attr2); // DIR_NTRes
		printf(" %s%02X%s "   ENTRY_ASH_VBAR, attr1, ref.dir.ds.DIR_CrtTimeTenth, attr2); // DIR_CrtTimeTenth
		printf(" %s%-5u%s "   ENTRY_ASH_VBAR, attr1, ref.dir.ds.DIR_CrtTime, attr2); // DIR_CrtTime
		printf(" %s%-5u%s "   ENTRY_ASH_VBAR, attr1, ref.dir.ds.DIR_CrtDate, attr2); // DIR_CrtDate
		printf(" %s%-5u%s "   ENTRY_ASH_VBAR, attr1, ref.dir.ds.DIR_LstAccDate, attr2); // DIR_LstAccDate
		printf(" %s%-5u%s "   ENTRY_ASH_VBAR, attr1, ref.dir.ds.DIR_FstClusHI, attr2); // DIR_FstClusHI
		printf(" %s%-5u%s "   ENTRY_ASH_VBAR, attr1, ref.dir.ds.DIR_WrtTime, attr2); // DIR_WrtTime
		printf(" %s%-5u%s "   ENTRY_ASH_VBAR, attr1, ref.dir.ds.DIR_WrtDate, attr2); // DIR_WrtDate
		printf(" %s%-5u%s "   ENTRY_ASH_VBAR, attr1, ref.dir.ds.DIR_FstClusLO, attr2); // DIR_FstClusLO
		printf(" %s%-10lu%s " ENTRY_ASH_VBAR, attr1, ref.dir.ds.DIR_FileSize, attr2); // DIR_FileSize
	}
	printf("\033[0K");
}

void Page::print_long(const entry& ref, bool extended, bool selected) const {
	ColorizeOptions opts;	
	opts.chr_hex = false;
	opts.ctl_str = "~";
	opts.negative = false;
	opts.width = 1;
	opts.margin_left = 0;
	opts.margin_right = 0;
	opts.underline = selected;

	const char * attr1 = selected ? "\033[4m" : "";
	const char * attr2 = selected ? "\033[24m" : "";

	if (extended) {
		opts.padding_left = 2;
		opts.padding_right = 2;
		printf(ENTRY_ASH_VBAR " %02X ", ref.dir.dl.LDIR_Ord);
		print_colorized_str(ref.dir.dl.LDIR_Name1, 10, &opts, 0, 1);
		printf(" %s%02X%s "  ENTRY_ASH_VBAR, attr1, ref.dir.dl.LDIR_Attr, attr2);
		printf(" %s%02X%s "  ENTRY_ASH_VBAR, attr1, ref.dir.dl.LDIR_Type, attr2);
		printf(" %s%02X%s "                , attr1, ref.dir.dl.LDIR_Chksum, attr2);

		opts.padding_left = 2;
		opts.padding_right = 2;
		print_colorized_str(ref.dir.dl.LDIR_Name2, 12, &opts, 0, 1);
		printf(" %s%02X   %02X%s ", attr1, ((unsigned char*)&ref.dir.dl.LDIR_FstClusLO)[0], ((unsigned char*)&ref.dir.dl.LDIR_FstClusLO)[1], attr2);

		opts.padding_left = 2;
		opts.padding_right = 2;
		print_colorized_str(ref.dir.dl.LDIR_Name3, 4, &opts, 0, 1);
	} else {
		printf(ENTRY_ASH_VBAR " %s%02X%s ", attr1, ref.dir.dl.LDIR_Ord, attr2);
		
		//|    name1   |
		opts.padding_left = 0;
		opts.padding_right = 0;
		print_colorized_str(ref.dir.dl.LDIR_Name1, 10, &opts, 1, 1);
		printf(" %s%02X%s "  ENTRY_ASH_VBAR, attr1, ref.dir.dl.LDIR_Attr, attr2);
		printf(" %s%02X%s "  ENTRY_ASH_VBAR, attr1, ref.dir.dl.LDIR_Type, attr2);
		printf(" %s%02X%s "                , attr1, ref.dir.dl.LDIR_Chksum, attr2);
		
		//|                     name2                     |
		opts.padding_left = 1;
		opts.padding_right = 2;
		print_colorized_str(ref.dir.dl.LDIR_Name2, 12, &opts, 0, 1);
		printf(" %s%-5u%s ", attr1, ref.dir.dl.LDIR_FstClusLO, attr2);

		//|    name3   |
		opts.padding_left = 1;
		opts.padding_right = 1;
		print_colorized_str(ref.dir.dl.LDIR_Name3, 4, &opts, 0, 1);
	}
	printf("\033[0K");
}




