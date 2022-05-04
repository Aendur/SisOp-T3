#ifndef LONGSHORT_H
#define LONGSHORT_H

typedef union {
	struct _shortshort {
		unsigned short lower;
		unsigned short upper;
	} half;
	unsigned long full;
} longshort;

#endif
