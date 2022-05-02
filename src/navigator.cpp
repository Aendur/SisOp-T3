#include "navigator.h"
#include "term_ui.h"
#include "device.h"
#include "layout.h"
#include "fat32.h"
#include "dialog.h"

#include <cstdio>
#include <stdexcept>

using std::vector;

Navigator::~Navigator(void) {
	if (_FAT[0] != nullptr) {
		delete[] _FAT[0];
	}
	if (_FAT[1] != nullptr) {
		delete[] _FAT[1];
	}
}

int Navigator::n_fat_entries(void) const {
	return _sector0->BPB_FATSz32() * ((size_t)_sector0->BPB_BytsPerSec() / sizeof(unsigned int));
};

int Navigator::n_cluster_entries(void) const {
	return _sector0->cluster_size() / sizeof(entry);
}

void Navigator::init(TermUI * t, Device * d, fat32 * s0) {
	if (!_initialized) {
		_term = t;
		_device = d;
		_sector0 = s0;

		_X0 = 36;
		_Y0 = 2;
		_selected = 0;

		read_FAT(0);
		read_FAT(1);

		read_directory_at(2);

		_initialized = true;
	}
}

void Navigator::navigate(void) {
	static const DialogOptions quit_dialog_options = {
		{ "Stay"  , [](void) { return DIALOG_NO_SELECTION; } },
		{ "Exit"  , [](void) { return 1; } },
	};

	if (!_initialized) {
		printf(LAYOUT_FREE "navigator not initialized");
		return;
	}

	_term->clear_screen();
	_max_selection = print_main();

	KeyCode key = TERMUI_KEY_UNDEFINED;
	Dialog quit_dialog("Exit navigator?", quit_dialog_options);
	while ((key = _term->read()) != TERMUI_KEY_ESC || quit_dialog.query(93,20) == DIALOG_NO_SELECTION) {
		switch (key) {
			case TERMUI_KEY_ARROW_UP   : move_sel(   -1); break;
			case TERMUI_KEY_ARROW_DOWN : move_sel(    1); break;
			case TERMUI_KEY_ARROW_LEFT : move_sel(   -1); break;
			case TERMUI_KEY_ARROW_RIGHT: move_sel(    1); break;
			case TERMUI_KEY_PGUP       : move_sel(-1000); break;
			case TERMUI_KEY_PGDOWN     : move_sel( 1000); break;
			case TERMUI_KEY_TAB        : toggle_view()  ; break;
			default: break;
		}
		_max_selection = print_main();
	}

	_term->clear_screen();
}

int Navigator::print_main(void) const {
	int max;
	switch(_view_mode) {
		case ViewMode::FAT1:
		case ViewMode::FAT2: max = print_FAT((int) _view_mode); break;
		case ViewMode::PAGE: max = print_directory_at(2); break;
	}
	print_commands();
	return max;
}

void Navigator::print_commands(void) const {
	_term->clear_column(1,1,32,47);
	printf("\n--- NAV ---         \n");
	printf("ARROWS: move cursor   \n");
	printf("ESC   : exit navigator\n");
	printf("TAB   : toggle fat %d \n", (int)_view_mode + 1);
}

void Navigator::move_sel (int off) {
	_selected = _selected + off;
	if (_selected < 0) _selected = 0;
	if (_selected >= _max_selection) _selected = _max_selection - 1;
}

/// \param max maximum number of entries
/// \param current currently selected entry
/// \param height number of lines to print
int set_min_max_i(int max, int current, int height) {
	int half = height / 2;
	int i0 = (current < half) ? 0 : current - half;
	if (current >= max - half) { i0 = max - height; }
	return i0;
}

int Navigator::print_FAT(int nfat) const {
	static const int NL = 45;
	int max = n_fat_entries();
	int i0 = set_min_max_i(max, _selected, NL);

	for (int i = 0; i < NL; ++i) {
		int index = i + i0;
		if (index == _selected) {
			printf("\033[%d;%dH\033[7mFAT %d entry:%-10d %08X\033[27m", _Y0 + i, _X0, nfat+1, index, _FAT[nfat][index]);
		} else {
			printf("\033[%d;%dHFAT %d entry:%-10d %08X", _Y0 + i, _X0, nfat+1, index, _FAT[nfat][index]);
		}
	}

	return max;
}

