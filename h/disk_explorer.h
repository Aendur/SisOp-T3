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
	BYTE * _buffer = nullptr;
	Device _device;
	Page _page;

	fat32 _sector0;

	void set_print(void);
	void read_set_print(void);
	void advance_sectors(LONGLONG offset);
	void goto_sector(LONGLONG offset);
	void proc_fat32_info(void);

public:
	DiskExplorer(void);
	//~DiskExplorer(void);

	void run(void);
};

#endif