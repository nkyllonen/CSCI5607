#ifndef LIGHT_INCLUDED
#define LIGHT_INCLUDED

#include "Vec3D.h"
#include "Pixel.h"
#include "Intersection.h"
#include "Material.h"
#include "Util.h"

enum light_type
{
	DIR_LIGHT,
	POINT_LIGHT,
	SPOT_LIGHT,
	AMB_LIGHT
};

class Light
{
public:
	//CONSTRUCTORS AND DESTRUCTORS
	Light();
	Light(Pixel c);
	~Light();

	//SETTERS
	void setColor(Pixel c);

	//GETTERS
	Pixel getColor();

	//VIRTUAL
	virtual Vec3D calcLighting(Intersection iSect);
	virtual int getType();
	virtual Vec3D getToLightVec(Vec3D from);

protected:
	Pixel color;

	//LIGHTING COMPONENTS
	Vec3D calcDiffuse(Intersection iSect, Material mat, Vec3D I_new, Vec3D l_VEC, Vec3D n_VEC);
	Vec3D calcSpecular(Intersection iSect, Material mat, Vec3D I_new, Vec3D l_VEC, Vec3D n_VEC);
};

#endif