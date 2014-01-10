#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <getopt.h>

#include "bitmap.h"

/* Largest bitmap we support is 4K bits */
enum {
	NBITS  = 4096,
	NLONGS = BITS_TO_LONGS(NBITS)
};

enum Formats {
	HEX,
	LIST,
	EACH,
	COUNT // Output only
};

enum Actions {
	NONE,
	WEIGHT,
	INVERT,
	AND,
	OR,
	XOR,
	ANDNOT
};

static int ifmt = HEX, ofmt = HEX;
static int action = NONE;

void parse_format(char *fmt)
{
	while (*fmt) {
		switch (*fmt++) {
		case 'x':
			ifmt = HEX;
			break;
		case 'X':
			ofmt = HEX;
			break;
		case 'l':
			ifmt = LIST;
			break;
		case 'L':
			ofmt = LIST;
			break;
		case 'E':
			ofmt = EACH;
			break;
		}
	}
}

unsigned long do_action(int action, unsigned long *imask, unsigned long *amask, unsigned long *omask)
{
	unsigned long c;

	switch (action) {
	case NONE:
		bitmap_copy(omask, imask, NBITS);
		break;

	case WEIGHT:
		return bitmap_weight(imask, NBITS);

	case AND:
		bitmap_and(omask, imask, amask, NBITS);
		break;	

	case OR:
		bitmap_or(omask, imask, amask, NBITS);
		break;	

	case XOR:
		bitmap_xor(omask, imask, amask, NBITS);
		break;

	case ANDNOT:
		bitmap_andnot(omask, imask, amask, NBITS);
		break;
	};

	c = find_last(omask, NBITS);

	return c == NBITS ? 1 : c + 1;
}

void parse_bitmap(int fmt, const char *str, unsigned long *mask, unsigned int nbits)
{
	int err;

	switch (ifmt) {
	case LIST:
		err = bitmap_parselist(str, mask, nbits);
		if (err) {
			fprintf(stderr, "Bad list format %s\n", str);
			exit(1);
		}
		break;

	case HEX:
		err = bitmap_parse(str, strlen(str), mask, nbits);
		if (err) {
			fprintf(stderr, "Bad mask format %s\n", str);
			exit(1);
		}
		break;
	};
}

static struct option main_lopts[] = {
        { "help",   0, 0, 'h' },
        { "format", 1, 0, 'f' },
        { "fmt",    1, 0, 'f' },
        { "weight", 0, 0, 'w' },
        { "invert", 0, 0, 'i' },
        { "and",    1, 0, 'a' },
        { "or",     1, 0, 'o' },
        { "xor",    1, 0, 'x' },
        { "andnot", 1, 0, 'A' },
        { 0, 0, 0, 0 }
};

static const char main_sopts[] = "hf:wia:o:x:A:";

static const char main_help[] = 
        "bitops reads bit masks from standard input and performs various\n"
        "operations on them. Result is written to standard out.\n"
        "Usage:\n"
        "\tbitops [options]\n"
	"\n"
        "Options:\n"
        "\t--format --fmt -f <STR>  format of input and output masks\n"
        "\t--weight -w         count number of non-zero bits\n"
        "\t--invert -i         invert bits in the input mask\n"
        "\t--and -a <MASK>     output = input & MASK\n"
        "\t--or  -o <MASK>     output = input | MASK"
        "\t--xor -x <MASK>     output = input ^ MASK\n"
        "\t--andnot -A <MASK>  output = input & ~MASK\n"
        "\t--help              show this help\n"
	"\n"
        "I/O format:\n"
        "\tx hex mask input format (default)\n"
        "\tX hex mask output format (default)\n"
        "\tl comma separated list input format with ranges\n"
        "\tL comma separated list output format with ranges\n"
        "\tE space separated output format w/o ranges\n"

	"\n";


int main(int argc, char **argv)
{
        int opt = 0;

	unsigned long amask[NLONGS], imask[NLONGS], omask[NLONGS];
	unsigned long count;
	char str[4096];

        while ((opt=getopt_long(argc, argv, main_sopts, main_lopts, NULL)) != -1) {
                switch(opt) {
            	case 'f':
			parse_format(optarg);
                        break;

                case 'w':
                        action = WEIGHT;
			ofmt   = COUNT;
                        break;

                case 'i':
 			action = INVERT;
 			break;

		case 'a':
			action = AND;
			parse_bitmap(ifmt, optarg, amask, NBITS);
			break;

		case 'o':
			action = OR;
			parse_bitmap(ifmt, optarg, amask, NBITS);
			break;

		case 'x':
			action = XOR;
			parse_bitmap(ifmt, optarg, amask, NBITS);
			break;

		case 'A':
			action = ANDNOT;
			parse_bitmap(ifmt, optarg, amask, NBITS);
			break;

                case 'h':
                default:
                        printf(main_help);
                        exit(0);
                }
        }

        argc -= optind;
        argv += optind;

        if (argc < 0) {
                printf(main_help);
                exit(1);
        }

	// Read from stdin
	while (1) {
		int n = scanf("%4096s", str);
		if (!n || n == EOF) 
			break;

		// Parse input
		parse_bitmap(ifmt, str, imask, NBITS);

		// Do action
		count = do_action(action, imask, amask, omask);

		// Print out result
		switch (ofmt) {
		case LIST:
			bitmap_scnlistprintf(str, sizeof(str), omask, count);
			break;

		case EACH:
			bitmap_scneachprintf(str, sizeof(str), omask, count);
			break;

		case HEX:
			bitmap_scnprintf(str, sizeof(str), omask, count);
			break;

		case COUNT:
			sprintf(str, "%lu", count);
			break;
		};
		printf("%s\n", str);
	}

	return 0;
}
