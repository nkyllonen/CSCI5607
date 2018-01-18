#ifndef RAY_INCLUDED
#define RAY_INCLUDED

#include "Vec3D.h"

class Ray
{
public:
	//CONSTRUCTORS AND DESTRUCTORS
	Ray();
	Ray(Vec3D o, Vec3D d);
	~Ray();

	//SETTERS
	void setOrigin(Vec3D o);
	void setDir(Vec3D d);
	void setT(int t);

	//GETTERS
	Vec3D getOrigin();
	Vec3D getDir();
	int getCurrentT();

	//OTHERS
	void resetT();
	Vec3D evaluate(float t);
	void print();

private:
	Vec3D origin;
	Vec3D dir_VEC;
	int currentT;

};

#endif