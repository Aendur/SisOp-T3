#include "disk_explorer.h"
#include "entry.h"
#include "utility.h"
#include "layout.h"
#include "dialog.h"
#include <stdexcept>
#include <vector>
#include <string>
#include <fstream>

DiskExplorer::DiskExplorer(WCHAR drive) {
	_ui.init();
	_device.open_drive(drive);
	_input.init(&_ui, 38, 24, "\033[1mGOTO:\033[0m ");
	Dialog::init(&_ui);
	
	if (_device.geometry().BytesPerSector != 512) {
		throw std::runtime_error("mismatch assumed bytes per sector = 512");
	}

	load_bookmarks();

	// save fat32 sector0 data
	_device.read();
	memcpy(&_sector0, _device.buffer(0), _device.geometry().BytesPerSector);

	// save fsinfo sector data	
	_device.seek(_sector0.BPB_FSInfo() * _device.geometry().BytesPerSector, false);
	_device.read();
	memcpy(&_fsi_sector, _device.buffer(0), _device.geometry().BytesPerSector);

	goto_offset(0);
	read_setpages();
	_page[0].init(&_sector0, 36,  1);
	_page[1].init(&_sector0, 36, 26);
	_page[1].cycle_entries_views();

	_editor.init(_device.geometry().BytesPerSector, &_ui, &_page[0], &_page[1]);

}

void DiskExplorer::load_bookmarks(void) {
	std::ifstream stream;
	stream.open("bookmarks.dat", stream.binary);
	if (stream.good()) {
		stream.read((char*)_sector_bookmark, sizeof(_sector_bookmark));
	} else {
		printf("create new bookmark file\nSIZE: %lld\n", sizeof(_sector_bookmark));
		memset(_sector_bookmark, 0, sizeof(_sector_bookmark));
		save_bookmarks();
	}
	stream.close();
}

void DiskExplorer::save_bookmarks(void) {
	std::ofstream stream;
	stream.open("bookmarks.dat", stream.binary);
	if (stream.good()) {
		stream.write((char*)_sector_bookmark, sizeof(_sector_bookmark));
	}
	stream.close();
}

void DiskExplorer::print_commands(void) const {
	_ui.clear_column(1, 1, 32, 47);
	printf("\n--- BOOKMARKS ---        \n");
	print_bookmarks();
	printf("S0~S9: bookmark sector     \n");
	printf("B: switch info             \n");
	printf("\n--- NAV ---              \n");
	printf("HOME  : goto FirstDataSec  \n");
	printf("G     : goto sector        \n");
	printf("H     : goto cluster (data)\n");
	printf("T/Y   : select FAT1/2 entry\n");
	if (_select_mode) {
		printf("\n--- DISP ---                 \n");
		printf("ARROWS: move cursor            \n");
	} else {
		printf("U_ARR : rewind %d %-15s\n", _adv_N, _adv_N == 1 ? "sector" : "sectors");
		printf("D_ARR : forwrd %d %-15s\n", _adv_N, _adv_N == 1 ? "sector" : "sectors");
		printf("LR_ARR: set N=%-10d \n", _adv_N);
		printf("\n--- DISP ---                 \n");
	}
	printf("TAB   : toggle select mode     \n");
	printf("INS   : edit current sector    \n");
	printf("F1~4  : toggle disp 1 modes    \n");
	printf("F5~8  : toggle disp 2 modes    \n");
	printf("D     : show drive info        \n");
	printf("PAUSE : %-20s\n", _locked ? "\033[32;1mUNLOCK\033[0m" : "\033[31;1mDISMOUNT & LOCK\033[0m");
	printf("ESC   : exit                \n");

	printf("\nS+TAB: convert value\n");
	if (Editor::converted_value != 0) {
	printf("  HEX: \033[1m%X\033[0m  \n", (unsigned) Editor::converted_value);
	printf("  DEC: \033[1m%lld\033[0m  \n", Editor::converted_value);
	printf("  OCT: \033[1m%o\033[0m  \n", (unsigned) Editor::converted_value);
	printf("  CHR: ");
	if(' ' <= Editor::converted_value && Editor::converted_value <= '~') printf("'\033[1m%c\033[0m'", (char) Editor::converted_value);
	printf("     \n");
	}

	_ui.clear_column(205, 1, 32, 47);
	switch(_show_drive_info) {
		case NO_INFO: _ui.clear_column(205, 1, 32, 47); break;
		case DEVINFO: printf("\n\033[205C--- GEOMETRY ---\n"); show_geom_info(); break;
		case F32INFO: printf("\n\033[205C--- FAT32 ---\n")   ; show_fat32_info(); break;
		case FSIINFO: printf("\n\033[205C--- FSINFO ---\n")  ; show_fsi_info(); break;
	}
}

