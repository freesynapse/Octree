#ifndef __VXCAMERA_H
#define __VXCAMERA_H

// INCLUSIONS ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include "math3d.h"
#include "utility.h"


// DEFINITIONS ///////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

// c_Transform definitions ...............................................
#define FOV											60.0f
#define MATRIX_TRANSFORM_OBJ						0x0
#define MATRIX_TRANSFORM_CAM						0x1

// c_Camera definitions ..................................................
#define CAMERA_MOVE_FORWARD							0x0001
#define CAMERA_MOVE_BACKWARDS						0x0002
#define CAMERA_MOVE_LEFT							0x0004
#define CAMERA_MOVE_RIGHT							0x0008
#define CAMERA_MOVE_UP								0x0010
#define CAMERA_MOVE_DOWN							0x0020


struct sPersProj
{
	float fFOV;
	float fAspectRatio;
	float fZNear;
	float fZFar;
};


// CLASSES ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

// Transformation class ..................................................
class c_Transform
{
public:
	// Constructor / destructor
	c_Transform() : m_vScale(Vector3f(0.0f, 0.0f, 0.0f)),
					m_vWorldPos(Vector3f()),
					m_vRotate(Vector3f())
					{};

	// Accessors
	void Scale(float x, float y, float z)
	{	
		m_vScale.x = x;	
		m_vScale.y = y;	
		m_vScale.z = z;	
	}

	void Scale(Vector3f &_scale)	{	m_vScale = _scale;	}

	void WorldPos(float x, float y, float z)
	{
		m_vWorldPos.x = x;	
		m_vWorldPos.y = y;	
		m_vWorldPos.z = z;	
	}

	void WorldPos(Vector3f &_pos)	{	m_vWorldPos = _pos;	}

	void Rotate(float x, float y, float z)
	{
		m_vRotate.x = x;	
		m_vRotate.y = y;	
		m_vRotate.z = z;	
	}

	void Rotate(Vector3f &_rot)		{	m_vRotate = _rot;	}

	void SetPerspectiveProj(float fov, float aspect_ratio, float znear, float zfar)
	{
		m_persProj.fFOV			= fov;
		m_persProj.fAspectRatio = aspect_ratio;
		m_persProj.fZNear		= znear;
		m_persProj.fZFar		= zfar;
	}

	// Member functions
	void					Identity				(int matrix_flag);

	Matrix4f				*TransformCamMatrix		(Vector3f cam_pos,
													 Vector3f U, 
													 Vector3f V, 
													 Vector3f N);

	Matrix4f				*TransformObjMatrix		();

	void					SetShaderMatrix			(int matrix_flag,
													 GLint matrix_handle);

	void					PrintMatrix				(int matrix_flag,
													 const char *str);

private:
	// Member variables
	Vector3f				m_vScale;
	Vector3f				m_vWorldPos;
	Vector3f				m_vRotate;

	sPersProj				m_persProj;

	Matrix4f				m_mTransform[2];	// MATRIX_TRANSFORM_OBJ=0 or
												// MATRIX_TRANSFORM_CAM=1
};

// Camera class ..........................................................
class c_Camera
{
public:
	// Constructor
	c_Camera(GLuint &_shader_program);
	~c_Camera();

	// Accessors
	Vector3f				&Pos()						{	return (m_vPos);			}
	Vector3f				&Target()					{	return (m_vTarget);			}
	Vector3f				&Up()						{	return (m_vUp);				}
	c_Transform				*pTransform()				{	return (m_pTransform);		}

	Vector3f				&U()						{	return (m_u);				}
	Vector3f				&V()						{	return (m_v);				}
	Vector3f				&N()						{	return (m_n);				}

	float					MoveSpeed()					{	return (m_fMoveSpeed);		}
	void					SetMoveSpeed(float _s)		{	m_fMoveSpeed = _s;			}

	float					Heading()					{	return (m_fHeading);		}
	void					SetHeading(float _h)		{	m_fHeading = _h;			}
	float					HeadingSpeed()				{	return (m_fHeadingSpeed);	}
	void					SetHeadingSpeed(float _h)	{	m_fHeadingSpeed = _h;		}

	float					Elevation()					{	return (m_fElevation);		}
	void					SetElevation(float _e)		{	m_fElevation = _e;			}
	float					ElevationSpeed()			{	return (m_fElevationSpeed);	}
	void					SetElevationSpeed(float _e)	{	m_fElevationSpeed = _e;		}

	Matrix4f				ProjectionMatrix();
	Matrix4f				ViewMatrix();


	// Member functions
	void					SetCamera					(Vector3f &_pos,
														 Vector3f &_target,
														 Vector3f &_up);

	void					MoveCamera					(int _direction);

	void					SetTarget					(float _heading,
														 float _elevation);

	void					PerspectiveProjection		(float _fov,
														 float _ar,
														 float _znear,
														 float _zfar);

	void					Update						();


private:
	// Member variables
	Vector3f				m_vPos;
	Vector3f				m_vTarget;
	Vector3f				m_vUp;

	Vector3f				m_u;
	Vector3f				m_v;
	Vector3f				m_n;
	Vector3f				m_f;

	float					m_fMoveSpeed;
	float					m_fHeading;
	float					m_fHeadingSpeed;
	float					m_fElevation;
	float					m_fElevationSpeed;

	GLuint					m_programID;			// handle to shader program
	GLint					m_vsCamTransform;		// handle to the camera transformation matrix in the vertex shader

	c_Transform				*m_pTransform;			// transformation class pointer
	const Matrix4f			*m_mTransform;

	sPersProj				m_sProjection;
	Matrix4f				m_mProjectionMatrix;
	Matrix4f				m_mViewMatrix;
};



#endif // __VXCAMERA_H
