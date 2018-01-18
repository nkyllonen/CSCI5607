#include "Triangle.h"

/*----------------------------*/
// CONSTRUCTORS AND DESTRUCTORS
/*----------------------------*/
Triangle::Triangle()
{
}

//for regular triangles
Triangle::Triangle(Vec3D a, Vec3D b, Vec3D c)
{
	setVertices(a, b, c);
}

Triangle::~Triangle()
{
}

/*----------------------------*/
// SETTERS
/*----------------------------*/
void Triangle::setVertices(Vec3D a, Vec3D b, Vec3D c)
{
	vert_A = a;
	vert_B = b;
	vert_C = c;

	//calc new side vectors // don't normalize - need lengths preserved
	AtoB = b - a;
	BtoC = c - b;
	CtoA = a - c;

	//calc new side lengths
	AB_LEN = AtoB.getMagnitude();
	CA_LEN = CtoA.getMagnitude();
	BC_LEN = BtoC.getMagnitude();

	//normalize side vectors
	CtoA.normalize();
	AtoB.normalize();
	BtoC.normalize();

	//calc new normal vectors -- all the same since regular flat triangle
	norm_A = cross(AtoB, -1*BtoC); //initially assuming CCW with left-handing coord (-1)
	norm_A.normalize();
	norm_B = norm_A;
	norm_C = norm_A;
}

/*----------------------------*/
// GETTERS
/*----------------------------*/
Vec3D Triangle::getVertA()
{
	return vert_A;
}

Vec3D Triangle::getVertB()
{
	return vert_B;
}

Vec3D Triangle::getVertC()
{
	return vert_C;
}

/*----------------------------*/
// VIRTUALS FROM PRIMITIVE
/*----------------------------*/
bool Triangle::findIntersectionPoint(Ray r, Intersection & iSect)
{
	//algorithm below is a modification of the one described by:
	//https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/barycentric-coordinates

	Plane tPlane;
	tPlane.setPoint(vert_A);

	//check normals before constructing plane	
	if (dotProduct(norm_A, iSect.getToCamVec()) > 0)
	{
		tPlane.setNorm(-1*norm_A);
	}
	else
	{
		tPlane.setNorm(norm_A);
	}
	//construct plane containing triangle
	bool intersectedPlane = tPlane.findIntersectionPoint(r, iSect);

	if (intersectedPlane)
	{
		//printf("Intersected plane containing triangle.\n");
		float u, v;
		Vec3D int_pt = iSect.getPoint();

		//1. construct vector from vert A to pt P (contained in iSect)
		Vec3D AtoP = int_pt - vert_A;

		//2. inside-out test
		Vec3D perp_VEC;

		//2.1 AtoB edge
		perp_VEC = cross(AtoB, -1*AtoP);

		if (v = dotProduct(norm_A, perp_VEC) < 0) //this is v since v = ABP/ABC
		{
			iSect.setIfIntersected(false);
			return false;
		}

		//2.2 BtoC edge
		Vec3D BtoP = int_pt - vert_B;
		perp_VEC = cross(BtoC, -1*BtoP);

		if (dotProduct(norm_B, perp_VEC) < 0) //right side of BtoC
		{
			iSect.setIfIntersected(false);
			return false;
		}

		//2.3 CtoA edge
		Vec3D CtoP = int_pt - vert_C;
		perp_VEC = cross(CtoA, -1*CtoP);

		if (u = dotProduct(norm_C, perp_VEC) < 0) //this is u since u = CAP/ABC
		{
			iSect.setIfIntersected(false);
			return false;
		}

		u /= dotProduct(norm_C, norm_C); //u = 1 --> P = B
		v /= dotProduct(norm_A, norm_A); //v = 1 --> P = C

		//3. set values inside of iSect
		//minT, point, and color already set by Plane.findIntersectionPoint()
		iSect.setIfIntersected(true);
		
		Vec3D n = (1 - u - v)*norm_A + u*norm_B + v*norm_C;
		n.normalize();
		//make sure normal points back towards camera
		if (dotProduct(n, iSect.getToCamVec()) < 0)
		{
			//printf("Calculated normal pointing wrong way\n");
			n = -1 * n;
		}

		iSect.setNormal(n); //interpolate normal
		return true;
	}
	
	iSect.setIfIntersected(false);
	return false;
}

int Triangle::getType()
{
	return TRIANGLE_PRIM;
}