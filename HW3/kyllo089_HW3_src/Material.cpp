#include "Material.h"


/*----------------------------*/
// CONSTRUCTORS AND DESTRUCTORS
/*----------------------------*/
Material::Material()
{
	ambient_COLOR = Pixel(0, 0, 0);
	diffuse_COLOR = Pixel(255, 255, 255);
	specular_COLOR = Pixel(0, 0, 0);
	phong_cosine_power = 5.0;
	index_of_refraction = 1.0;
	transmissive_COLOR = Pixel();
}


Material::~Material()
{
}

/*----------------------------*/
// SETTERS
/*----------------------------*/
void Material::setAmbient(Pixel c)
{
	ambient_COLOR = c;
}

void Material::setDiffuse(Pixel c)
{
	diffuse_COLOR = c;
}

void Material::setSpecular(Pixel c)
{
	specular_COLOR = c;
}

void Material::setNS(float n)
{
	phong_cosine_power = n;
}

void Material::setIOR(float i)
{
	index_of_refraction = i;
}

void Material::setTransmissive(Pixel c)
{
	transmissive_COLOR = c;
}

/*----------------------------*/
// GETTERS
/*----------------------------*/
Pixel Material::getAmbient()
{
	return ambient_COLOR;
}

Pixel Material::getDiffuse()
{
	return diffuse_COLOR;
}

Pixel Material::getSpecular()
{
	return specular_COLOR;
}

float Material::getNS()
{
	return phong_cosine_power;
}

float Material::getIOR()
{
	return index_of_refraction;
}

Pixel Material::getTransmissive()
{
	return transmissive_COLOR;
}
