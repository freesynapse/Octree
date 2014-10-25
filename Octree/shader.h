#ifndef __SHADER_H
#define __SHADER_H


#pragma warning(disable : 4005)


// INCLUSIONS ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include "utility.h"


// DEFINITIONS ///////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


// CLASSES ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

// Shader setup class ....................................................
class c_ShaderObject
{
public:
	// Constructor
	c_ShaderObject();

	// Accessors
	GLuint					ProgramID(void)			{	return (m_programID);			}
	GLuint					ShaderID(int _index)	{	return (m_shaders[_index]);		}
	bool					Loaded(void)			{	return (m_bLoaded);				}

	std::vector<std::string> &VertexShaderSrc()		{	return (m_sVertexShaderSrc);	}
	std::vector<std::string> &FragmentShaderSrc()	{	return (m_sFragmentShaderSrc);	}

	// Member functions
	GLuint					LoadShader				(const char * _shader_src, 
													 GLenum _shader_type);
	GLuint					CreateShaderProgram		(const char *_vertex_shader_path,
													 const char *_fragment_shader_path);
	GLvoid					Release					(void);

private:
	// Member variables
	GLuint					m_programID;
	GLuint					m_shaders[2];

	std::vector<std::string>	m_sVertexShaderSrc;
	std::vector<std::string>	m_sFragmentShaderSrc;

	bool					m_bLoaded;
	
	char					m_strError[4096];

};




#endif // __SHADER_H
