#ifndef SPOT_LIGHT_INCLUDED
#define SPOT_LIGHT_INCLUDED

#include "Light.h"

class Spot_Light :	public Light
{
public:
	//CONSTRUCTORS AND DESTRUCTORS
	Spot_Light();
	~Spot_Light();

	//SETTERS
	void setPos(Vec3D p);
	void setDir(Vec3D d);
	void setMinAngle(float a);
	void setMaxAngle(float a);

	//GETTERS
	Vec3D getPos();
	Vec3D getDir();
	float getMinAngle();
	float getMaxAngle();

	//VIRTUAL
	Vec3D calcLighting(Intersection iSect);
	int getType();
	Vec3D getToLightVec(Vec3D from);

private:
	Vec3D pos_VEC;
	Vec3D dir_VEC;
	float min_ANGLE;
	float max_ANGLE;

};

#endif