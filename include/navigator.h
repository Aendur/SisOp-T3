#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include <map>
#include <deque>
#include "entry.h"
#include "entry_metadata.h"
#include "longshort.h"

class TermUI;
class Device;
class Dialog;
struct fat32;

typedef std::deque<EntryMetadata> Directory;

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

	std::map<unsigned long, Directory> _directory_tree;
	std::map<unsigned long, int> _position;
	unsigned long _current_directory;
	unsigned long _upstream_directory;
	
	unsigned int* _FAT[2] = { nullptr, nullptr };
	int  _position_fat[2] = { 0, 0 };

	int _X0;
	int _Y0;
	int _max_selection;

	ViewMode _view_mode;
	inline void toggle_view(void) { _view_mode = (ViewMode)((1 + (int)_view_mode) % (int) ViewMode::END); }

	
	int print_main(void) const;
	int print_FAT(int nfat) const;
	int print_directory_at(int N) const;
	void print_commands(void) const;
	void move_sel (int off);
	void move_to_last(void);
	void nav_upstream(void);
	void nav_downstream(void);
	void launch_ghost_ship(void);;

	// allocs _FAT[fatn]
	void read_FAT(int fatn);
	unsigned char* read_cluster(void);
	unsigned char* read_cluster(unsigned long N);

	Directory read_directory_at(unsigned long N);
	Directory read_full_directory_at(unsigned long N);
	Directory & retrieve_directory_at(unsigned long N);

	int n_fat_entries(void) const;
	int n_cluster_entries(void) const;

public:
	~Navigator(void);
	void init(TermUI * t, Device * d, fat32 * s0);
	void navigate(void);
};


#endif


