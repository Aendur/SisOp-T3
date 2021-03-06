#ifndef SEQ_FILE_H
#define SEQ_FILE_H

#include <string>
#include <random>

typedef char SeqMessage[4][256];

class SeqFile {
private:
	static const unsigned long long seed;
	static std::mt19937_64 RNG;

	std::string base_name;
	std::string extension;
	size_t file_size;
	char * file_buffer;

	unsigned long id1;
	unsigned long id2;
	char * head = nullptr;
	char * tail = nullptr;

	unsigned long gen_uuid(void);
	void init_rnd_bytes(void);
	void init_strings(void);
	void create_file(void);
	void create_page(size_t index, size_t max);
	void split_name(void);

public:
	inline static const int RF_HEAD_SIZE = 128;
	inline static const int RF_LINE_SIZE =  32;
	inline static const int RF_NAME_SIZE =  23;
	inline static const int RF_MINF_SIZE = 142;

	inline static const int RF_NAME_OFF =   5; inline static const int RF_NAME_LEN = 23;
	inline static const int RF_EXTN_OFF =  28; inline static const int RF_EXTN_LEN =  3;
	inline static const int RF_SIZE_OFF =  37; inline static const int RF_SIZE_LEN = 10;
	inline static const int RF_PART_OFF =  53; inline static const int RF_PART_LEN = 10;
	inline static const int RF_UID1_OFF =  69; inline static const int RF_UID1_LEN = 10;
	inline static const int RF_UID2_OFF =  85; inline static const int RF_UID2_LEN = 10;
	inline static const int RF_DATE_OFF = 101; inline static const int RF_DATE_LEN = 11;
	inline static const int RF_TIME_OFF = 118; inline static const int RF_TIME_LEN =  9;

	struct Header {
		char base_name[SeqFile::RF_NAME_LEN + 1];
		char extension[SeqFile::RF_EXTN_LEN + 1];
		char datestamp[SeqFile::RF_DATE_LEN + 1];
		char timestamp[SeqFile::RF_TIME_LEN + 1];
		unsigned long file_size;
		unsigned long part;
		unsigned long id1;
		unsigned long id2;
		bool valid = false;

		const char * get_str(void) const;
	};

	/// Checks if a chuck of data is part of a SeqFile
	/// \param memsrc  pointer to the data to be checked
	/// \param message an optional array to store validity checking status
	static Header check_file(const void * memsrc, char message[4][256] = nullptr);

	/// Checks if a SeqFile object contains valid SeqFile data
	/// \param memsrc  pointer to the file object to be checked
	/// \param message an optional array to store validity checking status
	static Header check_file(const SeqFile & src, char message[4][256] = nullptr);


	SeqFile(const char * name, const char * ext, size_t size, bool random);
	~SeqFile(void);
	void write(void) const;
};


#endif

