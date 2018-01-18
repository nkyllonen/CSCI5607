#include "Intersection.h"


/*----------------------------*/
// CONSTRUCTORS AND DESTRUCTORS
/*----------------------------*/
Intersection::Intersection()
{
	mat = Material(); //default to black
	didIntersect = false;
}

Intersection::Intersection(Pixel bg)
{
	pt_COLOR = bg;
	didIntersect = false;
}

Intersection::~Intersection()
{
}

void Intersection::setColor(Pixel p)
{
	pt_COLOR = p;
}

/*----------------------------*/
// SETTERS
/*----------------------------*/
void Intersection::setMaterial(Material m)
{
	mat = m;
}

void Intersection::setPoint(Vec3D v)
{
	point = v;
}

void Intersection::setNormal(Vec3D n)
{
	surf_NORMAL = n;
}

void Intersection::setToCamVec(Vec3D v)
{
	pt_to_cam_VEC = v;
}

void Intersection::setT(float t)
{
	t_val = t;
}

void Intersection::setIfIntersected(bool i)
{
	didIntersect = i;
}

void Intersection::setSceneObjIndex(int i)
{
	scene_obj_index = i;
}

void Intersection::setIncomingRay(Ray r)
{
	in_RAY = r;
}

Pixel Intersection::getColor()
{
	return pt_COLOR;
}

/*----------------------------*/
// GETTERS
/*----------------------------*/
Material Intersection::getMaterial()
{
	return mat;
}

Vec3D Intersection::getPoint()
{
	return point;
}

Vec3D Intersection::getNormal()
{
	return surf_NORMAL;
}

Vec3D Intersection::getToCamVec()
{
	return pt_to_cam_VEC;
}

float Intersection::getT()
{
	return t_val;
}

float Intersection::getMinT()
{
	return minT;
}

bool Intersection::getIfIntersected()
{
	return didIntersect;
}

int Intersection::getSceneObjIndex()
{
	return scene_obj_index;
}

Ray Intersection::getIncomingRay()
{
	return in_RAY;
}
