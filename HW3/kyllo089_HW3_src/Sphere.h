#ifndef SPHERE_INCLUDED
#define SPHERE_INCLUDED

#include "Primitive.h"

class Sphere : public Primitive
{
public:
	//CONSTRUCTORS AND DESTRUCTORS
	Sphere();
	Sphere(Vec3D o, float r);
	~Sphere();

	//SETTERS
	void setOrigin(Vec3D v);
	void setRadius(float r);

	//GETTERS
	Vec3D getOrigin();
	float getRadius();

	//VIRTUAL (from Primitive)
	bool findIntersectionPoint(Ray r, Intersection& iSect);
	int getType();

private:
	Vec3D origin;
	float radius;

};

#endif