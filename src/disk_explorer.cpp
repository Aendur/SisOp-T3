#include "disk_explorer.h"
#include "entry.h"
#include "utility.h"
#include "layout.h"
#include "dialog.h"
#include <stdexcept>
#include <vector>
#include <string>

DiskExplorer::DiskExplorer(WCHAR drive) {
	_ui.init();
	_device.open_drive(drive);
	_input.init(&_ui, 38, 24, "\033[1mGOTO:\033[0m ");
	Dialog::init(&_ui);
	
	if (_device.geometry().BytesPerSector != 512) {
		throw std::runtime_error("mismatch assumed bytes per sector = 512");
	}

	for (int i = 0; i < 10; ++i) { _sector_bookmark[i] = 0; }

	// save fat32 sector0 data
	_device.read();
	memcpy(&_sector0, _device.buffer(0), _device.geometry().BytesPerSector);

	// save fsinfo sector data	
	_device.seek(_sector0.BPB_FSInfo() * _device.geometry().BytesPerSector, false);
	_device.read();
	memcpy(&_fsi_sector, _device.buffer(0), _device.geometry().BytesPerSector);

	goto_offset(0);
	read_setpages();
	_page[0].init(_device.geometry().BytesPerSector, cluster_size(), 36, 1);
	_page[1].init(_device.geometry().BytesPerSector, cluster_size(), 36, 26);
	_editor.init(_device.geometry().BytesPerSector, &_ui, &_page[0], &_page[1]);

	_page[0].toggle_view();
	_page[1].toggle_view();
	_page[1].toggle_view();
	_page[1].toggle_view();
}

void DiskExplorer::print_commands(void) const {
	printf("\033[1;1H");
	printf("\n--- NAV ---               \n");
	printf("0     : goto sector 0      \n");
//	printf("1     : goto FSI           \n");
	printf("1     : goto FAT1          \n");
	printf("2     : goto FAT2          \n");
	printf("3     : goto FirstDataSec  \n");
	printf("4     : goto sector %-5lld \n", _sector_bookmark[4]);
	printf("5     : goto sector %-5lld \n", _sector_bookmark[5]);
	printf("6     : goto sector %-5lld \n", _sector_bookmark[6]);
	printf("7     : goto sector %-5lld \n", _sector_bookmark[7]);
	printf("8     : goto sector %-5lld \n", _sector_bookmark[8]);
	printf("9     : goto sector %-5lld \n", _sector_bookmark[9]);
	printf("S4~S9 : bookmark sector    \n");
	printf("G     : goto sector        \n");
	printf("H     : goto cluster (data)\n");
	printf("U_ARR : rewind %d %-15s\n", _adv_N, _adv_N == 1 ? "sector" : "sectors");
	printf("D_ARR : forwrd %d %-15s\n", _adv_N, _adv_N == 1 ? "sector" : "sectors");
	printf("LR_ARR: set N=%-10d \n", _adv_N);
	printf("\n--- DISP ---                \n");
	printf("INS   : edit current sector \n");
	printf("[S] F1: toggle disp 1 modes \n");
	printf("[S] F2: toggle disp 2 modes \n");
	printf("D     : show drive info     \n");
	printf("PAUSE : %-15s\n", _locked ? "UNLOCK" : "DISMOUNT & LOCK");
	printf("ESC   : exit                \n");

	switch(_show_drive_info) {
		case NO_INFO: printf("\033[0J"); break;
		case DEVINFO: printf("\n--- GEOMETRY ---\n"); show_geom_info(); break;
		case F32INFO: printf("\n--- FAT32 ---\n"); show_fat32_info(); break;
		case FSIINFO: printf("\n--- FSINFO ---\n"); show_fsi_info(); break;
		default: clear_column(15); break;
	}
}

static const DialogOptions quit_dialog_options = {
	{"Cancel", []() { return DIALOG_NO_SELECTION; }},
	{"OK"    , []() { return 1; }},
};