static const DialogOptions quit_dialog_options = {
	{"Cancel", []() { return DIALOG_NO_SELECTION; }},
	{"OK"    , []() { return 1; }},
};

void DiskExplorer::run(void) {
	static const DWORD LEN = _device.geometry().BytesPerSector;

	// LONGLONG fat1_offset =  _sector0.BPB_RsvdSecCnt() * LEN;
	// LONGLONG fat2_offset = (_sector0.BPB_RsvdSecCnt() + _sector0.BPB_FATSz32()) * LEN;

	_editor.select(0);

	_ui.clear_screen();
	_page[0].print();
	_page[1].print();
	print_commands();

	KeyCode key = TERMUI_KEY_UNDEFINED;
	Dialog quit_dialog("Confirm exit?", quit_dialog_options);
	while ((key = _ui.read()) != TERMUI_KEY_ESC || quit_dialog.query(93,20) == DIALOG_NO_SELECTION) {
		switch(key) {
		case TERMUI_KEY_PAUSE      : toggle_lock()                                    ;                  break;
		case TERMUI_KEY_F1         : _page[0].cycle_sectors_views()                   ;                  break;
		case TERMUI_KEY_F2         : _page[0].cycle_entries_views()                   ;                  break;
		case TERMUI_KEY_F3         : _page[0].switch_text()                           ;                  break;
		case TERMUI_KEY_F4         : _page[0].switch_buff()                           ;                  break;
		case TERMUI_KEY_F5         : _page[1].cycle_sectors_views()                   ;                  break;
		case TERMUI_KEY_F6         : _page[1].cycle_entries_views()                   ;                  break;
		case TERMUI_KEY_F7         : _page[1].switch_text()                           ;                  break;
		case TERMUI_KEY_F8         : _page[1].switch_buff()                           ;                  break;
		case TERMUI_KEY_HOME       : goto_offset(_sector0.fds_offset())               ; read_setpages(); break;
		case TERMUI_KEY_0          : open_bookmark(0)                                 ; read_setpages(); break;
		case TERMUI_KEY_1          : open_bookmark(1)                                 ; read_setpages(); break;
		case TERMUI_KEY_2          : open_bookmark(2)                                 ; read_setpages(); break;
		case TERMUI_KEY_3          : open_bookmark(3)                                 ; read_setpages(); break;
		case TERMUI_KEY_4          : open_bookmark(4)                                 ; read_setpages(); break;
		case TERMUI_KEY_5          : open_bookmark(5)                                 ; read_setpages(); break;
		case TERMUI_KEY_6          : open_bookmark(6)                                 ; read_setpages(); break;
		case TERMUI_KEY_7          : open_bookmark(7)                                 ; read_setpages(); break;
		case TERMUI_KEY_8          : open_bookmark(8)                                 ; read_setpages(); break;
		case TERMUI_KEY_9          : open_bookmark(9)                                 ; read_setpages(); break;
		case TERMUI_KEY_EXCL       : bookmark_sector(1);                              ;                  break;
		case TERMUI_KEY_AT         : bookmark_sector(2);                              ;                  break;
		case TERMUI_KEY_HASH       : bookmark_sector(3);                              ;                  break;
		case TERMUI_KEY_DOLL       : bookmark_sector(4);                              ;                  break;
		case TERMUI_KEY_PERC       : bookmark_sector(5);                              ;                  break;
		case TERMUI_KEY_CFLEX      : bookmark_sector(6);                              ;                  break;
		case TERMUI_KEY_AMP        : bookmark_sector(7);                              ;                  break;
		case TERMUI_KEY_STAR       : bookmark_sector(8);                              ;                  break;
		case TERMUI_KEY_OPAR       : bookmark_sector(9);                              ;                  break;
		case TERMUI_KEY_CPAR       : bookmark_sector(0);                              ;                  break;
		case TERMUI_KEY_SHIFT_TAB  : _editor.convert_value()                          ;                  break;
		case TERMUI_KEY_b          :
		case TERMUI_KEY_B          : _show_bm_info = !_show_bm_info                   ;                  break;
		case TERMUI_KEY_d          :
		case TERMUI_KEY_D          : toggle_info_mode()                               ;                  break;
		case TERMUI_KEY_f          :
		case TERMUI_KEY_F          : printf(LAYOUT_FREE "  WIP search")               ;                  break;
		case TERMUI_KEY_g          :
		case TERMUI_KEY_G          : input_and_goto_sector()                          ; /* reads/sets */ break;
		case TERMUI_KEY_h          :
		case TERMUI_KEY_H          : input_and_goto_cluster_data()                    ; /* reads/sets */ break;
		case TERMUI_KEY_t          :
		case TERMUI_KEY_T          : input_and_goto_fat(0)                            ; /* reads/sets */ break;
		case TERMUI_KEY_y          :
		case TERMUI_KEY_Y          : input_and_goto_fat(1)                            ; /* reads/sets */ break;
		case TERMUI_KEY_ARROW_UP   : if (_select_mode){ _editor.move(-32); } else {advance_sectors(-(_adv_N+2) * (long) LEN)        ; read_setpages();} break;
		case TERMUI_KEY_ARROW_DOWN : if (_select_mode){ _editor.move( 32); } else {advance_sectors( (_adv_N-2) * (long) LEN)        ; read_setpages();} break;
		case TERMUI_KEY_ARROW_LEFT : if (_select_mode){ _editor.move( -1); } else {_adv_N = _adv_N > 10     ? _adv_N / 10 : 1       ;                 } break;
		case TERMUI_KEY_ARROW_RIGHT: if (_select_mode){ _editor.move(  1); } else {_adv_N = _adv_N < 100000 ? _adv_N * 10 : 1000000 ;                 } break;
		case TERMUI_KEY_CTRL_ARROW_UP   : _editor.move(-128); break;
		case TERMUI_KEY_CTRL_ARROW_DOWN : _editor.move( 128); break;
		case TERMUI_KEY_CTRL_ARROW_RIGHT: _editor.move(   4); break;
		case TERMUI_KEY_CTRL_ARROW_LEFT : _editor.move(  -4); break;
		case TERMUI_KEY_TAB        : _select_mode = !_select_mode                     ;                  break;
		case TERMUI_KEY_INSERT     : _editor.edit(_device)                            ; read_setpages(); break;
		case TERMUI_KEY_SPACE      : setpages()                                       ;                  break;
		default                    : setpages()                                       ;                  break;
		}
		_page[0].print();
		_page[1].print();
		print_commands();
	}
	_ui.clear_screen();
}

