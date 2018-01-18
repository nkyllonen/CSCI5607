#include "Ray.h"

#include <iostream>

/*----------------------------*/
// CONSTRUCTORS AND DESTRUCTORS
/*----------------------------*/
Ray::Ray()
{
	origin = Vec3D(0, 0, 0);
	currentT = 1;

}

Ray::Ray(Vec3D o, Vec3D d)
{
	origin = o;
	d.normalize(); //just to make sure
	dir_VEC = d;
	currentT = 1;
}

Ray::~Ray()
{

}

/*----------------------------*/
// SETTERS
/*----------------------------*/
void Ray::setOrigin(Vec3D o)
{
	origin = o;
}

void Ray::setDir(Vec3D d)
{
	dir_VEC = d;
}

void Ray::setT(int t)
{
	currentT = t;
}

/*----------------------------*/
// GETTERS
/*----------------------------*/
Vec3D Ray::getOrigin()
{
	return origin;
}

Vec3D Ray::getDir()
{
	return dir_VEC;
}

int Ray::getCurrentT()
{
	return currentT;
}

/*----------------------------*/
// OTHERS
/*----------------------------*/
void Ray::resetT()
{
	currentT = 1;
}

Vec3D Ray::evaluate(float t)
{
	return origin + (t*dir_VEC);
}

void Ray::print()
{
	std::cout << "Origin: ";
	origin.print();
	std::cout << "Direction: ";
	dir_VEC.print();
	//printf("\nCurrent t: %i\n", currentT);
}