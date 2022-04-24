#ifndef DISK_EXPLORER_H
#define DISK_EXPLORER_H

#include "term_ui.h"
#include "device.h"
#include "page.h"
#include "fat32.h"
#include "input_field.h"
#include <Windows.h>

class DiskExplorer {
private:
	TermUI _ui;

	unsigned long long _offset_start = 0;
	unsigned long long _offset_current = 0;
	unsigned long long _buffer_length = 0;

	Device _device;
	Page _page;
	InputField _input;

	long _adv_N = 1;
	LONGLONG _sector_bookmark = 0;
	bool _extended_entry_info = false;

	fat32 _sector0;

	void setpage(void);
	void read_setpage(void);
	void advance_sectors(LONGLONG offset);
	void goto_sector(LONGLONG offset);
	void input_and_go(void);
	void show_geom_info(void) const;
	void show_fat32_info(void) const;
	void show_entry_info(void) const;
	void print_commands(void) const;
	
	ULONG cluster_size(void) const;
	LONG first_data_sector(void) const;
	LONGLONG fds_offset(void) const;

public:
	DiskExplorer(void);
	//~DiskExplorer(void);

	void run(void);
};

#endif