void DiskExplorer::toggle_lock(void) {
	if (_locked) {
		_locked = false;
		try {
			_device.unlock_drive();
			_device.reopen_drive();
		} catch (std::exception & e) {
			printf("Error: %s\n", e.what());
			_locked = true;
		}
	} else {
		_locked = true;
		try {
			_device.dismount_drive();
			try {
				_device.lock_drive();
			} catch (std::exception & e) {
				printf("Error: %s\n", e.what());
				_locked = false;
			}
		} catch (std::exception & e) {
			printf("Error: %s\n", e.what());
			_locked = false;
		}
	}
}

void DiskExplorer::setpages(void) {
	static const DWORD LEN = _device.geometry().BytesPerSector;
	_page[0].set(_device.buffers(), _device.offset());
	_page[1].set(_device.buffers(), _device.offset());
}

void DiskExplorer::read_setpages(void) {
	_device.read();
	setpages();
}

void DiskExplorer::advance_sectors(LONGLONG offset) {
	static const DWORD LEN = _device.geometry().BytesPerSector;
	LONGLONG mod = _device.capacity() % LEN;
	LONGLONG max_off = _device.capacity() - mod - LEN - LEN;

	if (_device.offset() + offset < 0) {
		offset = -_device.offset();
	} else if (_device.offset() + offset > max_off) {
		offset = max_off - _device.offset();
	} else if (offset % LEN != 0) {
		offset -= offset % LEN;
		wprintf(L"must offset by sector size!!");
	}
	_device.seek(offset, true);
}

