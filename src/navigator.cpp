#include "navigator.h"
#include "term_ui.h"
#include "device.h"
#include "layout.h"
#include "fat32.h"
#include "dialog.h"

#include <cstdio>
#include <stdexcept>

using std::vector;
using std::deque;


enum FAT_Code : unsigned int {
	FAT_RESERVED1 = 0x0FFFFFF8,
	FAT_RESERVED2 = 0x7FFFFFFF,
	FAT_EOC       = 0x0FFFFFFF,
	FAT_EMPTY     = 0x00000000,
};


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
		_max_selection = 1;
		_view_mode = ViewMode::PAGE;

		read_FAT(0);
		read_FAT(1);

		retrieve_directory_at(2);
		_current_directory = 2;
		_upstream_directory = 2;
		_position[2] = 0;


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
			case TERMUI_KEY_ARROW_UP   : move_sel(  -1) ; break;
			case TERMUI_KEY_ARROW_DOWN : move_sel(   1) ; break;
			case TERMUI_KEY_ARROW_LEFT : move_sel(  -1) ; break;
			case TERMUI_KEY_ARROW_RIGHT: move_sel(   1) ; break;
			case TERMUI_KEY_PGUP       : move_sel(-100) ; break;
			case TERMUI_KEY_PGDOWN     : move_sel( 100) ; break;
			case TERMUI_KEY_TAB        : toggle_view()  ; break;
			case TERMUI_KEY_RETURN     : nav_downstream() ; break;
			case TERMUI_KEY_BACKSPACE  : nav_upstream() ; break;
			default: break;
		}
		_max_selection = print_main();
	}

	_term->clear_screen();
}

