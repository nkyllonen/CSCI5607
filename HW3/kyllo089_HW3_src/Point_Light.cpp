#include "Point_Light.h"
#include <algorithm>


/*----------------------------*/
// CONSTRUCTORS AND DESTRUCTORS
/*----------------------------*/
Point_Light::Point_Light()
{
}

Point_Light::Point_Light(Pixel c, Vec3D p)
{
	color = c;
	pos_VEC = p;
}


Point_Light::~Point_Light()
{
}

/*----------------------------*/
// SETTERS
/*----------------------------*/
void Point_Light::setPos(Vec3D d)
{
	pos_VEC = d;
}

/*----------------------------*/
// GETTERS
/*----------------------------*/
Vec3D Point_Light::getPos()
{
	return pos_VEC;
}

/*----------------------------*/
// VIRTUAL FROM LIGHT
/*----------------------------*/
Vec3D Point_Light::calcLighting(Intersection iSect)
{
	using namespace util;

	Vec3D colorV;

	Material mat = iSect.getMaterial();
	pixel_to_vec(color, colorV);

	Vec3D l_VEC = pos_VEC - iSect.getPoint();
	Vec3D n_VEC = iSect.getNormal();

	//calculate attentuation
	float d = l_VEC.getMagnitude(); //get mag BEFORE normalizing to get distance
	Vec3D I_new = (1.0 / (d*d)) * colorV;
	
	l_VEC.normalize();

	return calcDiffuse(iSect, mat, I_new, l_VEC, n_VEC)
		+ calcSpecular(iSect, mat, I_new, l_VEC, n_VEC);
}

int Point_Light::getType()
{
	return POINT_LIGHT;
}

Vec3D Point_Light::getToLightVec(Vec3D from)
{
	return (pos_VEC - from);
}
