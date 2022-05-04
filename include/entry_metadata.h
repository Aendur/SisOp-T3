#ifndef ENTRY_METADATA_H
#define ENTRY_METADATA_H

#include "entry.h"

struct EntryMetadata {
	unsigned long cluster;
	unsigned long sector;
	unsigned long position;
	entry data;

	EntryMetadata(void) {}
	EntryMetadata(unsigned long clus, unsigned long sec, unsigned long pos, entry* src);
};

#endif


