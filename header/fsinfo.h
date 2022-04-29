#ifndef FAT32_FSINFO_SPEC_H
#define FAT32_FSINFO_SPEC_H

class fsinfo {
private:
	unsigned char sector[512];

public:
	inline const unsigned long  FSI_LeadSig   (void) const { return *(unsigned long*) &sector[0  ]; }   // 0     4
	inline const unsigned char* FSI_Reserved1 (void) const { return                   &sector[4  ]; }   // 4     480
	inline const unsigned long  FSI_StrucSig  (void) const { return *(unsigned long*) &sector[484]; }   // 484   4
	inline const unsigned long  FSI_Free_Count(void) const { return *(unsigned long*) &sector[488]; }   // 488   4
	inline const unsigned long  FSI_Nxt_Free  (void) const { return *(unsigned long*) &sector[492]; }   // 492   4
	inline const unsigned char* FSI_Reserved2 (void) const { return                   &sector[496]; }   // 496   12
	inline const unsigned long  FSI_TrailSig  (void) const { return *(unsigned long*) &sector[508]; }   // 508   4
};

//char .* Size=(([2-9])|(1[1-9]+))

#endif
