#include "Plane.h"


/*----------------------------*/
// CONSTRUCTORS AND DESTRUCTORS
/*----------------------------*/
Plane::Plane()
{
	point = Vec3D(0, 0, 0);
	normal = Vec3D(0, 0, 1);
}

Plane::Plane(Vec3D p, Vec3D n)
{
	point = p;
	normal = n;
}

Plane::~Plane()
{
}

/*----------------------------*/
// SETTERS
/*----------------------------*/
void Plane::setPoint(Vec3D p)
{
	point = p;
}

void Plane::setNorm(Vec3D n)
{
	normal = n;
}

/*----------------------------*/
// GETTERS
/*----------------------------*/
Vec3D Plane::getPoint()
{
	return point;
}

Vec3D Plane::getNorm()
{
	return normal;
}

/*----------------------------*/
// VIRTUAL FROM PRIMITIVE
/*----------------------------*/
bool Plane::findIntersectionPoint(Ray r, Intersection& iSect)
{
	//algorithm below is a modification of the one described by:
	//https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-plane-and-ray-disk-intersection

	Vec3D ray_dir = r.getDir();
	Vec3D ray_o = r.getOrigin();

	float denom = dotProduct(ray_dir, normal);

	if (denom > kEpsilon) //save time; don't worry about really small numbers
	{
		Vec3D dist = point - ray_o;
		float t = dotProduct(dist, normal) / denom;
		
		bool result = (t >= 0);

		if (result)
		{
			//store intersection point (need this for Triangle)
			iSect.setPoint(ray_o + t*ray_dir);

			//store T value
			iSect.setT(t);

			//set normal
			iSect.setNormal(normal);

			//determine color if intersected
			Pixel color;
			color = mat.getAmbient();
			iSect.setColor(color);
		}
		iSect.setIfIntersected(result);
		return result;
	}

	iSect.setIfIntersected(false);
	return false;
}

int Plane::getType()
{
	return PLANE_PRIM;
}
