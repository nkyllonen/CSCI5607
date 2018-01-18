#include "Primitive.h"


/*----------------------------*/
// CONSTRUCTORS AND DESTRUCTORS
/*----------------------------*/
Primitive::Primitive()
{
	mat = Material();
}

Primitive::~Primitive()
{
}

/*----------------------------*/
// SETTERS
/*----------------------------*/
void Primitive::setMaterial(Material m)
{
	mat = m;
}

/*----------------------------*/
// GETTERS
/*----------------------------*/
Material Primitive::getMaterial()
{
	return mat;
}

/*----------------------------*/
// VIRTUAL
/*----------------------------*/
bool Primitive::findIntersectionPoint(Ray r, Intersection& iSect)
{
	return false;
}

int Primitive::getType()
{
	return NULL;
}
