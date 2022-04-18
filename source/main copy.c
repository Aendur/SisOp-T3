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

const char* head[NUM_FILDS + 1] = {
	"File name: %*s\n",
	"Head size: %*d\n",
	"Tail size: %*d\n",
	"File size: %*d\n",
	"Timestamp: %*s\n",
	"-- START --\n"
};

const char tail[] = "--- END ---\n";   // 12

const char * get_timestamp(char * buffer) {
	time_t now = time(NULL);
	struct tm * utc = gmtime(&now);
	snprintf(buffer, FILD_SIZE + 1, "%4d-%02d-%02dT%02d:%02d:%02d",
		utc->tm_year, utc->tm_mon, utc->tm_mday,
		utc->tm_hour, utc->tm_min, utc->tm_sec
	);
	return buffer;
}

void write_head(FILE * f, const char * name) {
	char buffer[HEAD_SIZE + 1];
	char timebuffer[FILD_SIZE + 1];
	char * pos = buffer;

	snprintf(pos + 0 * (DESC_SIZE + FILD_SIZE), DESC_SIZE + FILD_SIZE + 1, head[0], FILD_SIZE, name);
	snprintf(pos + 1 * (DESC_SIZE + FILD_SIZE), DESC_SIZE + FILD_SIZE + 1, head[1], FILD_SIZE, HEAD_SIZE);
	snprintf(pos + 2 * (DESC_SIZE + FILD_SIZE), DESC_SIZE + FILD_SIZE + 1, head[2], FILD_SIZE, TAIL_SIZE);
	snprintf(pos + 3 * (DESC_SIZE + FILD_SIZE), DESC_SIZE + FILD_SIZE + 1, head[3], FILD_SIZE, -1);
	snprintf(pos + 4 * (DESC_SIZE + FILD_SIZE), DESC_SIZE + FILD_SIZE + 1, head[4], FILD_SIZE, get_timestamp(timebuffer));
	snprintf(pos + 5 * (DESC_SIZE + FILD_SIZE), DESC_SIZE                , head[5]);
	fwrite(buffer, sizeof(char), HEAD_SIZE, f);
}

void write_body(FILE * f, const char * body) {
	size_t size = strlen(body);
	fwrite(body, sizeof(char), size, f);
}

void write_tail(FILE * f) {
	fwrite(tail, sizeof(char), TAIL_SIZE, f);
}

void write_all(const char * name) {
	FILE * f = fopen(name, "w");
	write_head(f, name);
	write_body(f, "hi!\n");
	write_tail(f);
	fclose(f);

}

int main() {
	write_all("test.txt");
	return 0;
}

//void FileGenerator::GenerateFile(const char * path, unsigned long size) {
//	
//}

//aendur@DESK-00:~$ date -uIm
//2022-04-18T18:24+00:00
//aendur@DESK-00:~$ date -uIs
//2022-04-18T18:24:36Z