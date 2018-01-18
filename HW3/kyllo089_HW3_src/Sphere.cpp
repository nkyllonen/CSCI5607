#include "Sphere.h"
#include <iostream>

using namespace std;

/*----------------------------*/
// CONSTRUCTORS AND DESTRUCTORS
/*----------------------------*/
Sphere::Sphere()
{
	origin = Vec3D(0, 0, 0);
	radius = 1;
}

Sphere::Sphere(Vec3D o, float r)
{
	origin = o;
	radius = r;
}

Sphere::~Sphere()
{
}

/*----------------------------*/
// SETTERS
/*----------------------------*/
void Sphere::setOrigin(Vec3D v)
{
	origin = v;
}

void Sphere::setRadius(float r)
{
	radius = r;
}

/*----------------------------*/
// GETTERS
/*----------------------------*/
Vec3D Sphere::getOrigin()
{
	return origin;
}

float Sphere::getRadius()
{
	return radius;
}

/*----------------------------*/
// VIRTUAL FROM PRIMITIVE
/*----------------------------*/

//find T corresponding to intersection --> store in iSect
bool Sphere::findIntersectionPoint(Ray r, Intersection& iSect)
{
	//algorithm below is a modification of the one described by:
	//https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection
	
	Vec3D ray_dir = r.getDir();
	Vec3D ray_o = r.getOrigin();

	Vec3D dist = origin - ray_o;
	
	float L_o_mag = dotProduct(dist, ray_dir);
	float d_mag_sq = dist.getMagnitude()*dist.getMagnitude() - L_o_mag*L_o_mag;

	//simple out of bounds check of perpendicular distance d against radius
	if (d_mag_sq > radius*radius)
	{
		iSect.setIfIntersected(false);
		//cout << "Did not intersect" << endl;
		return false;
	}

	//find t value(s) by solving quadratic
	float a = 1;
	float b = dotProduct(2 * ray_dir, (ray_o - origin));
	
	Vec3D to_eye = ray_o - origin;
	float c = dotProduct(to_eye, to_eye) - radius*radius;
	float det = b*b - 4 * a*c;
	float t0 = 0, t1 = 0;

	if (!util::solveQuadratic(a, b, c, t0, t1)) //if unable to be solved quadratically
	{
		iSect.setIfIntersected(false);
		//cout << "Did not intersect" << endl;
		return false;
	}

	if (t0 > t1)
	{
		float temp = t0;
		t0 = t1;
		t1 = temp;
	}

	if (t0 < 0)
	{
		t0 = t1; //don't use t0 if it's negative
		if (t0 < 0)
		{
			iSect.setIfIntersected(false);
			//cout << "Did not intersect" << endl;
			return false; //both are negative
		}
	}

	Vec3D pt = ray_o + t0*ray_dir;
	Vec3D n = pt - origin;
	n.normalize();

	iSect.setT(t0);
	iSect.setPoint(pt);
	iSect.setNormal(n);
	
	//determine color if intersected
	Pixel color;
	color = mat.getAmbient();
	iSect.setColor(color);
	//printf("Intersection Color set to : %i  %i  %i\n", color.r, color.g, color.b);
	iSect.setIfIntersected(true);
	return true;
}

int Sphere::getType()
{
	return SPHERE_PRIM;
}
