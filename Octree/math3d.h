#ifndef __MATH3D_H
#define __MATH3D_H

#include <stdio.h>
#ifdef WIN32
#define _USE_MATH_DEFINES
#include <cmath>
#else
#include <math.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>



// DEFINITIONS ///////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

// Pi ....................................................................
#define PI					((float)3.141592654f)
#define PI2					((float)6.283185307f)
#define PI_DIV_2			((float)1.570796327f)
#define PI_DIV_4			((float)0.785398163f)
#define PI_INV				((float)0.318309886f)
#define PI_OVER_180			((float)PI / 180.0f)
#define _180_OVER_PI		((float)180.0f / PI)

// Small numbers .........................................................
#define EPSILON_E3			(float)(1E-3)
#define EPSILON_E4			(float)(1E-4)
#define EPSILON_E5			(float)(1E-5)
#define EPSILON_E6			(float)(1E-6)


// MACROS / INLINES //////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#define DEG_TO_RAD(x)		(float)(((x) * PI_OVER_180))
#define RAD_TO_DEG(x)		(float)(((x) * _180_OVER_PI))
#define MIN(a, b)			((a)<(b) ? (a):(b))
#define MAX(a, b)			((a)>(b) ? (a):(b))


// TYPEDEFS /STRUCTS /////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

// 3D vector (float) .....................................................
struct Vector3f
{
	float x, y, z;

	// Constructors
	Vector3f() : x(0.0f), y(0.0f), z(0.0f) {};
	Vector3f(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {};

	// Operators
    Vector3f& operator+=(const Vector3f& r)	{	x += r.x;	y += r.y;	z += r.z;	return *this;	}
    Vector3f& operator-=(const Vector3f& r)	{	x -= r.x;	y -= r.y;	z -= r.z;	return *this;	}
    Vector3f& operator*=(float f)			{	x *= f;		y *= f;		z *= f;		return *this;	}

	Vector3f& operator*(float f)			{	x *= f;		y *= f;		z *= f;		return *this;	}
	Vector3f operator+(const Vector3f &v)	{	return (Vector3f(x+v.x, y+v.y, z+v.z));				}
	Vector3f operator-(const Vector3f &v)	{	return (Vector3f(x-v.x, y-v.y, z-v.z));				}
	
	// Member functions
    Vector3f Cross(const Vector3f &v) const;
	float Dot(const Vector3f &v) const;
    float Length() const;
	float Length2() const;
	Vector3f& Normalize();
	
	void Print(const char *s);
	
};

// 2D vector (float) .....................................................
struct Vector2f
{
	float x, y;

	// Constructors
	Vector2f() : x(0.0f), y(0.0f) {};
	Vector2f(float _x, float _y) : x(_x), y(_y) {};

	// Operators
	Vector2f& operator+=(const Vector2f& r)	{ x += r.x;	y += r.y;	return *this; }
	Vector2f& operator+=(float f)			{ x += f;	y += f;		return *this; }
	Vector2f& operator-=(const Vector2f& r)	{ x -= r.x;	y -= r.y;	return *this; }
	Vector2f& operator*=(float f)			{ x *= f;	y *= f;		return *this; }

	Vector2f& operator*(float f)			{ x *= f;	y *= f;		return *this; }
	Vector2f operator+(const Vector2f &v)	{ return (Vector2f(x + v.x, y + v.y)); }
};

// 2D vector (double) .....................................................
struct Vector2d
{
	double x, y;

	// Constructors
	Vector2d() : x(0.0), y(0.0) {};
	Vector2d(double _x, double _y) : x(_x), y(_y) {};

	// Operators
	Vector2d& operator+=(const Vector2d& r)	{ x += r.x;	y += r.y;	return *this; }
	Vector2d& operator+=(double d)			{ x += d;	y += d;		return *this; }
	Vector2d& operator-=(const Vector2d& r)	{ x -= r.x;	y -= r.y;	return *this; }
	Vector2d& operator*=(double d)			{ x *= d;	y *= d;		return *this; }

	Vector2d& operator*(double d)			{ x *= d;	y *= d;		return *this; }
	Vector2d operator+(const Vector2d &v)	{ return (Vector2d(x + v.x, y + v.y)); }
};


// CLASSES ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

class Matrix4f
{
public:

	// Constructor
	Matrix4f() {};

	// Inline member functions
	inline void Identity()
	{
        m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0.0f;
        m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = 0.0f;
        m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = 0.0f;
        m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
	}

	// matrix multiplication
	inline Matrix4f operator *(const Matrix4f &mult) const
	{
        Matrix4f ret;

        for (unsigned int i = 0 ; i < 4 ; i++) {
            for (unsigned int j = 0 ; j < 4 ; j++) {
                ret.m[i][j] = m[i][0] * mult.m[0][j] +
                              m[i][1] * mult.m[1][j] +
                              m[i][2] * mult.m[2][j] +
                              m[i][3] * mult.m[3][j];
            }
        }
        return ret;
	}

	// Memeber functions
	void					ScaleTransform			(float x, float y, float z);
	void					RotateTransform			(float x, float y, float z);
    void					TranslationTransform	(float x, float y, float z);

	void					QuaternionRotation		(glm::fquat q);

    void					CameraTransform			(Vector3f U, Vector3f V, Vector3f N);
    void					PersProjTransform		(float fov, float ar, float znear, float zfar);
	
	void					Print					(const char *str);
	
	// Member variables
	float					m[4][4];

};

// FUNCTION PROTOTYPES ///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

Vector3f	Cross					(const Vector3f &v0, const Vector3f &v1);
float		Dot						(const Vector3f &v0, const Vector3f &v1);
glm::fquat	AxisAngleToQuaternion	(float &angle, Vector3f &axis);


#endif // __MATH3D_H


