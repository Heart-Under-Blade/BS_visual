#include <math.h>
#include "Particle.h"
#include "intrinsic/intrinsics.h"

float DotProduct(const Point3f &v1, const Point3f &v2)
{
	__m128 _v1 = _mm_setr_ps(v1.c_x, v1.c_y, v1.c_z, 0.0);
	__m128 _v2 = _mm_setr_ps(v2.c_x, v2.c_y, v2.c_z, 0.0);
	__m128 _dp0 = _mm_dp_ps(_v1, _v2, MASK_FULL);
	return _dp0[0];
}

double DotProductD(const Point3d &v1, const Point3d &v2)
{
	return	  v1.x * v2.x
			+ v1.y * v2.y
			+ v1.z * v2.z;
}

double Norm(const Point3f &p)
{
	return	  p.c_x * p.c_x
			+ p.c_y * p.c_y
			+ p.c_z * p.c_z;
}

double NormD(const Point3d &p)
{
	return	  p.x * p.x
			+ p.y * p.y
			+ p.z * p.z;
}

void CrossProduct(const Point3f &v1, const Point3f &v2, Point3f &res)
{
	__m128 _v1 = _mm_setr_ps(v1.c_x, v1.c_y, v1.c_z, 0.0);
	__m128 _v2 = _mm_setr_ps(v2.c_x, v2.c_y, v2.c_z, 0.0);
	__m128 _cp = _cross_product(_v1, _v2);

	res.c_x = _cp[0];
	res.c_y = _cp[1];
	res.c_z = _cp[2];
}

// Rc_z: try to move to Point3f
Point3f CrossProduct(const Point3f &v1, const Point3f &v2)
{
	__m128 _v1 = _mm_setr_ps(v1.c_x, v1.c_y, v1.c_z, 0.0);
	__m128 _v2 = _mm_setr_ps(v2.c_x, v2.c_y, v2.c_z, 0.0);
	__m128 _cp = _cross_product(_v1, _v2);

	Point3f res;
	res.c_x = _cp[0];
	res.c_y = _cp[1];
	res.c_z = _cp[2];

	return res;
}

// OPT:
Point3d CrossProductD(const Point3d &v1, const Point3d &v2)
{
	Point3d res;
	res.x = v1.y*v2.z - v1.z*v2.y;
	res.y = v1.z*v2.x - v1.x*v2.z;
	res.z = v1.x*v2.y - v1.y*v2.x;
	return res;
}

double Length(const Point3f &v)
{
	return sqrt(Norm(v));
}

double LengthD(const Point3d &v)
{
	return sqrt(NormD(v));
}

void Normalize(Point3f &v)
{
	double lenght = sqrt(Norm(v));
	v.c_x /= lenght;
	v.c_y /= lenght;
	v.c_z /= lenght;
}
