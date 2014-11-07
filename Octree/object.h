#ifndef __OBJECT_H
#define __OBJECT_H



// INCLUSIONS ////////////////////////////////////////////////////////////

#include "utility.h"
#include "shader.h"


// DEFINITIONS / MACROS //////////////////////////////////////////////////



// CLASSES ///////////////////////////////////////////////////////////////

class c_OGLObject
{
public:
	// CONSTRUCTOR
	c_OGLObject(c_GLSLProgram *_program, std::string _obj_file);

	// ACCESSORS
	Matrix4f &GetMatrixRotation() { return (m_matRotation); }
	Matrix4f &GetMatrixTranslation() { return (m_matTranslation); }
	Matrix4f &GetMatrixScale() { return (m_matScale); }

	Vector3t<float> &GetWorldPos() { return (m_vWorldPos); }
	void SetWorldPos(Vector3t<float> _v) { m_vWorldPos = _v; }

	std::vector<Vector3t<float>> GetVertices()	{ return (m_vVertices); }
	std::vector<Vector3t<float>> GetNormals() { return (m_vNormals); }

	GLuint GetVAO() { return (m_vaoObject); }
	//GLuint GetVBO() { return (m_vboObject); }

	// MEMBER FUNCTIONS
	void InitializeObject();
	void LoadObjectOBJ(std::string _filename);
	void Render();
	

private:
	std::string m_sFileName;
	bool m_bObjLoaded;

	c_GLSLProgram *m_pProgram;

	GLuint m_vaoObject;
	GLuint m_vboObjectVertices;
	GLuint m_vboObjectNormals;

	Matrix4f m_matRotation;
	Matrix4f m_matTranslation;
	Matrix4f m_matScale;

	Vector3t<float> m_vWorldPos;

	std::vector<Vector3t<float>> m_vVertices;
	int m_nVertices;
	std::vector<Vector3t<float>> m_vNormals;

	GLint m_attributePosition;
	GLint m_attributeNormal;
	GLint m_uniformModelMatrix;

};



// FUNCTION PROTOTYPES ///////////////////////////////////////////////////

void LoadBlenderObject(std::string _filename,
					   std::vector<Vector3t<float> > &_vertices,
					   std::vector<Vector3t<float> > &_normals);





#endif // __OBJECT_H


