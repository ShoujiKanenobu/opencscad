#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <getopt.h>

#include "opencscad.h"

void finger(float length, float angle, int segments, float thickness, float taper, float curl)
{
	float len = length / (float) segments;
	float a = angle;
	float curve = 1.0;
	float diam = thickness;
	float x, y;
	int i;

	onion();

	x = 0;
	y = 0;
	for (i = 0; i < segments; i++) {
		xlate(x, y, 0);
		rotate(a, 0, 0, 1);
		rotate(90, 0, 1, 0);
		xlate(0, 0, 0);
		sphere(diam * 1.1);
		endxlate();
		xlate(0, 0, len);
		sphere(diam * 1.1);
		endxlate();
		cylinder(len * 1.05, diam, diam * taper);
		endrotate();
		endrotate();
		endxlate();
		x = x + cos(a * M_PI / 180.0) * len;
		y = y + sin(a * M_PI / 180.0) * len;
		a = a + curve;
		curve = curve * curl;
		diam = diam * taper;
	}
	endonion();
}

void fingers(int nfingers, float length, float deltalength, float angle, float deltaangle, int left,
		int segments, float thickness, float taper, float curl)
{
	int i;
	float armlength = 0.7 * length;

	if (!left)
		rotate(180, 1, 0, 0);

	diff();
	onion();
	for (i = 0; i < nfingers; i++) {
		finger(length, angle, segments, thickness, taper, curl);
		angle += deltaangle;
		length -= deltalength;
	}
	finger(armlength, angle, segments, thickness * 1.5, 2.0 - taper, curl);
	endonion();
	xlate(-length * 2.5, -length * 2.5, left * length * -5);
	cube(length * 5, length * 5, length * 5, 0);
	endxlate();
	enddiff();
	if (!left)
		endrotate();
}

static void usage(void)
{
	fprintf(stderr, "usage: batwing ...\n");
	exit(1);
}

static int parse_int_option(char *o, int *i)
{
	int rc;

	rc = sscanf(o, "%d", i);
	if (rc != 1)
		return 1;
	return 0;
}

static int parse_float_option(char *o, float *f)
{
	int rc;

	rc = sscanf(o, "%f", f);
	if (rc != 1)
		return 1;
	return 0;
}

static void process_options(int argc, char *argv[], float *length, int *segments, float *deltaangle,
		int *left, float *thickness, float *taper, float *curl)
{
	int i, c;
	int digit_optind = 0;
	int optind;
	float f;

	while (1) {
		int this_option_optind = optind ? optind : 1;
		int option_index = 0;
		static struct option long_options[] = {
			{"segments",	required_argument, 0, 's' },
			{"angle",	required_argument, 0,  'a' },
			{"curl",	required_argument, 0,  'c' },
			{"left",	required_argument, 0,  'l' },
			{"size",	required_argument, 0,  'z' },
			{"thickness",	required_argument, 0,  't' },
			{"taper",	required_argument, 0,  'T' },
			{0,         0,                 0,  0 }
		};

		c = getopt_long(argc, argv, "a:c:l:s:t:T:z:", long_options, &option_index);
		if (c == -1)
			break;

		switch (c) {
		case 'a':
			if (parse_float_option(optarg, &f))
				usage();
			*deltaangle = f;
			break;
		case 's':
			if (parse_int_option(optarg, &i))
				usage();
			*segments = i;
			break;
		case 'l':
			if (parse_int_option(optarg, &i))
				usage();
			*left = i;
			break;
		case 'T':
			if (parse_float_option(optarg, &f))
				usage();
			*taper = f;
			break;
		case 'z':
			if (parse_float_option(optarg, &f))
				usage();
			*length = f;
			break;
		case 't':
			if (parse_float_option(optarg, &f))
				usage();
			*thickness = f;
			break;
		case 'c':
			if (parse_float_option(optarg, &f))
				usage();
			*curl = f;
			break;
		default:
			usage();
			break;
		}
	}
}

int main(int argc, char *argv[])
{
	int segments = 10;
	float deltaangle = 30;
	float angle = 0;
	float curl = 1.3;
	float taper = 0.95;
	float length = 800;
	float deltalength = length / 8;
	float thickness = 8.0;
	int left = 0;

	process_options(argc, argv, &length, &segments, &deltaangle, &left, &thickness, &taper, &curl);

	opencscad_init();
	scadinline("$fn = 16;\n");
	fingers(4, length, deltalength, angle, deltaangle, left, segments,
			thickness, taper, curl);
	finalize();
	return 0;
}
