
#include "math3d.h"
#include "utility.h"


// Vector3f MEMBER FUNCTIONS /////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

Vector3f Vector3f::Cross(const Vector3f &v) const
{
    const float _x = y * v.z - z * v.y;
    const float _y = z * v.x - x * v.z;
    const float _z = x * v.y - y * v.x;

    return (Vector3f(_x, _y, _z));
}

//////////////////////////////////////////////////////////////////////////
Vector3f Cross(const Vector3f &v0, const Vector3f &v1)
{
    const float _x = v0.y * v1.z - v0.z * v1.y;
    const float _y = v0.z * v1.x - v0.x * v1.z;
    const float _z = v0.x * v1.y - v0.y * v1.x;

    return (Vector3f(_x, _y, _z));
}

//////////////////////////////////////////////////////////////////////////
float Vector3f::Dot(const Vector3f &v) const
{
	return (x*v.x + y*v.y + z*v.z);
}

//////////////////////////////////////////////////////////////////////////
float Dot(const Vector3f &v0, const Vector3f &v1)
{
	return (v0.x*v1.x + v0.y*v1.y + v0.z*v1.z);
}

//////////////////////////////////////////////////////////////////////////
float Vector3f::Length() const
{
	return (sqrtf(x*x + y*y + z*z));
}

//////////////////////////////////////////////////////////////////////////
float Vector3f::Length2() const
{
	return (x*x + y*y + z*z);
}

//////////////////////////////////////////////////////////////////////////
Vector3f& Vector3f::Normalize()
{
    const float length = sqrtf(x * x + y * y + z * z);

	if (length < EPSILON_E5)
		return *this;
    
	float length_inv = 1.0f / length;
	x *= length_inv;
    y *= length_inv;
    z *= length_inv;

    return *this;
}

//////////////////////////////////////////////////////////////////////////
void Vector3f::Print(const char *s)
{
	Logw("%s  [ %.1f  %.1f  %.1f ]\n", s, x, y, z);

} // end Vector3f::Print()

//////////////////////////////////////////////////////////////////////////
glm::fquat AxisAngleToQuaternion(float &angle, Vector3f &axis)
{
	glm::fquat q;
	float angle_2 = DEG_TO_RAD(angle) / 2.0f;
	float sin_angle_2 = sinf(angle_2);
	
	q.w = cosf(angle_2);
	q.x = axis.x * sin_angle_2;
	q.y = axis.y * sin_angle_2;
	q.z = axis.z * sin_angle_2;

	q = glm::normalize(q);

	return (q);

} // end AxisAngleToQuat()

// Matrix4f MEMBER FUNCTIONS /////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void Matrix4f::ScaleTransform(float x, float y, float z)
{
    m[0][0] = x;	m[0][1] = 0.0f;	m[0][2] = 0.0f;	m[0][3] = 0.0f;
    m[1][0] = 0.0f;	m[1][1] = y;	m[1][2] = 0.0f;	m[1][3] = 0.0f;
    m[2][0] = 0.0f;	m[2][1] = 0.0f;	m[2][2] = z;	m[2][3] = 0.0f;
    m[3][0] = 0.0f;	m[3][1] = 0.0f;	m[3][2] = 0.0f;	m[3][3] = 1.0f;

} // end Matrix4f::ScaleTransform()

//////////////////////////////////////////////////////////////////////////
void Matrix4f::RotateTransform(float x, float y, float z)
{
	Matrix4f m;

    const float _x = DEG_TO_RAD(x);
    const float _y = DEG_TO_RAD(y);
    const float _z = DEG_TO_RAD(z);

	const float a = cosf(_x);
	const float b = sinf(_x);
	const float c = cosf(_y);
	const float d = sinf(_y);
	const float e = cosf(_z);
	const float f = sinf(_z);
	
	const float ad = a * d;
	const float bd = b * d;

	m.m[0][0] = c * e;
	m.m[0][1] = -c * f;
	m.m[0][2] = -d;
	m.m[1][0] = -bd * e + (a * f);
	m.m[1][1] = bd * f + (a * e);
	m.m[1][2] = -b * c;
	m.m[2][0] = ad * e + (b * f);
	m.m[2][1] = -ad * f + (b * e);
	m.m[2][2] = a * c;

	m.m[0][3] = m.m[1][3] = m.m[2][3] = m.m[3][0] = m.m[3][1] = m.m[3][2] = 0.0f;
	m.m[3][3] = 1.0f;

    *this = m;

} // end Matrix4f::RotateTransform()