void DiskExplorer::run(void) {
	static const DWORD LEN = _device.geometry().BytesPerSector;

	LONGLONG fat1_offset =  _sector0.BPB_RsvdSecCnt() * LEN;
	LONGLONG fat2_offset = (_sector0.BPB_RsvdSecCnt() + _sector0.BPB_FATSz32()) * LEN ;

	_ui.clear_screen();
	print_commands();
	_page[0].print();
	_page[1].print();

	KeyCode key = TERMUI_KEY_UNDEFINED;
	Dialog quit_dialog("Confirm exit?", quit_dialog_options);
	while ((key = _ui.read()) != TERMUI_KEY_ESC || quit_dialog.query(93,20) == DIALOG_NO_SELECTION) {
		switch(key) {
		case TERMUI_KEY_PAUSE      : toggle_lock()                                    ;                  break;
		case TERMUI_KEY_F1         : _page[0].toggle_view()                           ;                  break;
		case TERMUI_KEY_F2         : _page[1].toggle_view()                           ;                  break;
		case TERMUI_KEY_SHIFT_F1   : _page[0].switch_buff()                           ;                  break;
		case TERMUI_KEY_SHIFT_F2   : _page[1].switch_buff()                           ;                  break;
		case TERMUI_KEY_0          : goto_offset(0)                                   ; read_setpages(); break;
//		case TERMUI_KEY_1          : goto_offset(_sector0.BPB_FSInfo() * LEN)         ; read_setpages(); break;
		case TERMUI_KEY_1          : goto_offset(fat1_offset)                         ; read_setpages(); break;
		case TERMUI_KEY_2          : goto_offset(fat2_offset)                         ; read_setpages(); break;
		case TERMUI_KEY_3          : goto_offset(fds_offset())                        ; read_setpages(); break;
		case TERMUI_KEY_4          : goto_offset(_sector_bookmark[4] * LEN)           ; read_setpages(); break;
		case TERMUI_KEY_5          : goto_offset(_sector_bookmark[5] * LEN)           ; read_setpages(); break;
		case TERMUI_KEY_6          : goto_offset(_sector_bookmark[6] * LEN)           ; read_setpages(); break;
		case TERMUI_KEY_7          : goto_offset(_sector_bookmark[7] * LEN)           ; read_setpages(); break;
		case TERMUI_KEY_8          : goto_offset(_sector_bookmark[8] * LEN)           ; read_setpages(); break;
		case TERMUI_KEY_9          : goto_offset(_sector_bookmark[9] * LEN)           ; read_setpages(); break;
		case TERMUI_KEY_DOLL       : _sector_bookmark[4]=(_device.offset()-2*LEN)/LEN ;                  break;
		case TERMUI_KEY_PERC       : _sector_bookmark[5]=(_device.offset()-2*LEN)/LEN ;                  break;
		case TERMUI_KEY_CFLEX      : _sector_bookmark[6]=(_device.offset()-2*LEN)/LEN ;                  break;
		case TERMUI_KEY_AMP        : _sector_bookmark[7]=(_device.offset()-2*LEN)/LEN ;                  break;
		case TERMUI_KEY_STAR       : _sector_bookmark[8]=(_device.offset()-2*LEN)/LEN ;                  break;
		case TERMUI_KEY_OPAR       : _sector_bookmark[9]=(_device.offset()-2*LEN)/LEN ;                  break;
		case TERMUI_KEY_d          :
		case TERMUI_KEY_D          : toggle_info_mode()                               ;                  break;
		case TERMUI_KEY_f          :
		case TERMUI_KEY_F          : printf(LAYOUT_FREE "  WIP search")               ;                  break;
		case TERMUI_KEY_g          :
		case TERMUI_KEY_G          : input_and_goto_sector()                          ; /* reads/sets */ break;
		case TERMUI_KEY_h          :
		case TERMUI_KEY_H          : input_and_goto_cluster_data()                    ; /* reads/sets */ break;
		case TERMUI_KEY_ARROW_UP   : advance_sectors(-(_adv_N+2) * (long) LEN)        ; read_setpages(); break;
		case TERMUI_KEY_ARROW_DOWN : advance_sectors( (_adv_N-2) * (long) LEN)        ; read_setpages(); break;
		case TERMUI_KEY_ARROW_RIGHT: _adv_N = _adv_N < 100000 ? _adv_N * 10 : 1000000 ;                  break;
		case TERMUI_KEY_ARROW_LEFT : _adv_N = _adv_N > 10     ? _adv_N / 10 : 1       ;                  break;
		case TERMUI_KEY_INSERT     : _editor.edit(_device)                            ; read_setpages(); break;
		case TERMUI_KEY_SPACE      : setpages()                                       ;                  break;
		default                    : setpages()                                       ;                  break;
		}
		print_commands();
		_page[0].print();
		_page[1].print();
	}
	_ui.clear_screen();
}

