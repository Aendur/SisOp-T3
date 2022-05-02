#ifndef UTILITY_H
#define UTILITY_H

// Converts a size value to human-readable string
const char* size_to_string(size_t size, bool append_unit);
const wchar_t* size_to_wstring(size_t size, bool append_unit);


struct ColorizeOptions {
	unsigned char byte = 0;
	bool   chr_hex = true;
	const char * ctl_str = nullptr;
	bool negative = false;
	bool underline = false;
	int width = 1;
	int padding_left = 0;
	int padding_right = 0;
	int margin_left = 0;
	int margin_right = 0;
};

const char * colorize_byte(const struct ColorizeOptions & opts);

#endif
