#ifndef MATERIAL_INCLUDED
#define MATERIAL_INCLUDED

#include "Pixel.h"

class Material
{
public:
	//CONSTRUCTORS AND DESTRUCTORS
	Material();
	~Material();

	//SETTERS
	void setAmbient(Pixel c);
	void setDiffuse(Pixel c);
	void setSpecular(Pixel c);
	void setNS(float n);
	void setIOR(float i);
	void setTransmissive(Pixel c);

	//GETTERS
	Pixel getAmbient();
	Pixel getDiffuse();
	Pixel getSpecular();
	float getNS();
	float getIOR();
	Pixel getTransmissive();

private:
	Pixel ambient_COLOR;
	Pixel diffuse_COLOR;
	Pixel specular_COLOR;
	float phong_cosine_power;
	float index_of_refraction;
	Pixel transmissive_COLOR;

};

#endif