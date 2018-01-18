#include "Util.h"


bool util::solveQuadratic(const float & a, const float & b, const float & c, float & x0, float & x1)
{
	float discr = b * b - 4 * a * c;
	if (discr < 0) return false;
	else if (discr == 0) x0 = x1 = -0.5 * b / a;
	else {
		float q = (b > 0) ?
			-0.5 * (b + sqrt(discr)) :
			-0.5 * (b - sqrt(discr));
		x0 = q / a;
		x1 = c / q;
	}
	if (x0 > x1) std::swap(x0, x1);

	return true;
}

void util::pixel_to_vec(Pixel p, Vec3D& v)
{
	v.setX(p.r);
	v.setY(p.g);
	v.setZ(p.b);
}

void util::vec_to_pixel(Vec3D v, Pixel& p)
{
	p.r = ComponentClamp(v.getX());
	p.g = ComponentClamp(v.getY());
	p.b = ComponentClamp(v.getZ());
}
