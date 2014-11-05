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


// STRUCTURES / TYPE DEFINITIONS /////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


// Template, type dependent 3D vector ....................................
#pragma warning(disable : 4244)		// conversion from double to float

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

	Vector3t operator+(const Vector3t &_v)		{	return (Vector3t<float>(x + _v.x, y + _v.y, z + _v.z));	}
	Vector3t operator-(const Vector3t &_v)		{	return (Vector3t<float>(x - _v.x, y - _v.y, z - _v.z));	}
	Vector3t operator*(const T _t)				{	x *= _t;	y *= _t;	z *= _t;	return (*this); }

	// Member functions
	Vector3t<T> Cross(const Vector3t<T> &_v) const
	{
		const T _x = y * _v.z - z * _v.y;
		const T _y = z * _v.x - x * _v.z;
		const T _z = x * _v.y - y * _v.x;

		return (Vector3t<T>(_x, _y, _z));
	}

	T Dot(const Vector3t<T> &_v) const
	{
		return (x * _v.x + y * _v.y + z * _v.z);
	}

	T Length() const
	{
		return (sqrt(x * x + y * y + z * z));
	}

	T Length2() const
	{
		return (x * x + y * y + z * z);
	}

	Vector3t<T> &Normalize()
	{
		T length = sqrt(x * x + y * y + z * z);

		if (length < EPSILON_E5)
			return (*this);

		T length_inv = 1.0 / length;
		x *= length_inv;
		y *= length_inv;
		z *= length_inv;

		return (*this);
	}

	void Print(const char *_s)
	{
		Logw("%s  [ %.1f  %.1f  %.1f ]\n", s, x, y, z);
	}

};



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

    void					CameraTransform			(Vector3t<float> U, Vector3t<float> V, Vector3t<float> N);
    void					PersProjTransform		(float fov, float ar, float znear, float zfar);
	
	void					Print					(const char *str);
	
	// Member variables
	float					m[4][4];

};

// FUNCTION PROTOTYPES ///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

template<typename T>
Vector3t<T>	Cross(const Vector3t<T> &_v0, const Vector3t<T> &_v1)
{
	const T _x = _v0.y * _v1.z - _v0.z * _v1.y;
	const T _y = _v0.z * _v1.x - _v0.x * _v1.z;
	const T _z = _v0.x * _v1.y - _v0.y * _v1.x;

	return (Vector3t<T>(_x, _y, _z));
}

template<typename T>
T Dot(const Vector3t<T> &_v0, const Vector3t<T> &_v1)
{
	return (_v0.x * _v1.x + _v0.y * _v1.y + _v0.z * _v1.z);
}


glm::fquat	AxisAngleToQuaternion	(float &angle, Vector3t<float> &axis);


#endif // __MATH3D_H






// Vector3t MEMBER FUNCTIONS /////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/*
template<typename T>
Vector3t<T> Vector3t<T>::Cross(const Vector3t<T> &_v) const
{
	const T _x = y * _v.z - z * _v.y;
	const T _y = z * _v.x - x * _v.z;
	const T _z = x * _v.y - y * _v.x;

	return (Vector3t<T>(_x, _y, _z));
}

//////////////////////////////////////////////////////////////////////////
template<typename T>
Vector3t<T>	Cross(const Vector3t<T> &_v0, const Vector3t<T> &_v1)
{
	const T _x = _v0.y * _v1.z - _v0.z * _v1.y;
	const T _y = _v0.z * _v1.x - _v0.x * _v1.z;
	const T _z = _v0.x * _v1.y - _v0.y * _v1.x;

	return (Vector3t<T>(_x, _y, _z));
}

//////////////////////////////////////////////////////////////////////////
template<typename T>
T Vector3t<T>::Dot(const Vector3t<T> &_v) const
{
	return (x * _v.x + y * _v.y + z * _v.z);
}

//////////////////////////////////////////////////////////////////////////
template<typename T>
Vector3t<T> Dot(const Vector3t<T> &_v0, const Vector3t<T> &_v1)
{
	return (_v0.x * _v1.x + _v0.y * _v1.y + _v0.z * _v1.z);
}

//////////////////////////////////////////////////////////////////////////
template<typename T>
T Vector3t<T>::Length() const
{
	return (sqrt(x * x + y * y + z * z));
}

//////////////////////////////////////////////////////////////////////////
template<typename T>
T Vector3t<T>::Length2() const
{
	return (x * x + y * y + z * z);
}

//////////////////////////////////////////////////////////////////////////
template<typename T>
Vector3t<T> &Vector3t<T>::Normalize()
{
	T length = sqrt(x * x + y * y + z * z);

	if (length < EPSILON_E5)
		return (*this);

	T length_inv = 1.0 / length;
	x *= length_inv;
	y *= length_inv;
	z *= length_inv;

	return (*this);
}

//////////////////////////////////////////////////////////////////////////
template<typename T>
void Vector3t<T>::Print(const char *_s)
{
	Logw("%s  [ %.1f  %.1f  %.1f ]\n", s, x, y, z);
}

*/


