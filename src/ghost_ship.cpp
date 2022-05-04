#include "ghost_ship.h"
#include "entry.h"
#include "seqfile.h"
#include "longshort.h"

#include "fat32.h"
#include "device.h"


bool GhostShip::board(const entry& ghost_entry) {
	bool requirements =
		ghost_entry.is_ghost() &&
		!ghost_entry.is_dir() &&
		!ghost_entry.is_long() &&
		!ghost_entry.is_empty();
	if (!requirements) { return false; }

	longshort cluster;
	cluster.half.lower = ghost_entry.DIR.FstClusLO;
	cluster.half.upper = ghost_entry.DIR.FstClusHI;
	
// 	long long offset = _sector0->first_sector_of_cluster(cluster.full) * _sector0->BPB_BytsPerSec();
// 	_device->seek(offset, false);
}


