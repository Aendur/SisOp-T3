#ifndef DISK_EXPLORER_H
#define DISK_EXPLORER_H

#include "term_ui.h"
#include "device.h"
#include "page.h"
#include "fat32.h"
#include "fsinfo.h"
#include "input_field.h"
#include "editor.h"
#include "navigator.h"
#include <Windows.h>

class DiskExplorer {
private:
	enum DriveInfoMode {
		F32INFO,
		FSIINFO,
		DEVINFO,
		NO_INFO,
	};

	struct SectorBookmark {
		char name[32];
		LONGLONG sector;
		int selection;
	};

	TermUI _ui;
	Device _device;
	Page _page[2];
	InputField _input;
	Editor _editor;
	Navigator _navigator;

	bool _select_mode = false;
	long _adv_N = 1;
	SectorBookmark _sector_bookmark[10];
	bool _show_bm_info = false;
	DriveInfoMode _show_drive_info = F32INFO;

	fat32 _sector0;
	fsinfo _fsi_sector;

	bool _locked = false;

	void setpages(void);
	void read_setpages(void);

	inline void toggle_info_mode(void) { _show_drive_info = (DriveInfoMode)((_show_drive_info + 1) % (NO_INFO + 1)); }

	void advance_sectors(LONGLONG offset);
	void goto_offset(LONGLONG offset);
	void toggle_lock(void);
	void input_and_goto_sector(void);
	void input_and_goto_cluster_data(void);
	void input_and_goto_fat(int nfat);

	void open_bookmark(int i);
	void bookmark_sector(int i);
	void load_bookmarks(void);
	void save_bookmarks(void);
		
	void show_geom_info(void) const;
	void show_fat32_info(void) const;
	void show_fsi_info(void) const;
	void show_fat32_info_ext(void) const;
	void show_entry_info(void) const;
	void print_commands(void) const;
	void print_bookmarks(void) const;


public:
	DiskExplorer(WCHAR drive);
	//~DiskExplorer(void);

	void run(void);
};

#endif