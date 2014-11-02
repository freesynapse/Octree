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
/*
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
*/

// Template, type dependent 3D vector ....................................
template<typename T>
struct Vector3t
{
	T x, y, z;

	// CONSTRUCTORS
	Vector3t() : x(0.0), y(0.0), z(0.0) {}
	Vector3t(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}

	// OPERATORS
	Vector3t &operator+=(const Vector3t &_v)	{	x += _v.x;	y += _v.y;	z += _v.z;	return (*this);	}
	Vector3t &operator-=(const Vector3t &_v)	{	x -= _v.x;	y -= _v.y;	z -= _v.z;	return (*this); }
	Vector3t &operator*=(T _t)					{	x *= _t;	y *= _t;	z *= _t;	return (*this); }

	Vector3t operator+(const Vector3t &_v)		{	return (Vector3f(x + _v.x, y + _v.y, z + _v.z));	}
	Vector3t operator-(const Vector3t &_v)		{	return (Vector3f(x - _v.x, y - _v.y, z - _v.z));	}
	Vector3t operator*(const T _t)				{	x *= _t;	y *= _t;	z *= _t;	return (*this); }

	// Member functions
	Vector3t<T> Cross(const Vector3t<T> &_v) const;
	T Dot(const Vector3t<T> &_v) const;
	T Length() const;
	T Length2() const;
	Vector3t<T> &Normalize();

	void Print(const char *_s);

};


// Some basic type definitions of 3D vector types
typedef Vector3t<float> Vector3f;
typedef Vector3t<double> Vector3d;
typedef Vector3t<int> Vector3i;



// Template, type dependent 2D vector ....................................
template<typename T>
struct Vector2t
{
	T x, y;

	// Constructors
	Vector2t() : x(0), y(0) {}
	Vector2t(T _x, T _y) : x(_x), y(_y) {}

	// Operators
	Vector2t& operator+=(const Vector2t& r)	{ x += r.x;	y += r.y;	return (*this);	}
	Vector2t& operator+=(T d)				{ x += d;	y += d;		return (*this); }
	Vector2t& operator-=(const Vector2t& r)	{ x -= r.x;	y -= r.y;	return (*this); }
	Vector2t& operator*=(T d)				{ x *= d;	y *= d;		return (*this); }

	Vector2t& operator*(T d)				{ x *= d;	y *= d;		return (*this); }
	Vector2t operator+(const Vector2t &v)	{ return (Vector2t(x + v.x, y + v.y));	}

};

// Some basic type definitions of 2D vector types
typedef Vector2t<float> Vector2f;
typedef Vector2t<double> Vector2d;
typedef Vector2t<int> Vector2i;



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

//Vector3f	Cross					(const Vector3f &v0, const Vector3f &v1);
//float		Dot						(const Vector3f &v0, const Vector3f &v1);

template<typename T>
Vector3t<T>	Cross					(const Vector3t<T> &_v0, const Vector3t<T> &_v1);
template<typename T>
Vector3t<T> Dot						(const Vector3t<T> &_v0, const Vector3t<T> &_v1);

glm::fquat	AxisAngleToQuaternion	(float &angle, Vector3f &axis);


#endif // __MATH3D_H


