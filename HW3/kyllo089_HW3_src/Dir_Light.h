#ifndef DIR_LIGHT_INCLUDED
#define DIR_LIGHT_INCLUDED

#include "Light.h"
#include "Vec3D.h"

class Dir_Light : public Light
{
public:
	//CONSTRUCTORS AND DESTRUCTORS
	Dir_Light();
	Dir_Light(Pixel c, Vec3D d);
	~Dir_Light();

	//SETTERS
	void setDir(Vec3D d);

	//GETTERS
	Vec3D getDir();

	//VIRTUAL
	Vec3D calcLighting(Intersection iSect);
	int getType();
	Vec3D getToLightVec(Vec3D from);

private:
	Vec3D dir_VEC;

};

#endif