void DiskExplorer::goto_offset(LONGLONG offset) {
	if (offset < 0) { offset = _device.capacity() + offset; }

	static const DWORD LEN = _device.geometry().BytesPerSector;
	LONGLONG mod = _device.capacity() % LEN;
	LONGLONG max_off = _device.capacity() - LEN - mod;

	if (offset < 0) {
		offset = 0;
	} else if (offset > max_off) {
		offset = max_off;
	} else {
		offset -= offset % LEN;
	}
	_device.seek(offset, false);
}

void DiskExplorer::input_and_goto_sector(void) {
	_input.set_msg("\033[1mGOTO SECTOR:\033[0m ");
	LONGLONG target_sector;
	if (_input.get(&target_sector, TERMUI_KEY_UNDEFINED, true)) {
		goto_offset(target_sector * _device.geometry().BytesPerSector);
		read_setpages();
	}
}
void DiskExplorer::input_and_goto_fat(int nfat) {
	switch (nfat) {
		case 1: _input.set_msg("\033[1mGOTO FAT2 ENTRY N:\033[0m "); break;
		default: nfat = 0;
		case 0: _input.set_msg("\033[1mGOTO FAT1 ENTRY N:\033[0m "); break;
	}
	
	LONGLONG N;
	if (_input.get(&N, TERMUI_KEY_UNDEFINED, true)) {
		goto_offset(_sector0.fat_sec_num(N, nfat) * _device.geometry().BytesPerSector);
		_editor.select(_sector0.fat_ent_off(N));
		read_setpages();
	}
}
void DiskExplorer::input_and_goto_cluster_data(void) {
	_input.set_msg("\033[1mGOTO DATA CLUSTER:\033[0m ");
	LONGLONG target_cluster;
	if (_input.get(&target_cluster, TERMUI_KEY_UNDEFINED, true)) {
		goto_offset(_sector0.first_sector_of_cluster(target_cluster) * _device.geometry().BytesPerSector);
		read_setpages();
	}
}

void DiskExplorer::open_bookmark(int i) {
	static const int LEN = _device.geometry().BytesPerSector;
	goto_offset(_sector_bookmark[i].sector * LEN);
	_editor.select(_sector_bookmark[i].selection);
}
void DiskExplorer::bookmark_sector(int i) {
	static const int LEN = _device.geometry().BytesPerSector;
	
	static char msg[32];
	snprintf(msg, 32, "\033[1mBOOKMARK %d NAME:\033[0m ", i);
	
	_input.set_maxlen(sizeof(SectorBookmark::name) * 2);
	_input.set_msg(msg);
	if (_input.get(_sector_bookmark[i].name, sizeof(SectorBookmark::name) - 1, TERMUI_KEY_UNDEFINED, true)) {
		if (_sector_bookmark[i].name[0] == 0) {
			snprintf(_sector_bookmark[i].name, sizeof(SectorBookmark::name), "<NO NAME>");
		}
		_sector_bookmark[i].sector = (_device.offset() - 2 * LEN) / LEN;
		_sector_bookmark[i].selection = _editor.position();
		save_bookmarks();
	}
}

