
#include "camera.h"
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/quaternion.hpp>


// c_Transform MEMBER FUNCTIONS //////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void c_Transform::Identity(int matrix_flag)
{
	m_mTransform[matrix_flag].Identity();

} // end c_Transform::Identity()

//////////////////////////////////////////////////////////////////////////
Matrix4f *c_Transform::TransformCamMatrix(Vector3f cam_pos,
										  Vector3f U, 
										  Vector3f V, 
										  Vector3f N)
{
	Matrix4f cam_transl_trans, 
			 cam_rot_trans, 
			 pers_proj_trans;

	m_mTransform[MATRIX_TRANSFORM_CAM].Identity();
	cam_transl_trans.TranslationTransform(-cam_pos.x, -cam_pos.y, -cam_pos.z);
	cam_rot_trans.CameraTransform(U, V, N);
	pers_proj_trans.PersProjTransform(m_persProj.fFOV, m_persProj.fAspectRatio, m_persProj.fZNear, m_persProj.fZFar);
	
	m_mTransform[MATRIX_TRANSFORM_CAM] = pers_proj_trans * cam_rot_trans * cam_transl_trans;

	return (&m_mTransform[MATRIX_TRANSFORM_CAM]);

} // end c_Transform::GetTransformed()

//////////////////////////////////////////////////////////////////////////
Matrix4f *c_Transform::TransformObjMatrix(void)
{
	Matrix4f scale_trans, 
			 rot_trans, 
			 transl_trans;

	m_mTransform[MATRIX_TRANSFORM_OBJ].Identity();
	scale_trans.ScaleTransform(m_vScale.x, m_vScale.y, m_vScale.z);
	rot_trans.RotateTransform(m_vRotate.x, m_vRotate.y, m_vRotate.z);
	transl_trans.TranslationTransform(m_vWorldPos.x, m_vWorldPos.y, m_vWorldPos.z);

	m_mTransform[MATRIX_TRANSFORM_OBJ] = transl_trans * rot_trans * scale_trans;

	return (&m_mTransform[MATRIX_TRANSFORM_OBJ]);


} // end c_Transform::GetTransformedObj()

//////////////////////////////////////////////////////////////////////////
void c_Transform::SetShaderMatrix(int matrix_flag, 
								  GLint matrix_handle)
/*
 * matrix_flag either MATRIX_TRANSFORM_OBJ=0 or
 *					  MATRIX_TRANSFORM_CAM=1
 */
{
	glUniformMatrix4fv(matrix_handle, 1, GL_TRUE, (const GLfloat *)&m_mTransform[matrix_flag]);

} // end c_Transform::SetShaderMatrix()

//////////////////////////////////////////////////////////////////////////
void c_Transform::PrintMatrix(int matrix_flag, const char *str)
{
	m_mTransform[matrix_flag].Print(str);

} // end c_Transform::PrintMatrix()


// c_Camera CONSTRUCTOR/DESTRUCTOR ///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

c_Camera::c_Camera(GLuint &_shader_program)
{
	// store handle to shader program and transformation matrix
	m_programID = _shader_program;
	m_vsCamTransform = glGetUniformLocation(m_programID, "m_camera_transform");

	// create the transformation object
	m_pTransform = new c_Transform();

	m_vPos		= Vector3f();
	m_vTarget	= Vector3f(0.0f, 0.0f, 1.0f);
	m_vUp		= Vector3f(0.0f, 1.0f, 0.0f);

	// Set default move speed / direction
	m_fMoveSpeed	= 0.1f;
	m_fHeading		= 270.0f;
	m_fElevation	= 90.0f;

	// Set the vertex shader object transformation matrix to identity.
	// This is only done once in c_Camera, since it is initiated before
	// all objects.
	GLint obj_matrix_handle = glGetUniformLocation(m_programID, "m_object_transform");
	m_pTransform->Identity(MATRIX_TRANSFORM_OBJ);

} // end c_Camera::c_Camera()

//////////////////////////////////////////////////////////////////////////
c_Camera::~c_Camera()
{
	delete m_pTransform;
	m_pTransform = NULL;

} // end c_Camera::~c_Camera()


// c_Camera MEMBER FUNCTIONS /////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void c_Camera::SetCamera(Vector3f &_pos,
						 Vector3f &_target,
						 Vector3f &_up)
{	
	m_vPos		= _pos;
	m_vTarget	= _target;
	m_vUp		= _up;

} // end c_Camera::SetCamera()

