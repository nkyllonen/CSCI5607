#ifndef UTIL_INCLUDED
#define UTIL_INCLUDED

#include <cmath>
#include <algorithm>

#include "Pixel.h"
#include "Vec3D.h"

namespace util
{

	//following function is copied from:
	//https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection
	bool solveQuadratic(const float &a, const float &b, const float &c, float &x0, float &x1);

	//inputs Pixel values into Vec3D
	void pixel_to_vec(Pixel p, Vec3D& v);

	void vec_to_pixel(Vec3D v, Pixel& p);
}

#endif