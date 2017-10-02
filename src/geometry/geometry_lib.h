#pragma once

#include <ostream>

#define CLIP_RESULT_SINGLE 1

#define MIN_VERTEX_NUM 3		///< minimum number of vertices in polygon
#define MAX_VERTEX_NUM 64		///< maximum number of vertices in polygon
#define MAX_POLYGON_NUM 512		///< maximum number of polygons in array of polygons
#define MAX_FACET_NUM 256

enum Location: bool
{
	In, Out
};

struct Orientation
{
	double beta;
	double gamma;
	double alpha;
};

typedef Orientation Symmetry;

struct IntArray
{
	int arr[MAX_FACET_NUM];
	int size = 0;

	void Add(int elem)
	{
		arr[size++] = elem;
	}
};

template <class T>
struct Couple
{
	T first;
	T last;
};

// short access for Point3f
#define c_x		_point[0]
#define c_y		_point[1]
#define c_z		_point[2]
#define d_param _point[3]

// short access for normals of Facet
#define in_normal normal[0]
#define ex_normal normal[1]

/**
 * @brief The Point3 struct
 * 3D coordinate point
 */
struct Point3f
{
	float _point[4]; /// coordinates

	Point3f() {}

	Point3f(float x, float y, float z)
	{
		_point[0] = x;
		_point[1] = y;
		_point[2] = z;
	}

	Point3f(float x, float y, float z, float d)
	{
		_point[0] = x;
		_point[1] = y;
		_point[2] = z;
		_point[3] = d;
	}

	Point3f(const Point3f &other)
	{
		_point[0] = other._point[0];
		_point[1] = other._point[1];
		_point[2] = other._point[2];
	}

	Point3f & operator = (const Point3f &other)
	{
		_point[0] = other._point[0];
		_point[1] = other._point[1];
		_point[2] = other._point[2];

		return *this;
	}

	Point3f operator * (double value) const
	{
		return Point3f(_point[0] * value,
				_point[1] * value,
				_point[2] * value);
	}

	Point3f operator / (double value) const
	{
		return Point3f(_point[0] / value,
				_point[1] / value,
				_point[2] / value);
	}

	Point3f operator - (const Point3f &value) const
	{
		return Point3f(_point[0] - value._point[0],
				_point[1] - value._point[1],
				_point[2] - value._point[2]);
	}

	Point3f operator + (const Point3f &value) const
	{
		return Point3f(_point[0] + value._point[0],
				_point[1] + value._point[1],
				_point[2] + value._point[2]);
	}

	Point3f operator += (double value)
	{
		return *this = Point3f(_point[0] + value,
				_point[1] + value,
				_point[2] + value);
	}

	Point3f operator - () const
	{
		return Point3f(-_point[0], -_point[1], -_point[2], -_point[3]);
	}

} __attribute__ ((aligned (16)));

struct Point3d
{
	double x;
	double y;
	double z;
	double d;

	Point3d() {}

	Point3d(const Point3f &other)
	{
		x = other.c_x;
		y = other.c_y;
		z = other.c_z;
		d = other.d_param;
	}

	Point3d(double p_x, double p_y, double p_z, double p_d = 0.0)
	{
		x = p_x;
		y = p_y;
		z = p_z;
		d = p_d;
	}

	Point3d operator * (double value) const
	{
		return Point3d(x*value, y*value, z*value);
	}

	Point3d operator + (const Point3d &other) const
	{
		return Point3d(x + other.x, y + other.y, z + other.z);
	}

	Point3d operator - (const Point3d &other) const
	{
		return Point3d(x - other.x, y - other.y, z - other.z);
	}

	Point3d operator - () const
	{
		return Point3d(-x, -y, -z);
	}

	Point3d operator / (double value) const
	{
		return Point3d(x/value, y/value, z/value);
	}
};


/**
 * Functions
 */

float DotProduct(const Point3f &v1, const Point3f &v2);
double DotProductD(const Point3d &v1, const Point3d &v2);
void CrossProduct(const Point3f &v1, const Point3f &v2, Point3f &res);
Point3f CrossProduct(const Point3f &v1, const Point3f &v2);

double Norm(const Point3f &point);
void Normalize(Point3f &v);
double Length(const Point3f &v);



// REF: try to create template class 'Array<type>'

/**
 * Functions
 */

float DotProduct(const Point3f &v1, const Point3f &v2);
double DotProductD(const Point3d &v1, const Point3d &v2);

double Norm(const Point3f &point);

void CrossProduct(const Point3f &v1, const Point3f &v2, Point3f &res);
Point3d CrossProductD(const Point3d &v1, const Point3d &v2);

void Normalize(Point3f &v);

double Length(const Point3f &v);
double LengthD(const Point3d &v);
