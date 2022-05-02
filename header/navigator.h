#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include <map>
#include <vector>
#include "entry.h"


class TermUI;
class Device;
class Dialog;
struct fat32;
class Navigator {
private:
	enum class ViewMode {
		FAT1 = 0,
		FAT2 = 1,
		PAGE = 2,
		END,
	};

	TermUI *   _term = nullptr;
	Device * _device = nullptr;
	fat32 * _sector0 = nullptr;
	bool _initialized =  false;

	std::map<unsigned long long, std::vector<entry>> _directory_tree;
	unsigned int* _FAT[2] = { nullptr, nullptr };

	int _X0;
	int _Y0;
	int _selected;
	int _max_selection;
	ViewMode _view_mode = ViewMode::FAT1;
	inline void toggle_view(void) { _view_mode = (ViewMode)((1 + (int)_view_mode) % (int) ViewMode::END); }

	int print_main(void) const;
	int print_FAT(int nfat) const;
	int print_directory_at(int N) const;
	void print_commands(void) const;
	void move_sel (int off);
	

	
	// allocs _FAT[fatn]
	void read_FAT(int fatn);
	unsigned char* read_cluster(void);
	unsigned char* read_cluster(long long N);

	std::vector<entry> & read_directory_at(long long N);

	int n_fat_entries(void) const;
	int n_cluster_entries(void) const;

public:
	~Navigator(void);
	//void init(TermUI * t, Device * d) { if (!_initialized) {_term = t; _device = d; _initialized = true;} }
	void init(TermUI * t, Device * d, fat32 * s0);
	void navigate(void);
};


#endif