void DiskExplorer::toggle_lock(void) {
	if (_locked) {
		_locked = false;
		try { _device.unlock_drive(); }
		catch (std::exception & e) {
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
void DiskExplorer::input_and_goto_cluster_raw(void) {
	_input.set_msg("\033[1mGOTO RAW CLUSTER:\033[0m ");
	LONGLONG target_cluster;
	if (_input.get(&target_cluster, TERMUI_KEY_UNDEFINED, true)) {
		goto_offset(target_cluster * cluster_size());
		read_setpages();
	}
}
void DiskExplorer::input_and_goto_cluster_data(void) {
	_input.set_msg("\033[1mGOTO DATA CLUSTER:\033[0m ");
	LONGLONG target_cluster;
	if (_input.get(&target_cluster, TERMUI_KEY_UNDEFINED, true)) {
		goto_offset(first_sector_of_cluster(target_cluster) * _device.geometry().BytesPerSector);
		read_setpages();

		printf(LAYOUT_FREE "          fat_sec_num: %-10lld fat_ent_off %-10lld"
			, fat_sec_num(target_cluster)
			, fat_ent_off(target_cluster)
		);
	}
}

void DiskExplorer::show_geom_info(void) const {
	wprintf(L"MediaType        : %d\n",   _device.geometry().MediaType);
	wprintf(L"Cylinders (Quad) : %lld\n", _device.geometry().Cylinders.QuadPart);
	wprintf(L"Cylinders (High) : %d\n",   _device.geometry().Cylinders.HighPart);
	wprintf(L"Cylinders (Low)  : %d\n",   _device.geometry().Cylinders.LowPart);
	wprintf(L"TracksPerCylinder: %d\n",   _device.geometry().TracksPerCylinder);
	wprintf(L"SectorsPerTrack  : %d\n",   _device.geometry().SectorsPerTrack);
	wprintf(L"BytesPerSector   : %d\n",   _device.geometry().BytesPerSector);
	wprintf(L"Total capacity   : %lld B\n", _device.capacity());
	wprintf(L"      %s\n"    , size_to_wstring(_device.capacity(), true));
	//fwprintf(_out, L"NBytes            %d\n", _geom_nbytes);
	clear_column(5);
}


void DiskExplorer::show_fat32_info(void) const {
	static char buf[12];
	snprintf(buf, 12, "%s", _sector0.BS_VolLab());

	printf("BPB_FATSz16    : %u\n", _sector0.BPB_FATSz16());
	printf("BPB_FATSz32    : %u\n", _sector0.BPB_FATSz32());
	printf("BPB_NumFATs    : %u\n", _sector0.BPB_NumFATs());
	printf("BPB_RsvdSecCnt : %u\n", _sector0.BPB_RsvdSecCnt());
	printf("Bytes/sector   : %u\n", _sector0.BPB_BytsPerSec());
	printf("Sectors/cluster: %u\n", _sector0.BPB_SecPerClus());
	printf("BPB_RootClus   : %d\n", _sector0.BPB_RootClus());
	printf("BPB_FSInfo     : %u\n", _sector0.BPB_FSInfo());
	printf("BS_VolLab      : %s\n", buf);
	printf("\n");
	printf("Cluster size   : %lu\n" , cluster_size());
	printf("FirstDataSector: %lu\n" , first_data_sector());
	printf("FDS offset     : %llu\n", fds_offset());
	clear_column(1);
}
void DiskExplorer::show_fsi_info(void) const {
	printf("FSI_LeadSig   : 0x%0lX\n", _fsi_sector.FSI_LeadSig   ());
	printf("              : 0x41615252\n");
	//printf("FSI_Reserved1 : %lu\n", _fsi_sector.FSI_Reserved1 ());
	printf("FSI_StrucSig  : 0x%0lX\n", _fsi_sector.FSI_StrucSig  ());
	printf("              : 0x61417272\n");
	printf("FSI_Free_Count: %lu\n", _fsi_sector.FSI_Free_Count());
	printf("FSI_Nxt_Free  : %lu\n", _fsi_sector.FSI_Nxt_Free  ());
	//printf("FSI_Reserved2 : %lu\n", _fsi_sector.FSI_Reserved2 ());
	printf("FSI_TrailSig  : 0x%0lX\n", _fsi_sector.FSI_TrailSig  ());
	printf("              : 0xAA550000\n");
	clear_column(5);
}





void DiskExplorer::clear_column(int n) const {
	for(int i = 0; i < n; ++i) {
		printf("%*s\n", 32, "");
	}
}
void DiskExplorer::show_fat32_info_ext(void) const {
	//printf("BS_jmpBoot    : %u", _sector0.BS_jmpBoot    ()); //                    &sector[0]   
	//printf("BS_OEMName    : %u", _sector0.BS_OEMName    ()); //                    &sector[3]   
	///**/printf("BPB_BytsPerSec: %u", _sector0.BPB_BytsPerSec()); // *((unsigned short*)&sector[11]) 
	///**/printf("BPB_SecPerClus: %u", _sector0.BPB_SecPerClus()); //                     sector[13]  
	///**/printf("BPB_RsvdSecCnt: %u", _sector0.BPB_RsvdSecCnt()); // *((unsigned short*)&sector[14]) 
	///**/printf("BPB_NumFATs   : %u", _sector0.BPB_NumFATs   ()); //                     sector[16]  
	//printf("BPB_RootEntCnt: %u", _sector0.BPB_RootEntCnt()); // *((unsigned short*)&sector[17]) 
	//printf("BPB_TotSec16  : %u", _sector0.BPB_TotSec16  ()); // *((unsigned short*)&sector[19]) 
	//printf("BPB_Media     : %u", _sector0.BPB_Media     ()); //                     sector[21]  
	///**/printf("BPB_FATSz16   : %u", _sector0.BPB_FATSz16   ()); // *((unsigned short*)&sector[22]) 
	//printf("BPB_SecPerTrk : %u", _sector0.BPB_SecPerTrk ()); // *((unsigned short*)&sector[24]) 
	//printf("BPB_NumHeads  : %u", _sector0.BPB_NumHeads  ()); // *((unsigned short*)&sector[26]) 
	//printf("BPB_HiddSec   : %u", _sector0.BPB_HiddSec   ()); // *((unsigned int*)  &sector[28]) 
	//printf("BPB_TotSec32  : %u", _sector0.BPB_TotSec32  ()); // *((unsigned int*)  &sector[32]) 
	///**/printf("BPB_FATSz32   : %u", _sector0.BPB_FATSz32   ()); // *((unsigned int*)  &sector[36]) 
	//printf("BPB_ExtFlags  : %u", _sector0.BPB_ExtFlags  ()); // *((unsigned short*)&sector[40]) 
	//printf("BPB_FSVer     : %u", _sector0.BPB_FSVer     ()); // *((unsigned short*)&sector[42]) 
	///**/printf("BPB_RootClus  : %u", _sector0.BPB_RootClus  ()); // *((unsigned int*)  &sector[44]) 
	///**/printf("BPB_FSInfo    : %u", _sector0.BPB_FSInfo    ()); // *((unsigned short*)&sector[48]) 
	//printf("BPB_BkBootSec : %u", _sector0.BPB_BkBootSec ()); // *((unsigned short*)&sector[50]) 
	//printf("BPB_Reserved  : %u", _sector0.BPB_Reserved  ()); //                    &sector[52]  
	//printf("BS_DrvNum     : %u", _sector0.BS_DrvNum     ()); //                     sector[64]  
	//printf("BS_Reserved1  : %u", _sector0.BS_Reserved1  ()); //                     sector[65]  
	//printf("BS_BootSig    : %u", _sector0.BS_BootSig    ()); //                     sector[66]  
	//printf("BS_VolID      : %u", _sector0.BS_VolID      ()); // *((unsigned int*)  &sector[67]) 
	//printf("BS_VolLab     : %u", _sector0.BS_VolLab     ()); //                    &sector[71]  
	//printf("BS_FilSysType : %u", _sector0.BS_FilSysType ()); //                    &sector[82]  
	//printf("SigByte1      : %u", _sector0.SigByte1      ()); //                     sector[510] 
	//printf("SigByte2      : %u", _sector0.SigByte2      ()); //                     sector[511] 
	///**/printf("\n");
	///**/printf("Cluster size   : %lu\n" , cluster_size());
	///**/printf("FirstDataSector: %lu\n" , first_data_sector());
	///**/printf("FDS offset     : %llu\n", fds_offset());
}


ULONG DiskExplorer::cluster_size(void) const {
	return _sector0.BPB_BytsPerSec() * _sector0.BPB_SecPerClus();
}
LONG DiskExplorer::first_data_sector(void) const {
	return _sector0.BPB_RsvdSecCnt() + _sector0.BPB_FATSz32() * _sector0.BPB_NumFATs();
}
LONGLONG DiskExplorer::fds_offset(void) const {
	return first_data_sector() * _sector0.BPB_BytsPerSec();
}
LONGLONG DiskExplorer::first_sector_of_cluster(LONGLONG N) const {
	return ((N - 2) * _sector0.BPB_SecPerClus()) + first_data_sector();
}

LONGLONG DiskExplorer::fat_sec_num(LONGLONG N) const {
	// If(BPB_FATSz16 != 0)
	// FATSz = BPB_FATSz16;
	// Else
	// FATSz = BPB_FATSz32;
	// If(FATType == FAT16)
	// FATOffset = N * 2;
	// Else if (FATType == FAT32)
	// FATOffset = N * 4;
	// ThisFATSecNum = BPB_ResvdSecCnt + (FATOffset / BPB_BytsPerSec);
	// ThisFATEntOffset = REM(FATOffset / BPB_BytsPerSec);
	// REM(…) is the remainder operator. That means the remainder a
	unsigned int fatsz = _sector0.BPB_FATSz32();
	LONGLONG fat_offset = N * 4;
	return _sector0.BPB_RsvdSecCnt() + fat_offset / _sector0.BPB_BytsPerSec();
}
LONGLONG DiskExplorer::fat_ent_off(LONGLONG N) const {
	unsigned int fatsz = _sector0.BPB_FATSz32();
	LONGLONG fat_offset = N * 4;
	return fat_offset % _sector0.BPB_BytsPerSec();
}

