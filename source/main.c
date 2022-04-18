//#include "filegen.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

//using std::string;

#define DESC_SIZE 12
#define FILD_SIZE 20
#define NUM_FILDS  5
#define HEAD_SIZE DESC_SIZE + (DESC_SIZE + FILD_SIZE) * NUM_FILDS
#define TAIL_SIZE DESC_SIZE

const char * get_timestamp(char * buffer);
void write_head(FILE * f, const char * name);
void write_body(FILE * f, const char * body);
void write_tail(FILE * f);
void write_all(const char * name);

int main() {
	write_all("test.txt");
	return 0;
}

void write_head(FILE * f, const char * name) {
	static const char head[] =
		"File name: %*s\n" // 12 + N
		"Head size: %*d\n" // 12 + N
		"Tail size: %*d\n" // 12 + N
		"File size: %*d\n" // 12 + N
		"Timestamp: %*s\n" // 12 + N
		"-- START --\n"   // 12
	;

	char buffer[HEAD_SIZE + 1];
	char timebuffer[FILD_SIZE + 1];

	snprintf(buffer, HEAD_SIZE + 1, head,
		FILD_SIZE, name,
		FILD_SIZE, HEAD_SIZE,
		FILD_SIZE, TAIL_SIZE,
		FILD_SIZE, -1,
		FILD_SIZE, get_timestamp(timebuffer)
	);
	fwrite(buffer, sizeof(char), HEAD_SIZE, f);
}

void write_body(FILE * f, const char * body) {
	size_t size = strlen(body);
	fwrite(body, sizeof(char), size, f);
}

void write_tail(FILE * f) {
	static const char tail[] = "--- END ---\n";   // 12
	fwrite(tail, sizeof(char), TAIL_SIZE, f);
}

void write_all(const char * name) {
	FILE * f = fopen(name, "w");
	write_head(f, name);
	write_body(f, "hi!\n");
	write_tail(f);
	fclose(f);

}

const char * get_timestamp(char * buffer) {
	time_t now = time(NULL);
	struct tm * utc = gmtime(&now);
	snprintf(buffer, FILD_SIZE + 1, "%d-%02d-%02dT%02d:%02d:%02d",
		utc->tm_year + 11900, utc->tm_mon, utc->tm_mday,
		utc->tm_hour, utc->tm_min, utc->tm_sec
	);
	return buffer;
}

//void FileGenerator::GenerateFile(const char * path, unsigned long size) {
//	
//}


