#include "disk_explorer.h"
#include <stdexcept>
#include <vector>
#include <string>

DiskExplorer::DiskExplorer(void) {
	_ui.init();
	_page.init(&_ui);
	_device.open_drive('G');
	
	if (_device.geometry().BytesPerSector != 512) {
		throw std::runtime_error("mismatch assumed bytes per sector = 512");
	}
}

static const std::vector<std::pair<std::string, std::string>> commands = {
	{"0:"        , "show sector 0"      },
	{"UP:"       , "last sector"        },
	{"DOWN:"     , "next sector"        },
	{"RIGHT:"    , "rew 100 sectors"    },
	{"LEFT:"     , "fwd 100 sectors"    },
	{"PGUP:"     , "rew 10000 sectors"  },
	{"PGDOWN:"   , "fwd 10000 sectors"  },
	{"S_PGUP:"   , "rew 1000000 sectors"},
	{"S_PGDOWN:" , "fwd 1000000 sectors"},
	{"HOME:"     , "goto first sector"  },
	{"END:"      , "goto last sector"   },
	{"-----"     , "-----"              },
	{"D:"        , "show drive info"    },
	{"F:"        , "show fat32 info"    },
	{"Q:"        , "quit"               },
};


void print_commands(void) {
	printf("\n");
	for (const auto & i : commands) {
		printf("%-10s %-22s\n", i.first.c_str(), i.second.c_str());
	}
}

void DiskExplorer::run(void) {
	static const DWORD LEN = _device.geometry().BytesPerSector;
	_ui.clear_screen();
	print_commands();


	// save fat32 _sector0 data
	_device.read();
	memcpy(&_sector0, _device.buffer(), _device.geometry().BytesPerSector);
	set_print();

	
	KeyCode key = TERMUI_KEY_UNDEFINED;
	while ((key = _ui.read()) != TERMUI_KEY_Q) {
		switch(key) {
		case TERMUI_KEY_0:
			_page.set((PBYTE)(&_sector0), LEN, 0);
			_page.print(true);
			break;
		case TERMUI_KEY_D: printf("\033[0J"); _device.print_geometry(); _page.print(true); break;
		case TERMUI_KEY_F: proc_fat32_info(); _page.print(true); break;
		case TERMUI_KEY_ARROW_UP: advance_sectors(-2 * (long) LEN); read_set_print(); break;
		case TERMUI_KEY_ARROW_DOWN: read_set_print(); break;
		case TERMUI_KEY_ARROW_LEFT: advance_sectors(99 * (long) LEN); read_set_print(); break;
		case TERMUI_KEY_ARROW_RIGHT: advance_sectors(-101 * (long) LEN); read_set_print(); break;
		// case TERMUI_KEY_HOME: advance_sectors(-_device.offset()); read_set_print(); break;
		// case TERMUI_KEY_END: advance_sectors(_device.capacity()); read_set_print(); break;
		case TERMUI_KEY_HOME: goto_sector(0); read_set_print(); break;
		case TERMUI_KEY_END: goto_sector(-(long)LEN); read_set_print(); break;
		case TERMUI_KEY_PGUP: advance_sectors(-10001 * (long) LEN); read_set_print(); break;
		case TERMUI_KEY_PGDOWN: advance_sectors(9999 * (long) LEN); read_set_print(); break;
		case TERMUI_KEY_SHIFT_PGUP: advance_sectors(-1000001 * (long) LEN); read_set_print(); break;
		case TERMUI_KEY_SHIFT_PGDOWN: advance_sectors(999999 * (long) LEN); read_set_print(); break;
		default:
			set_print();
			break;
		}
	}
}

void DiskExplorer::set_print(void) {
	static const DWORD LEN = _device.geometry().BytesPerSector;
	_page.set(_device.buffer(), LEN, _device.offset() - LEN);
	_page.print(true);
}

void DiskExplorer::read_set_print(void) {
	_device.read();
	set_print();
}

void DiskExplorer::advance_sectors(LONGLONG offset) {
	static const DWORD LEN = _device.geometry().BytesPerSector;
	LONGLONG mod = _device.capacity() % LEN;
	LONGLONG max_off = _device.capacity() - mod - LEN;

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

void DiskExplorer::goto_sector(LONGLONG offset) {
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

void DiskExplorer::proc_fat32_info(void) {
	unsigned long cluster_size = _sector0.BPB_BytsPerSec() * _sector0.BPB_SecPerClus();
	size_t FirstDataSector = _sector0.BPB_RsvdSecCnt() + _sector0.BPB_FATSz32() * _sector0.BPB_NumFATs();
	size_t FDS_offset = FirstDataSector * _sector0.BPB_BytsPerSec();
	
	printf("\033[0J");
	printf("BPB_FATSz16:     %u\n", _sector0.BPB_FATSz16());
	printf("BPB_FATSz32:     %u\n", _sector0.BPB_FATSz32());
	printf("BPB_NumFATs:     %u\n", _sector0.BPB_NumFATs());
	printf("BPB_RsvdSecCnt:  %u\n", _sector0.BPB_RsvdSecCnt());
	printf("Bytes/sector:    %u\n", _sector0.BPB_BytsPerSec());
	printf("Sectors/cluster: %u\n", _sector0.BPB_SecPerClus());
	printf("Cluster size   : %u\n", cluster_size);
	printf("\n");
	printf("FirstDataSector: %llu\n", FirstDataSector);
	printf("FDS offset     : %llu\n", FDS_offset);
	printf("BPB_RootClus   : %d\n"  , _sector0.BPB_RootClus());

	// size_t FirstSectorofCluster = ((N - 2) * _sector0.BPB_SecPerClus()) + FirstDataSector
	// size_t FirstSectorofCluster = ((N - 2) * _sector0.BPB_SecPerClus()) + FirstDataSector
	// SEEK
	//FDS_offset
}
