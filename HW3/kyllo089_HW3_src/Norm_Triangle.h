#pragma once
#include "Triangle.h"
class Norm_Triangle :
	public Triangle
{
public:
	Norm_Triangle();
	Norm_Triangle(Vec3D a, Vec3D b, Vec3D c, Vec3D nA, Vec3D nB, Vec3D nC);
	~Norm_Triangle();

	//VIRTUAL (from Primitive)
	bool findIntersectionPoint(Ray r, Intersection& iSect);
	int getType();

private:
	Vec3D d_normA;
	Vec3D d_normB;
	Vec3D d_normC;

};

