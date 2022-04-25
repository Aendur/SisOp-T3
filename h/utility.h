#ifndef UTILITY_H
#define UTILITY_H

// Converts a size value to human-readable string
const char* size_to_string(size_t size, bool append_unit);
const wchar_t* size_to_wstring(size_t size, bool append_unit);
const char * colorize_char(char c, char ctl, bool negative);
const char * colorize_char(unsigned char c, int width, bool negative);
const char * colorize_char(char c, const char * ctl, int width, bool negative);

#endif
