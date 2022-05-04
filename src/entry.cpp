
#include "entry.h"
#include "longshort.h"
#include <cstdio>

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

const char* entry::get_str(void) const {
	static char short_name[16];
	static char long_name[16];
	static char output[128];

	const char * stats = is_ghost() ? "GHOST" : "";
	const char * ltype = is_long()  ? "LONG" : "";
	const char * dtype = is_dir()  ? "DIR" : "";
	const char * useok = is_dir() != is_ghost() ? "-->" : "";

	if (is_empty()) {
		snprintf(output, 128, "%     -53s", "(NO DATA)");
	} else if (is_long()) {
		concat_long_name(long_name, (char*)LDIR.Name1, (char*)LDIR.Name2, (char*)LDIR.Name3);
		unsigned short ord = LDIR.Ord;
		unsigned short cks = LDIR.Chksum;
		snprintf(output, 128, "%-13s   %4s  %5s  %3s      %3u  %-3u            ", long_name, ltype, stats, dtype, ord, cks);
	} else {
		ulongshort fc;
		concat_name(short_name, (char*)DIR.Name);
		fc.half.lower = DIR.FstClusLO;
		fc.half.upper = DIR.FstClusHI;
		int length = DIR.FileSize;
		snprintf(output, 128, "%-13s   %4s  %5s  %3s   %6d  %-10d  %3s", short_name, ltype, stats, dtype, fc.full, length, useok);
		//output[0] = '.';
	}
	return output;
}

