#ifndef INTERSECTION_INCLUDED
#define INTERSECTION_INCLUDED

#include "Vec3D.h"
#include "Pixel.h"
#include "Material.h"
#include "Ray.h"

class Intersection
{
public:
	//CONSTRUCTORS AND DESTRUCTORS
	Intersection();
	Intersection(Pixel bg);
	~Intersection();

	//SETTERS
	void setColor(Pixel p);
	void setMaterial(Material m);
	void setPoint(Vec3D v);
	void setNormal(Vec3D n);
	void setToCamVec(Vec3D v);
	void setT(float t);
	void setIfIntersected(bool i);
	void setSceneObjIndex(int i);
	void setIncomingRay(Ray r);

	//GETTERS
	Pixel getColor();
	Material getMaterial();
	Vec3D getPoint();
	Vec3D getNormal();
	Vec3D getToCamVec();
	float getT();
	float getMinT();
	bool getIfIntersected();
	int getSceneObjIndex();
	Ray getIncomingRay();

private:
	Pixel pt_COLOR;
	Material mat;
	Vec3D point;
	Vec3D surf_NORMAL;
	Vec3D pt_to_cam_VEC;
	Ray in_RAY;

	float minT = 0.01f;
	float t_val;
	bool didIntersect;
	int scene_obj_index;

};

#endif