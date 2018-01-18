#include "Dir_Light.h"


/*----------------------------*/
// CONSTRUCTORS AND DESTRUCTORS
/*----------------------------*/
Dir_Light::Dir_Light()
{
}

Dir_Light::Dir_Light(Pixel c, Vec3D d)
{
	color = c;
	dir_VEC = d;
}


Dir_Light::~Dir_Light()
{
}

/*----------------------------*/
// SETTERS
/*----------------------------*/
void Dir_Light::setDir(Vec3D d)
{
	dir_VEC = d;
}

/*----------------------------*/
// GETTERS
/*----------------------------*/
Vec3D Dir_Light::getDir()
{
	return Vec3D();
}

/*----------------------------*/
// VIRTUAL FROM LIGHT
/*----------------------------*/
Vec3D Dir_Light::calcLighting(Intersection iSect)
{
	using namespace util;

	Vec3D colorV;

	Material mat = iSect.getMaterial();
	pixel_to_vec(color, colorV);

	Vec3D l_VEC = getToLightVec(iSect.getPoint());
	Vec3D n_VEC = iSect.getNormal();
	l_VEC.normalize();

	return calcDiffuse(iSect, mat, colorV, l_VEC, n_VEC)
		+ calcSpecular(iSect, mat, colorV, l_VEC, n_VEC);
}

int Dir_Light::getType()
{
	return DIR_LIGHT;
}

Vec3D Dir_Light::getToLightVec(Vec3D from)
{
	Vec3D result = (-1 * dir_VEC); //point back the way the light came from
	result.normalize();

	return result;
}
