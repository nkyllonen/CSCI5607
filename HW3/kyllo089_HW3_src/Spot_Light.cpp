#include "Spot_Light.h"
#include <cmath>

#define _USE_MATH_DEFINES
#include <math.h>

/*----------------------------*/
// CONSTRUCTORS AND DESTRUCTORS
/*----------------------------*/
Spot_Light::Spot_Light()
{
	pos_VEC = Vec3D(0, 0, 0);
	dir_VEC = Vec3D(0, -1, 0);
	min_ANGLE = M_PI / 4.0;
	max_ANGLE = M_PI / 2.0;
}

Spot_Light::~Spot_Light()
{
}

/*----------------------------*/
// SETTERS
/*----------------------------*/
void Spot_Light::setDir(Vec3D d)
{
	d.normalize();
	dir_VEC = d;
}

void Spot_Light::setPos(Vec3D d)
{
	pos_VEC = d;
}

void Spot_Light::setMinAngle(float a)
{
	min_ANGLE = a * M_PI / 180.0;		//convert to radians
}

void Spot_Light::setMaxAngle(float a)
{
	max_ANGLE = a* M_PI / 180.0;		//convert to radians
}

/*----------------------------*/
// GETTERS
/*----------------------------*/
Vec3D Spot_Light::getDir()
{
	return Vec3D();
}

Vec3D Spot_Light::getPos()
{
	return pos_VEC;
}

float Spot_Light::getMinAngle()
{
	return min_ANGLE;
}

float Spot_Light::getMaxAngle()
{
	return max_ANGLE;
}

/*----------------------------*/
// VIRTUAL FROM LIGHT
/*----------------------------*/
Vec3D Spot_Light::calcLighting(Intersection iSect)
{
	using namespace util;

	float half_min = min_ANGLE / 2.0;
	float half_max = max_ANGLE / 2.0;

	Vec3D l_VEC = pos_VEC - iSect.getPoint();
	float d = l_VEC.getMagnitude(); //get mag BEFORE normalizing to get distance
	Vec3D n_VEC = iSect.getNormal();

	l_VEC.normalize();

	//1. check angle between l and dir
	float between_ANGLE = acos(dotProduct(-1*l_VEC, dir_VEC));

	if (between_ANGLE > half_max) return Vec3D(0, 0, 0);	//outside of cone

	//2.1 calc as if a point light
	Vec3D colorV;

	Material mat = iSect.getMaterial();
	pixel_to_vec(color, colorV);

	Vec3D I_new = (1.0 / (d*d)) * colorV;

	Vec3D pt_result = calcDiffuse(iSect, mat, I_new, l_VEC, n_VEC)
		+ calcSpecular(iSect, mat, I_new, l_VEC, n_VEC);

	//2.2 clamp values before interpolating
	Pixel clamped;
	vec_to_pixel(pt_result, clamped);
	pixel_to_vec(clamped, pt_result);

	//printf("\nclamped pt_result: ");
	//pt_result.print();
	//3.1 if less than min --> point light
	if (between_ANGLE < half_min) return pt_result;
	//printf("between_ANGLE : %f\n", between_ANGLE * 180.0 / M_PI);

	//3.2 linear interp based on between_ANGLE
	float interp = (half_max - between_ANGLE) / (half_max - half_min);
	//printf("interp : %f\n", interp);
	
	Vec3D result = interp*pt_result;
	//result.print();
	return result;
}

int Spot_Light::getType()
{
	return SPOT_LIGHT;
}

Vec3D Spot_Light::getToLightVec(Vec3D from)
{
	return (pos_VEC - from);
}
