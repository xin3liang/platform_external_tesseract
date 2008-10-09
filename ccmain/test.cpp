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
#include "varable.h"
#include "tessvars.h"

#define FAILIF(cond, msg...) do {                        	 \
        if (cond) { 	                                	 \
	        fprintf(stderr, "%s(%d): ", __FILE__, __LINE__); \
        	        fprintf(stderr, ##msg);                  \
                	exit(1);                                 \
        }                                                	 \
} while(0)

// This is to make the ratings file parser happy.
BOOL_VAR (tessedit_write_images, FALSE,
                 "Capture the image from the IPE");

int main(int argc, char **argv) {
	const char *tessdata, *infile, *outfile, *lang, *ratings;
	void *buffer;
	struct stat s;
	int x, y, bpp, ifd;

	FAILIF(argc < 8 || argc > 9,
		"tesstest infile xres yres bpp outfile lang tessdata [ratings]\n"); 

	infile = argv[1];
	FAILIF(sscanf(argv[2], "%d", &x) != 1, "could not parse x!\n");
	FAILIF(sscanf(argv[3], "%d", &y) != 1, "could not parse y!\n");
	FAILIF(sscanf(argv[4], "%d", &bpp) != 1, "could not parse bpp!\n");
	outfile = argv[5];
	lang = argv[6];
	tessdata = argv[7];
	ratings = argv[8];

	printf("input file %s\n", infile);
	ifd = open(infile, O_RDONLY);
	FAILIF(ifd < 0, "open(%s): %s\n", infile, strerror(errno));
	FAILIF(fstat(ifd, &s) < 0, "fstat(%d): %s\n", ifd, strerror(errno));
	printf("file size %lld\n", s.st_size);
	buffer = mmap(NULL, s.st_size, PROT_READ, MAP_PRIVATE, ifd, 0);
	FAILIF(buffer == MAP_FAILED, "mmap(): %s\n", strerror(errno));
	printf("infile mmapped at %p\n", buffer);
	FAILIF(!tessdata, "You must specify a path for tessdata.\n");

	tesseract::TessBaseAPI  api;

	printf("tessdata %s\n", tessdata);
	printf("lang %s\n", lang);
	FAILIF(api.Init(tessdata, lang), "could not initialize tesseract\n");
	if (ratings) {
		printf("ratings %s\n", ratings);
		FAILIF(false == api.ReadConfigFile(ratings),
			"could not read config file\n");
	}

	printf("set image x=%d, y=%d bpp=%d\n", x, y, bpp);
	FAILIF(!bpp || bpp == 2 || bpp > 4, 
		"Invalid value %d of bpp\n", bpp);
	api.SetImage((const unsigned char *)buffer, x, y, bpp, bpp*x); 

	printf("set rectangle to cover entire image\n");
	api.SetRectangle(0, 0, x, y);

	printf("set page seg mode to single character\n");
	//api.SetPageSegMode(tesseract::PSM_AUTO);
	//api.SetPageSegMode(tesseract::PSM_SINGLE_CHAR);
	printf("recognize\n");
	char * text = api.GetUTF8Text();
	if (tessedit_write_images) {
		page_image.write("tessinput.tif");
	}
	FAILIF(text == NULL, "didn't recognize\n");

	printf("write to output %s\n", outfile);
	FILE* fp = fopen(outfile, "w");
	if (fp != NULL) {
		fwrite(text, strlen(text), 1, fp);
		fclose(fp);
	}

	int mean_confidence = api.MeanTextConf();
	printf("mean confidence: %d\n", mean_confidence);

	int* confs = api.AllWordConfidences();
	int len, *trav;
	for (len = 0, trav = confs; *trav != -1; trav++, len++)
		printf("confidence %d: %d\n", len, *trav);
	free(confs);

	printf("clearing api\n");
	api.Clear();
	printf("clearing adaptive classifier\n");
	api.ClearAdaptiveClassifier();

	//delete [] text;
	return 0;
}
