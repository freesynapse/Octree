#ifndef __OBJECT_H
#define __OBJECT_H



// INCLUSIONS ////////////////////////////////////////////////////////////

#include "utility.h"


// DEFINITIONS / MACROS //////////////////////////////////////////////////




// CLASSES ///////////////////////////////////////////////////////////////

class c_OGLObject
{
public:
	// constructor 
	c_OGLObject(GLint _glsl_program, std::string _obj_file="");

	// accessors
	std::vector<Vector3f> GetVertices()	{ return (m_vVertices); }
	std::vector<Vector3f> GetNormals() { return (m_vNormals); }
	GLint GetVAO() { return (m_vaoObject); }
	GLint GetVBO() { return (m_vboObject); }

private:
	GLint m_vaoObject;
	GLint m_vboObject;

	std::vector<Vector3f> m_vVertices;
	std::vector<Vector3f> m_vNormals;
	
};




#endif // __OBJECT_H