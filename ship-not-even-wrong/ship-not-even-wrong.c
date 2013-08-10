#include <stdio.h>
#include <stdlib.h>

#include "opencscad.h"

static int randn(int n)
{
	return (int) ((float) n * (float) rand() / (float) RAND_MAX);
}

static float randf(float f)
{
	return ((float) rand() / (float) RAND_MAX) * f;
}

static void cylinder_rings(float length, float r1, float r2, int nrings)
{
	int i;
	float ringspacing = length / (nrings + 1);
	float ringheight = ringspacing / 3.0;
	float z, dz, r, dr;

	z = -(ringspacing * nrings) / 2;
	dz = ringspacing;
	r = r1 * 1.3;
	dr = ((r2 * 1.3) - r) / nrings;

	for (i = 0; i < nrings; i++) {
		xlate(0, 0, z);
			cyl(ringheight, r, r, 1);
			z += dz;
			r += dr; 
		end();
	}
}

float maxf(float a, float b)
{
	return a > b ? a : b;
}

float minf(float a, float b)
{
	return a > b ? a : b;
}

static void cylinder_ribs(float length, float r1, float r2, int nribs)
{
	static int rib_fn_number = 0;
	int i;
	char ribfn[30], ribsetfn[30];
	float dangle = 360.0 / nribs;
	float angle = 0.0;

	sprintf(ribfn, "ribfn_%d", rib_fn_number++);
	sprintf(ribsetfn, "ribsetfn_%d", rib_fn_number++);
	module(ribfn);
		diff();
			cyl(length, r1, r2, 1);
			xlate(maxf(r1, r2) * 1.05, 0, 0);
				cube(maxf(r1, r2) * 2, maxf(r1, r2) * 2, length + 2, 1);
			end();
			xlate(maxf(r1, r2) * -1.05, 0, 0);
				cube(maxf(r1, r2) * 2, maxf(r1, r2) * 2, length + 2, 1);
			end();
			xlate(0, maxf(r1, r2), 0);
				cube(maxf(r1, r2) * 2.1, maxf(r1, r2) * 2.1, length + 2, 1);
			end();
		end();
	end_module();

	module(ribsetfn);
		onion();
		for (i = 0; i < nribs; i++) {
			rotate(angle, 0, 0, 1); 
				call_module(ribfn);
			end();
			angle += dangle;
		}
		end();
	end_module();
	call_module(ribsetfn);
}

static void cylindrical_thing(float length, float r1, float r2)
{
	onion();
		cyl(length, r1, r2, 1);
		cylinder_rings(length, r1, r2, randn(3) + 5);
	endonion();
}

static void cylindrical_module(char *modulename, float length, float r1, float r2)
{
	module(modulename);
		cylindrical_thing(length, r1, r2);
	end_module();
}

static void cylinder_protrusions(float length, float r1, float r2, int nprotrusions)
{
	int i;
	float h, z, x;
	float angle;

	intersection();
		cyl(length, r1 * 1.1, r2 * 1.1, 1);
		onion();
			for (i = 0; i < nprotrusions; i++) {
				h = ((float) (randn(80) + 20) / 100.0) * length;
				z = -((length - h) / 2.0) * ((float) randn(100) / 100.0);
				x = minf(r1, r2) * randf(0.2); 
				angle = randf(360.0);
				xlate(0, 0, z);
					rotate(angle, 0, 0, 1);
						cube(x, maxf(r1, r2) * 3, h, 1);
					end();
				end();
			}
		end();
	end();
}
	
int main(int argc, char *argv[])
{
	opencscad_init();
#if 0
	cylindrical_module("testthing", 10, 5, 3);
	call_module("testthing");
#endif
	radial_dist(10, 30.0);
		cyl(10, 5, 3, 1);
		cylinder_ribs(10, 5.3, 3.3, 15);
		cylinder_protrusions(10, 5, 3, 9);
	end_radial_dist();
	scadinline("$fn=32;\n");
	finalize();
	return 0;
}

