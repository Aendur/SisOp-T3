#ifndef DISK_EXPLORER_H
#define DISK_EXPLORER_H

#include "term_ui.h"
#include "device.h"
#include "page.h"
#include "fat32.h"
#include <Windows.h>

class DiskExplorer {
private:
	TermUI _ui;

	unsigned long long _offset_start = 0;
	unsigned long long _offset_current = 0;
	unsigned long long _buffer_length = 0;

	Device _device;
	Page _page;
	long _adv_N = 1;

	fat32 _sector0;

	void setpage(void);
	void read_setpage(void);
	void advance_sectors(LONGLONG offset);
	void goto_sector(LONGLONG offset);
	void show_fat32_info(void);
	void show_entry_info(void);
	void print_commands(void);
	
	ULONG cluster_size(void) const;
	LONG first_data_sector(void) const;
	LONGLONG fds_offset(void) const;

public:
	DiskExplorer(void);
	//~DiskExplorer(void);

	void run(void);
};

#endif