//////////////////////////////////////////////////////////////////////////
void Matrix4f::TranslationTransform(float x, float y, float z)
{
    m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = x;
    m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = y;
    m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = z;
    m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;

} // end Matrix4f::TranslationTransform()

//////////////////////////////////////////////////////////////////////////
void Matrix4f::QuaternionRotation(glm::fquat q)
{
	float x2 = q.x * q.x;
	float y2 = q.y * q.y;
	float z2 = q.z * q.z;
	float xy = q.x * q.y;
	float xz = q.x * q.z;
	float yz = q.y * q.z;
	float wx = q.w * q.x;
	float wy = q.w * q.y;
	float wz = q.w * q.z;

	
	m[0][0] = 1.0f - 2*y2 - 2*z2;	m[0][1] = 2*xy - 2*wz;			m[0][2] = 2*xz + 2*wy;			m[0][3] = 0.0f;
	m[1][0] = 2*xy + 2*wz;			m[1][1] = 1.0f - 2*x2 - 2*z2;	m[1][2] = 2*yz - 2*wx;			m[1][3] = 0.0f;
	m[2][0] = 2*xz - 2*wy;			m[2][1] = 2*yz + 2*wx;			m[2][2] = 1.0f - 2*x2 - 2*y2;	m[2][3] = 0.0f;
	m[3][0] = 0.0f;					m[3][1] = 0.0f;					m[3][2] = 0.0f;					m[3][3] = 1.0f;

} // end Matrix4f::QuaternionRotation()

//////////////////////////////////////////////////////////////////////////
void Matrix4f::CameraTransform(Vector3f U, Vector3f V, Vector3f N)
{
    m[0][0] = U.x;   m[0][1] = U.y;   m[0][2] = U.z;   m[0][3] = 0.0f;
    m[1][0] = V.x;   m[1][1] = V.y;   m[1][2] = V.z;   m[1][3] = 0.0f;
    m[2][0] = N.x;   m[2][1] = N.y;   m[2][2] = N.z;   m[2][3] = 0.0f;
    m[3][0] = 0.0f;  m[3][1] = 0.0f;  m[3][2] = 0.0f;  m[3][3] = 1.0f;

} // end Matrix4f::CameraTransform()

//////////////////////////////////////////////////////////////////////////
void Matrix4f::PersProjTransform(float fov, float ar, float znear, float zfar)
{
	const float zRange     = znear - zfar;
	const float tanHalfFOV = tanf(DEG_TO_RAD(fov / 2.0f));
	
	m[0][0] = 1.0f/(tanHalfFOV * ar); m[0][1] = 0.0f;            m[0][2] = 0.0f;          m[0][3] = 0.0;
	m[1][0] = 0.0f;                   m[1][1] = 1.0f/tanHalfFOV; m[1][2] = 0.0f;          m[1][3] = 0.0;
	m[2][0] = 0.0f;                   m[2][1] = 0.0f;            m[2][2] = (-znear-zfar)/zRange ; m[2][3] = 2.0f * zfar*znear/zRange;
	m[3][0] = 0.0f;                   m[3][1] = 0.0f;            m[3][2] = 1.0f;          m[3][3] = 0.0;

} // end Matrix4f::PersProjTransform()

//////////////////////////////////////////////////////////////////////////
void Matrix4f::Print(const char *str)
{
	Logw("%s\n", str);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			Logw("%.10f  ", m[i][j]);
		}
		Logw("\n");
	}
	Logw("\n");


} // end Matrix4f::Print()

