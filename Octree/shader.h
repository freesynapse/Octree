#ifndef __SHADER_H
#define __SHADER_H


#pragma warning(disable : 4005)


// INCLUSIONS ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include "utility.h"



// DEFINITIONS ///////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#define GLSL_ATTRIBUTE						0x01
#define GLSL_UNIFORM						0x02



// STRUCTURES / TYPE DEFINITIONS /////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

struct GLSL_variable_location
{
	std::string sVariableName;
	GLint iLocation;

	GLSL_variable_location() :
		sVariableName(""), iLocation(-1) {}
	GLSL_variable_location(std::string _variable_name, GLint _location) :
		sVariableName(_variable_name), iLocation(_location) {}
	
};



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


// GLSL Program class ....................................................
class c_GLSLProgram
{
public:
	// Constructor
	c_GLSLProgram(std::string _vertex_shader, std::string _fragment_shader);
	~c_GLSLProgram();

	// Accessors / Member functions
	GLuint ProgramID()											{	return (m_programID);		}
	std::map<std::string, GLSL_variable_location> Attributes()	{	return (m_mapAttributes);	}
	std::map<std::string, GLSL_variable_location> Uniforms()	{	return (m_mapUniforms);		}

	void AddVariable(int _type, std::string _name, std::string _variable);
	
	GLSL_variable_location GetGLSLVariable(int _type, std::string _name);
	GLint GetLocation(int _type, std::string _name);

	void FinalizeProgram();


private:
	GLuint m_programID;

	c_ShaderObject *m_pShader;

	// Mapping of attributes and uniforms separately. The std::string <key> 
	// corresponds to the name of the variable, e.g. 'attributePosition' or
	// 'normals'. The value of the corresponding key contains both the name
	// of the GLSL variable, e.g. 'a_vPosition' and the location of that variable
	// as located using glGetAttribLocation() and glGetUniformLocation().
	std::map<std::string, GLSL_variable_location> m_mapAttributes;
	std::map<std::string, GLSL_variable_location> m_mapUniforms;

};



#endif // __SHADER_H
