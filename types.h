#pragma once

#include <vector>
#include <math.h>
#include <float.h>

class Point2d
{
public:
	double x;
	double y;

	Point2d() {
		x = y = 0;
	}

	Point2d(double x, double y) {
		this->x = x;
		this->y = y;
	}
};
typedef Point2d CPoint2D;

inline bool isPracticallyZero(double a, double b = 0)
{
	if (!b) return fabs (a) <=  DBL_MIN;	
	return fabs (a - b) <= std::min(fabs(a), fabs(b)) * DBL_EPSILON;	
}

struct Point3d 
{
public:
	Point3d(void) {}
	Point3d(double xx, double yy, double zz) {
		x = xx; y = yy; z = zz;
	}

	void SetXYZ(double xx, double yy, double zz) {
		x = xx; y = yy; z = zz;
	}
	double x, y, z;

	Point3d& operator+=(Point3d t)
    {
        x += t.x;
        y += t.y;
        z += t.z;

        return *this;
    }

    Point3d& operator-=(Point3d t)
    {
        x -= t.x;
        y -= t.y;
        z -= t.z;

        return *this;
    }
    Point3d& operator*=(double d)
    {
        x *= d;
        y *= d;
        z *= d;

        return *this;
    }
    Point3d& operator/=(double d)
    {
        x /= d;
        y /= d;
        z /= d;

        return *this;
    }
    Point3d& operator*=(Point3d t) // scale
    {
        x *= t.x;
        y *= t.y;
        z *= t.z;

        return *this;
    }

    bool operator!=(Point3d t) const
    {
        return !isPracticallyZero(x,t.x) || !isPracticallyZero(y,t.y) || !isPracticallyZero(z,t.z);
    }

    bool operator==(Point3d t) const
    {
        return !operator!=(t);
    }

    double length() const
    {
        double l2 = x*x + y*y + z*z;
        return (isPracticallyZero(l2)) ? 0 :sqrt(l2);
    }

    void normalize()
    {
        double l = length();
        if (l)
            *this /= l;
    }
};

inline const Point3d operator+(const Point3d& t, const Point3d& t2)
{
    return Point3d(t) += t2;
}
inline const Point3d operator-(const Point3d& t, const Point3d& t2)
{
    return Point3d(t) -= t2;
}
inline const Point3d operator*(double d, const Point3d& t)
{
    return Point3d(t) *= d;
}
inline const Point3d operator*(const Point3d& t, double d)
{
    return Point3d(t) *= d;
}
inline const Point3d operator/(double d, const Point3d& t)
{
    return Point3d(t) /= d;
}
inline const Point3d operator/(const Point3d& t, double d)
{
    return Point3d(t) /= d;
}
inline const Point3d operator*(const Point3d& t, const Point3d& t2)
{
    return Point3d(t) *= t2;
}


inline double length(const Point3d& t, const Point3d& t2)
{
	double l2 = (t.x - t2.x) * (t.x - t2.x) + (t.y - t2.y) * (t.y - t2.y) + (t.z - t2.z) * (t.z - t2.z);
	return (isPracticallyZero(l2)) ? 0 :sqrt(l2);
}

typedef Point3d Vector3d;
inline Vector3d normalizedcross(Vector3d const& u, Vector3d const& v)
{
	Vector3d n;

	/* compute the cross product (u x v for right-handed [ccw]) */
	n.x = u.y * v.z - u.z * v.y;
	n.y = u.z * v.x - u.x * v.z;
	n.z = u.x * v.y - u.y * v.x;

	/* normalize */
	double l = n.length();
	if (l)
	{
		n /= l;
	}
	else
	{
		n = Vector3d(0,0,0);
	}

	return n;
}

inline double dotProduct(Vector3d const& u, Vector3d const& v)
{
	return u.x*v.x + u.y*v.y + u.z*v.z;
}


typedef std::vector<Point3d> DataRow;
typedef std::vector<DataRow> DataGrid;

typedef DataRow PingData;
typedef DataGrid SwathData;
typedef DataRow NavData;