//////////////////////////////////////////////////////////////////////////
void c_Camera::MoveCamera(int _direction)
{
	if (_direction & CAMERA_MOVE_FORWARD)	m_vPos		+= m_f  * m_fMoveSpeed;
	if (_direction & CAMERA_MOVE_BACKWARDS)	m_vPos		-= m_f  * m_fMoveSpeed;
	if (_direction & CAMERA_MOVE_LEFT)		m_vPos		-= m_u  * m_fMoveSpeed;
	if (_direction & CAMERA_MOVE_RIGHT)		m_vPos		+= m_u  * m_fMoveSpeed;
	if (_direction & CAMERA_MOVE_UP)		m_vPos.y	+= 1.0f * m_fMoveSpeed;
	if (_direction & CAMERA_MOVE_DOWN)		m_vPos.y	-= 1.0f * m_fMoveSpeed;

	Update();

} // end c_Camera::MoveCamera()

//////////////////////////////////////////////////////////////////////////
void c_Camera::SetTarget(float _heading, float _elevation)
{
	m_fHeading		+= _heading;
	m_fElevation	-= _elevation;

	// update with new angles
	Update();

} // end c_Camera::DeltaTarget()

//////////////////////////////////////////////////////////////////////////
void c_Camera::PerspectiveProjection(float _fov,
									 float _ar,
									 float _znear,
									 float _zfar)
{
	m_sProjection.fFOV = _fov;
	m_sProjection.fAspectRatio = _ar;
	m_sProjection.fZNear = _znear;
	m_sProjection.fZFar = _zfar;

	m_pTransform->SetPerspectiveProj(_fov, _ar, _znear, _zfar);

} // end c_Camera::PerspectiveProjection()

//////////////////////////////////////////////////////////////////////////
void c_Camera::Update(void)
{
	// clamp / overflow heading/elevation
	if (m_fElevation > 179.9f)	m_fElevation = 179.9f;
	if (m_fElevation < 0.1f)	m_fElevation = 0.1f;
	if (m_fHeading > 360.0f)	m_fHeading -= 360.0f;
	if (m_fHeading < 0.0f)		m_fHeading += 360.0f;

	// build the target vector based in rotation angles
	float theta		= DEG_TO_RAD(m_fHeading);
	float phi		= DEG_TO_RAD(m_fElevation);
	float radius	= sinf(phi);

	//Logw("heading: %.1f   elevation: %.1f", m_fHeading, m_fElevation);
	m_n.x = -radius * cosf(theta);
	m_n.y = cosf(phi);
	m_n.z = -radius * sinf(theta);
	//m_n.Print("\tN: ");

	// Build UVN and F
	// N: target vector
	m_vTarget = m_n;
	m_n.Normalize();

	// V: up vector
	m_v = Vector3f(0.0f, 1.0f, 0.0f);

	// U: right vector = N x V
	m_u = m_v.Cross(m_n);
	
	// compute new V
	m_v = m_n.Cross(m_u);

	// F: forward vector = V x U
	m_f = m_u.Cross(Vector3f(0.0f, 1.0f, 0.0f));

	// normalize U, V and F
	m_u.Normalize();
	m_v.Normalize();
	m_f.Normalize();

	// Update the transformation matrix
	//m_pTransform->TransformCamMatrix(m_vPos, m_u, m_v, m_n);
	
	// Update the transformation matrix in the vertex shader (through the c_Transform class)
	//m_pTransform->SetShaderMatrix(MATRIX_TRANSFORM_CAM, m_vsCamTransform);
	
	//glUniformMatrix4fv(m_uniformTransform, 1, GL_TRUE, (const GLfloat *)m_mTransform);

} // end c_Camera::Update()



// Accessors for matrices ////////////////////////////////////////////////

Matrix4f c_Camera::ViewMatrix()
{
	m_mViewMatrix.Identity();

	Matrix4f mCameraTranslation, mCameraRotation;
	mCameraTranslation.TranslationTransform(-m_vPos.x, -m_vPos.y, -m_vPos.z);
	mCameraRotation.CameraTransform(m_u, m_v, m_n);

	m_mViewMatrix = mCameraRotation * mCameraTranslation;

	return (m_mViewMatrix);

} // end c_Camera::ViewMatrix()

//////////////////////////////////////////////////////////////////////////
Matrix4f c_Camera::ProjectionMatrix()
{
	m_mProjectionMatrix.Identity();
	m_mProjectionMatrix.PersProjTransform(m_sProjection.fFOV, 
										  m_sProjection.fAspectRatio, 
										  m_sProjection.fZNear, 
										  m_sProjection.fZFar);

	return (m_mProjectionMatrix);
	

} // end c_Camera::ProjectionMatrix()