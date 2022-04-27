#ifndef DISK_EXPLORER_H
#define DISK_EXPLORER_H

#include "term_ui.h"
#include "device.h"
#include "page.h"
#include "fat32.h"
#include "input_field.h"
#include "editor.h"
#include <Windows.h>

class DiskExplorer {
private:
	TermUI _ui;

	unsigned long long _offset_start = 0;
	unsigned long long _offset_current = 0;
	unsigned long long _buffer_length = 0;

	Device _device;
	Page _page[2];
	InputField _input;
	Editor _editor;

	long _adv_N = 1;
	//LONGLONG _sector_bookmark = 0;
	bool _show_drive_info = true;

	fat32 _sector0;

	void setpages(void);
	void read_setpages(void);

	void advance_sectors(LONGLONG offset);
	void goto_sector(LONGLONG offset);
	void input_and_goto_sector(void);
	void input_and_goto_cluster_raw(void);
	void input_and_goto_cluster_data(void);
	void show_geom_info(void) const;
	void show_fat32_info(void) const;
	void show_entry_info(void) const;
	void print_commands(void) const;
	
	ULONG cluster_size(void) const;
	LONG first_data_sector(void) const;
	LONGLONG fds_offset(void) const;
	LONGLONG first_sector_of_cluster(LONGLONG N) const;

public:
	DiskExplorer(WCHAR drive);
	//~DiskExplorer(void);

	void run(void);
};

#endif