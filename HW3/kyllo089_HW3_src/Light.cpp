#include "Light.h"
#include <iostream>
#include <algorithm>

/*----------------------------*/
// CONSTRUCTORS AND DESTRUCTORS
/*----------------------------*/
Light::Light()
{
	color = Pixel(0, 0, 0);
}

Light::Light(Pixel c)
{
	color = c;
}

Light::~Light()
{
}

/*----------------------------*/
// SETTERS
/*----------------------------*/
void Light::setColor(Pixel c)
{
	color = c;
}

/*----------------------------*/
// GETTERS
/*----------------------------*/
Pixel Light::getColor()
{
	return color;
}

/*----------------------------*/
// VIRTUAL
/*----------------------------*/
Vec3D Light::calcLighting(Intersection iSect) //for ambient lighting!
{
	Pixel amb_val = iSect.getMaterial().getAmbient();
	Vec3D ambV;
	util::pixel_to_vec(amb_val, ambV);

	//convert light's color into percentages of the [0,255] range
	//component-wise mult of amb RGB vals and light's color
	float r = (color.r / 255.0)*ambV.getX();
	float g = (color.g / 255.0)*ambV.getY();
	float b = (color.b / 255.0)*ambV.getZ();

	return Vec3D(r, g, b);
}

int Light::getType()
{
	return AMB_LIGHT;
}

Vec3D Light::getToLightVec(Vec3D from)
{
	return Vec3D(); //ambient light has no light position
}

/*----------------------------*/
// LIGHTING COMPONENTS
/*----------------------------*/
Vec3D Light::calcDiffuse(Intersection iSect, Material mat, Vec3D I_new, Vec3D l_VEC, Vec3D n_VEC)
{
	using namespace util;

	Vec3D diffuseV;

	Pixel diff_val = mat.getDiffuse();
	pixel_to_vec(diff_val, diffuseV);
	
	float max_d = std::max(0.f, dotProduct(n_VEC, l_VEC));
	return max_d * (diffuseV * I_new);;
}

Vec3D Light::calcSpecular(Intersection iSect, Material mat, Vec3D I_new, Vec3D l_VEC, Vec3D n_VEC)
{
	using namespace util;

	Vec3D specularV;

	float cos_pow = mat.getNS();
	Pixel spec_val = mat.getSpecular();
	pixel_to_vec(spec_val, specularV);

	Vec3D v_VEC = iSect.getToCamVec();
	Vec3D half_VEC = (v_VEC + l_VEC);
	half_VEC.normalize();

	float max_p = std::max(0.f, dotProduct(n_VEC, half_VEC));

	return pow(max_p, cos_pow) * (specularV*I_new);
}