int Navigator::print_main(void) const {
	int max;
	switch(_view_mode) {
		case ViewMode::FAT1: max = print_FAT(0); break;
		case ViewMode::FAT2: max = print_FAT(1); break;
		case ViewMode::PAGE: max = print_directory_at(_current_directory); break;
		default:             max = _max_selection;
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
	int * selected;
	switch(_view_mode) {
		case ViewMode::FAT1: selected = &_position_fat[0]; break;
		case ViewMode::FAT2: selected = &_position_fat[1]; break;
		case ViewMode::PAGE: selected = &_position[_current_directory]; break;
		default: return;
	}

	*selected = *selected + off;
	if (*selected < 0) *selected = 0;
	if (*selected >= _max_selection) *selected = _max_selection - 1;
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
	int selected = _position_fat[nfat];
	int i0 = set_min_max_i(max, selected, NL);

	static char buffer1[64];
	static char buffer2[64];
	
	printf("\033[%d;%dH\033[0KFAT %d        INDEX -> VALUE", _Y0, _X0,  nfat+1);
	for (int i = 0; i < NL; ++i) {
		int index = i + i0;
		const char * attr1 = index == selected ? "\033[7m" : "";
		const char * attr2 = index == selected ? "\033[27m" : "";

		snprintf(buffer1, 64, "%08X (%d)", index, index);
		unsigned int value = _FAT[nfat][index];
		switch(value) {
			case FAT_RESERVED1: snprintf(buffer2, 64, "(reserved.1)"); break;
			case FAT_RESERVED2: snprintf(buffer2, 64, "(reserved.2)"); break;
			case FAT_EMPTY: snprintf(buffer2, 64, "(empty)"); break;
			case FAT_EOC: snprintf(buffer2, 64, "(EOC)"); break;
			default: snprintf(buffer2, 64, "%08X (%d)", value, value); break;
		}
		
		printf("\033[%d;%dH\033[0K%s%18s -> %-18s%s", _Y0+i+1, _X0, attr1, buffer1, buffer2, attr2);
	}

	return max;
}

void concat_name(char * tgt, const char * src) {
	snprintf(tgt, 9, "%s", src);
	
	if (src[8] != ' ') {
		int pos = 0;
		while(pos < 8 && tgt[pos] != ' ') { ++pos; }
		tgt[pos++] = '.';
		snprintf(tgt + pos, 4, "%s", src + 8);
	}
}

void concat_long_name(char * tgt, const char * src1, const char * src2, const char * src3) {
	int j = 0;
	for (int i = 0; i < 10; i+= 2) { tgt[j++] = src1[i]; }
	for (int i = 0; i < 12; i+= 2) { tgt[j++] = src2[i]; }
	for (int i = 0; i <  4; i+= 2) { tgt[j++] = src3[i]; }
	tgt[j] = 0;
}

int Navigator::print_directory_at(int N) const {
	static const int NL = 45;
	static char short_name[16];
	static char long_name[16];
	const deque<entry> & directory = _directory_tree.at(N);
	int max = (int) directory.size();
	int selected = _position.at(_current_directory);
	int i0 = set_min_max_i(max, selected, NL);


	printf("\033[%d;%dH\033[0K    N   NAME            ----  -----  ---    START  SIZE", _Y0, _X0);
	for (int i = 0; i < NL; ++i) {
		int index = i + i0;
		const char * attr1 = index == selected ? "\033[7m" : "";
		const char * attr2 = index == selected ? "\033[27m" : "";

		const char * stats = directory[index].is_ghost() ? "GHOST" : "";
		const char * ltype = directory[index].is_long()  ? "LONG" : "";
		const char * dtype = directory[index].is_dir()  ? "DIR" : "";
		const char * useok = directory[index].is_dir() || directory[index].is_ghost() ? "-->" : "";

		

		if (directory[index].is_long()) {
			concat_long_name(long_name, (char*)directory[index].LDIR.Name1, (char*)directory[index].LDIR.Name2, (char*)directory[index].LDIR.Name3);
			unsigned short ord = directory[index].LDIR.Ord;
			unsigned short cks = directory[index].LDIR.Chksum;
			printf("\033[%d;%dH\033[0K%s%5d   %-13s   %4s  %5s  %3s      %3u  %-3u            %s", _Y0+i+1, _X0, attr1, index, long_name, ltype, stats, dtype, ord, cks, attr2);
			continue;
		} else {
			broken_int32 fc;
			concat_name(short_name, (char*)directory[index].DIR.Name);
			fc.half.lower = directory[index].DIR.FstClusLO;
			fc.half.upper = directory[index].DIR.FstClusHI;
			int length = directory[index].DIR.FileSize;
			printf("\033[%d;%dH\033[0K%s%5d   %-13s   %4s  %5s  %3s   %6d  %-10d  %3s%s", _Y0+i+1, _X0, attr1, index, short_name, ltype, stats, dtype, fc.full, length, useok, attr2);
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
unsigned char* Navigator::read_cluster(unsigned long N) {
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

deque<entry> Navigator::read_directory_at(unsigned long N) {
	deque<entry> result;

	unsigned char * cluster = read_cluster(N);
	entry* entries = (entry *) cluster;
	for (int i = 0; i < n_cluster_entries(); ++i) {
		result.emplace_back(&entries[i]);
	}		
	delete[] cluster;
	return result;
}

deque<entry> Navigator::read_full_directory_at(unsigned long N) {
	switch(_FAT[0][N]) {
		case FAT_EMPTY:
		case FAT_RESERVED1:
		case FAT_RESERVED2:
			return deque<entry>(0);
	} 

	deque<entry> result = read_directory_at(N);
	unsigned int next = _FAT[0][N];
	while (next != FAT_EOC) {
		deque<entry> next_entries = read_directory_at(next);
		result.insert(result.end(), next_entries.begin(), next_entries.end());
		next = _FAT[0][next];
	}
	
	return result;
}

deque<entry> & Navigator::retrieve_directory_at(unsigned long N) {
	if (!_directory_tree.contains(N)) {
		_directory_tree[N] = read_full_directory_at(N);
	}
	return _directory_tree[N];
}

void Navigator::nav_downstream(void) {
	switch(_view_mode) {
		case ViewMode::FAT1: return;
		case ViewMode::FAT2: return;
	}

	int selected = _position.at(_current_directory);
	const entry & entry = _directory_tree.at(_current_directory).at(selected);
	if (entry.is_long()) {
		// do nothing
	} else {
		if (entry.is_dir()) {
			broken_int32 addr;
			addr.half.lower = entry.DIR.FstClusLO;
			addr.half.upper = entry.DIR.FstClusHI;
			if (addr.full == 0) { addr.full = 2; }
			if (retrieve_directory_at(addr.full).size() > 0) {
				_upstream_directory = _current_directory;
				_current_directory = addr.full;
				_position[addr.full];
			}
		} else {
			if (entry.is_ghost()) {
				// ghost file opts
			} else {
				// alive file opts
			}
		}
	}
}

void Navigator::nav_upstream(void) {
	switch(_view_mode) {
		case ViewMode::FAT1: return;
		case ViewMode::FAT2: return;
	}
	if(_current_directory == _upstream_directory) { return; }

	deque<entry> & updir = _directory_tree.at(_upstream_directory);
	_current_directory = _upstream_directory;

	broken_int32 addr;
	addr.half.lower = updir[1].DIR.FstClusLO;
	addr.half.upper = updir[1].DIR.FstClusHI;
	_upstream_directory = (updir[0].DIR.Name[0] == '.' && updir[1].DIR.Name[1] == '.') ? addr.full : 2;
	if (_upstream_directory == 0) _upstream_directory = 2;
}
