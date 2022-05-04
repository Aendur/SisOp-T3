#ifndef GHOST_SHIP_H
#define GHOST_SHIP_H

struct entry;
struct fat32;
class Device;

class GhostShip {
	Device * _device;
	fat32 * _sector0;
public:
	GhostShip(Device * d, fat32 * s0) : _device(d), _sector0(s0) {}
	void launch(void);
	bool board(const entry& ghost_entry);
};

#endif
