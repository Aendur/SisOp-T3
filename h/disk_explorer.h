#ifndef DISK_EXPLORER_H
#define DISK_EXPLORER_H

#include "term_ui.h"
#include "device.h"
#include <Windows.h>
#include <stack>


class DiskExplorer {
private:
	TermUI _ui;

	unsigned long long _offset_start = 0;
	unsigned long long _offset_current = 0;
	unsigned long long _buffer_length = 0;
	BYTE * _buffer = nullptr;
	Device _device;
public:
	DiskExplorer(void);
	//~DiskExplorer(void);

	void run(void);
};

#endif