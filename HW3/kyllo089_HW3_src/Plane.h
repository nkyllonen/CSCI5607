#ifndef PLANE_INCLUDED
#define PLANE_INCLUDED

#include "Primitive.h"

class Plane : public Primitive
{
public:
	//CONSTRUCTORS AND DESTRUCTORS
	Plane();
	Plane(Vec3D p, Vec3D n);
	~Plane();

	//SETTERS
	void setPoint(Vec3D p);
	void setNorm(Vec3D n);

	//GETTERS
	Vec3D getPoint();
	Vec3D getNorm();

	//VIRTUAL (from Primitive)
	bool findIntersectionPoint(Ray r, Intersection& iSect);
	int getType();

private:
	Vec3D point;
	Vec3D normal;

};

#endif