#ifndef PT_LIGHT_INCLUDED
#define PT_LIGHT_INCLUDED

#include "Light.h"

class Point_Light :	public Light
{
public:
	//CONSTRUCTORS AND DESTRUCTORS
	Point_Light();
	Point_Light(Pixel c, Vec3D p);
	~Point_Light();

	//SETTERS
	void setPos(Vec3D p);

	//GETTERS
	Vec3D getPos();

	//VIRTUAL
	Vec3D calcLighting(Intersection iSect);
	int getType();
	Vec3D getToLightVec(Vec3D from);

private:
	Vec3D pos_VEC;

};

#endif