#include "entry_metadata.h"

#include <cstring>

EntryMetadata::EntryMetadata(unsigned long clus, unsigned long sec, unsigned long pos, entry* src) : cluster(clus), sector(sec), position(pos) {
	memcpy(&this->data, src, sizeof(entry));
}