void DiskExplorer::print_bookmarks(void) const {
	if (_show_bm_info) {
		for (int i = 0; i <= 9; ++i) {
			if ( _sector_bookmark[i].name[0] != 0) {
				printf("%d: sector %-12lld pos %-3d\n", i, _sector_bookmark[i].sector, _sector_bookmark[i].selection);
			}
		}
	} else {
		for (int i = 0; i <= 9; ++i) {
			if ( _sector_bookmark[i].name[0] != 0) {
				printf("%d: %-31s\n", i, _sector_bookmark[i].name);
			}
		}
	}
}

void DiskExplorer::show_geom_info(void) const {
	printf("\033[205CMediaType        : %d\n",   _device.geometry().MediaType);
	printf("\033[205CCylinders (Quad) : %lld\n", _device.geometry().Cylinders.QuadPart);
	printf("\033[205CCylinders (High) : %d\n",   _device.geometry().Cylinders.HighPart);
	printf("\033[205CCylinders (Low)  : %d\n",   _device.geometry().Cylinders.LowPart);
	printf("\033[205CTracksPerCylinder: %d\n",   _device.geometry().TracksPerCylinder);
	printf("\033[205CSectorsPerTrack  : %d\n",   _device.geometry().SectorsPerTrack);
	printf("\033[205CBytesPerSector   : %d\n",   _device.geometry().BytesPerSector);
	printf("\033[205CTotal capacity   : %lld B\n", _device.capacity());
	printf("\033[205C      %s\n"    , size_to_string(_device.capacity(), true));
}

void DiskExplorer::show_fat32_info(void) const {
	static char buf[12];
	snprintf(buf, 12, "%s", _sector0.BS_VolLab());

	printf("\033[205CBPB_FATSz16    : %u\n", _sector0.BPB_FATSz16());
	printf("\033[205CBPB_FATSz32    : %u\n", _sector0.BPB_FATSz32());
	printf("\033[205CBPB_NumFATs    : %u\n", _sector0.BPB_NumFATs());
	printf("\033[205CBPB_RsvdSecCnt : %u\n", _sector0.BPB_RsvdSecCnt());
	printf("\033[205CBytes/sector   : %u\n", _sector0.BPB_BytsPerSec());
	printf("\033[205CSectors/cluster: %u\n", _sector0.BPB_SecPerClus());
	printf("\033[205CBPB_RootClus   : %d\n", _sector0.BPB_RootClus());
	printf("\033[205CBPB_FSInfo     : %u\n", _sector0.BPB_FSInfo());
	printf("\033[205CBS_VolLab      : %s\n", buf);
	printf("\033[205C\n");
	printf("\033[205CCluster size   : %lu\n" , _sector0.cluster_size());
	printf("\033[205CFirstDataSector: %lu\n" , _sector0.first_data_sector());
	printf("\033[205CFDS offset     : %llu\n", _sector0.fds_offset());
}
void DiskExplorer::show_fsi_info(void) const {
	printf("\033[205CFSI_LeadSig   : 0x%0lX\n", _fsi_sector.FSI_LeadSig   ());
	printf("\033[205C              : 0x41615252\n");
	printf("\033[205CFSI_StrucSig  : 0x%0lX\n", _fsi_sector.FSI_StrucSig  ());
	printf("\033[205C              : 0x61417272\n");
	printf("\033[205CFSI_Free_Count: %lu\n", _fsi_sector.FSI_Free_Count());
	printf("\033[205CFSI_Nxt_Free  : %lu\n", _fsi_sector.FSI_Nxt_Free  ());
	printf("\033[205CFSI_TrailSig  : 0x%0lX\n", _fsi_sector.FSI_TrailSig  ());
	printf("\033[205C              : 0xAA550000\n");
}
