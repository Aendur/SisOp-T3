#ifndef UTILITY_H
#define UTILITY_H

#define STS_MAX_FORMAT_SIZE 32

// Converts a size value to human-readable string
const char* size_to_string(char * output, size_t size, bool append_unit);
const wchar_t* size_to_wstring(wchar_t * output, size_t size, bool append_unit);


#endif
