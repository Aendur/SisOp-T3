#ifndef LONGSHORT_H
#define LONGSHORT_H

typedef union {
	struct _shortshort {
		unsigned short lower;
		unsigned short upper;
	} half;
	unsigned long full;
} ulongshort;

typedef union {
	struct _bytebyte {
		unsigned char lower;
		unsigned char upper;
	} half;
	unsigned short full;
} ushortbyte;

#endif
