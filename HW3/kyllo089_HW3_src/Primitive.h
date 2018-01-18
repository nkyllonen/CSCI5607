#ifndef PRIMITIVE_INCLUDED
#define PRIMITIVE_INCLUDED

#include "Vec3D.h"
#include "Material.h"
#include "Ray.h"
#include "Intersection.h"
#include "Util.h"
#include <algorithm> //for swap

enum prim_type
{
	SPHERE_PRIM,
	TRIANGLE_PRIM,
	PLANE_PRIM,
	NORM_TRIANGLE_PRIM
};

class Primitive
{
public:
	//CONSTRUCTORS AND DESTRUCTORS
	Primitive();
	~Primitive();

	//SETTERS
	void setMaterial(Material m);

	//GETTERS
	Material getMaterial();

	//VIRTUAL
	virtual bool findIntersectionPoint(Ray r, Intersection& iSect);
	virtual int getType();

protected:
	Material mat;
	const int kEpsilon = 1e-6; //arbitrary small int for plane intersection

};

#endif