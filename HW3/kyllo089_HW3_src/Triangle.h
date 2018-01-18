#ifndef TRIANGLE_INCLUDED
#define TRIANGLE_INCLUDED

#include "Primitive.h"
#include "Plane.h"

class Triangle : public Primitive
{
public:
	//CONSTRUCTORS AND DESTRUCTORS
	Triangle();
	Triangle(Vec3D a, Vec3D b, Vec3D c);
	~Triangle();

	//SETTERS
	void setVertices(Vec3D a, Vec3D b, Vec3D c);

	//GETTERS
	Vec3D getVertA();
	Vec3D getVertB();
	Vec3D getVertC();

	//VIRTUAL (from Primitive)
	bool findIntersectionPoint(Ray r, Intersection& iSect);
	int getType();

protected:
	//VERTICES (CCW winding)
	Vec3D vert_A;
	Vec3D vert_B;
	Vec3D vert_C;

	//SIDES (normalized)
	Vec3D AtoB;
	Vec3D BtoC;
	Vec3D CtoA;

	//SIDE LENGTHS (stored for faster calculation)
	float AB_LEN;
	float BC_LEN;
	float CA_LEN;

	//VERTEX NORMALS (normalized)
	Vec3D norm_A;
	Vec3D norm_B;
	Vec3D norm_C;

};

#endif