int Navigator::print_directory_at(int N) const {
	static const int NL = 45;
	static char short_name[16];
	const vector<entry> & directory = _directory_tree.at(N);
	int max = (int) directory.size();
	int i0 = set_min_max_i(max, _selected, NL);
	
	for (int i = 0; i < NL; ++i) {
		snprintf(short_name, 9, "%s", directory[i].dir.ds.DIR_Name);
		short_name[8] = '.';
		snprintf(short_name + 9, 3, "%s", directory[i].dir.ds.DIR_Name + 8);

		int index = i + i0;
		if (index == _selected) {
			printf("\033[%d;%dH\033[7m%d. %s\033[27m", _Y0 + i, _X0, index, short_name);
		} else {
			printf("\033[%d;%dH%d. %s", _Y0 + i, _X0, index, short_name);
		}
	}

	return max;
}

void Navigator::read_FAT(int fatn) {
	printf("loading FAT%d info...\n", fatn);
	long long offset = (_sector0->BPB_RsvdSecCnt() + _sector0->BPB_FATSz32() * fatn) * _sector0->BPB_BytsPerSec();
	_device->seek(offset, false);
	_FAT[fatn] = new unsigned int[n_fat_entries()];
	
	int sector = 0;
	int n_sectors = _sector0->BPB_FATSz32();
	int n_sectors_pair = n_sectors - n_sectors % 2;

	unsigned char * position = (unsigned char*) _FAT[fatn];
	while (sector < n_sectors_pair) {
		_device->read();
		
		memcpy(position, _device->buffer(0), _sector0->BPB_BytsPerSec());
		position += _sector0->BPB_BytsPerSec();
		memcpy(position, _device->buffer(1), _sector0->BPB_BytsPerSec());
		position += _sector0->BPB_BytsPerSec();
		sector += 2;
	}
	if (sector < n_sectors) {
		_device->read();
		memcpy(position, _device->buffer(0), _sector0->BPB_BytsPerSec());
		position += _sector0->BPB_BytsPerSec();
		sector += 1;
		printf("%d\n", sector);
	}
}

// MUST delete[] the returned pointer
unsigned char* Navigator::read_cluster(long long N) {
	long long offset = _sector0->first_sector_of_cluster(N) * _sector0->BPB_BytsPerSec();
	_device->seek(offset, false);
	return read_cluster();
}

// MUST delete[] the returned pointer
unsigned char* Navigator::read_cluster(void) {
	static const unsigned long length = _sector0->cluster_size();
	unsigned char * cluster_buffer = new unsigned char[length];
	unsigned char * position = cluster_buffer;

	int sector = 0;
	while(sector < _sector0->BPB_SecPerClus()) {
		_device->read();
		
		memcpy(position, _device->buffer(0), _sector0->BPB_BytsPerSec());
		position += _sector0->BPB_BytsPerSec();
		
		memcpy(position, _device->buffer(1), _sector0->BPB_BytsPerSec());
		position += _sector0->BPB_BytsPerSec();
		
		sector += 2;
	}

	return cluster_buffer;
}

vector<entry> & Navigator::read_directory_at(long long N) {
	//if (_directory_tree.find(N) != _directory_tree.end()) {
	if (!_directory_tree.contains(N)) {
		unsigned char * cluster = read_cluster(N);
		entry* entries = (entry *) cluster;
		//printf("%zu\n", sizeof(entry));
		//getchar();
		//_device->seek(_sector0->fds_offset(), false);
		//memcpy(&_root_directory.reference_entry, _device->buffer(0), _device->geometry);
		_directory_tree[N] = vector<entry>();
		for (int i = 0; i < n_cluster_entries(); ++i) {
			_directory_tree[N].emplace_back(&entries[i]);
			//memcpy(&_directory_tree[N][i], &entries[i], sizeof(entry));
		}		
		delete[] cluster;

	}

	return _directory_tree[N];
}


