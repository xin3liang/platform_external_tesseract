#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "baseapi.h"

#define FAILIF(cond, msg...) do {                        	 \
        if (cond) { 	                                	 \
	        fprintf(stderr, "%s(%d): ", __FILE__, __LINE__); \
        	        fprintf(stderr, ##msg);                  \
                	exit(1);                                 \
        }                                                	 \
} while(0)


int main(int argc, char **argv) {
	const char *infile, *outfile, *lang;
	void *buffer;
	struct stat s;
	int x, y, ifd;

	FAILIF(argc != 6, 
		"tesstest infile xres yres outfile lang\n"); 

	infile = argv[1];
	FAILIF(sscanf(argv[2], "%d", &x) != 1, "could not parse x!\n");
	FAILIF(sscanf(argv[3], "%d", &y) != 1, "could not parse y!\n");
	outfile = argv[4];
	lang = argv[5];

	printf("input file %s\n", infile);
	ifd = open(infile, O_RDONLY);
	FAILIF(ifd < 0, "open(%s): %s\n", infile, strerror(errno));
	FAILIF(fstat(ifd, &s) < 0, "fstat(%d): %s\n", ifd, strerror(errno));
	printf("file size %lld\n", s.st_size);
	buffer = mmap(NULL, s.st_size * 2 / 3, PROT_READ, MAP_PRIVATE, ifd, 0);
	FAILIF(buffer == MAP_FAILED, "mmap(): %s\n", strerror(errno));
	printf("infile mmapped at %p\n", buffer);

	tesseract::TessBaseAPI  api;

	printf("lang %s\n", lang);
	FAILIF(api.Init("/sdcard/", lang), "could not initialize tesseract\n");
#if 0
	FAILIF(false == api.ReadConfigFile("/sdcard/tessdata/ratings"),
		"could not read config file\n");
#endif

	printf("set image x=%d, y=%d\n", x, y);
	api.SetImage((const unsigned char *)buffer, x, y, 1, x); 
	printf("recognize\n");
	char * text = api.GetUTF8Text();
	FAILIF(text == NULL, "didn't recognize\n");

	printf("write to output %s\n", outfile);
	FILE* fp = fopen(outfile, "w");
	if (fp != NULL) {
		fwrite(text, strlen(text), 1, fp);
		fclose(fp);
	}

	//delete [] text;
	return 0;
}
