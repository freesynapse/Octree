
#include "shader.h"
#include <fstream>
#include <gl/glew.h>


// SHADER CONSTRUCTOR ////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
c_ShaderObject::c_ShaderObject(void)
{
	m_bLoaded = false;

} // end c_ShaderObject::c_ShaderObject()

// SHADER CLASS MEMBER FUNCTIONS /////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

GLuint c_ShaderObject::LoadShader(const char *_shader_src, 
								 GLenum _shader_type)
{
	std::ifstream in(_shader_src);
	std::string src = "";
	std::string line = "";

	while(std::getline(in, line))
	{
		src += line + "\n";
		if (_shader_type == GL_VERTEX_SHADER)
			m_sVertexShaderSrc.push_back(line);
		else
			m_sFragmentShaderSrc.push_back(line);
	}

	GLuint shader;
	shader = glCreateShader(_shader_type);

	const char *source = src.c_str();
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);

	if(!shader)
	{
		GLsizei len;
		glGetProgramInfoLog(m_programID, sizeof(m_strError), &len, m_strError);
		Logw("ERROR: c_ShaderObject::LoadShader(): Could not compile shader of type %d\n\n[SOURCE]\n%s\n[/SOURCE]\n\n", _shader_type, src.c_str());
		Logw("ERROR:\n%s\n\n", m_strError);
		return (RETURN_FAILURE);
	}
	Logw("c_ShaderObject::LoadShader(): Shader %d compiled\n", _shader_type);
	

	return (shader);

} // end LoadShader()

//////////////////////////////////////////////////////////////////////////
GLuint c_ShaderObject::CreateShaderProgram(const char *_vertex_shader_path,
										   const char *_fragment_shader_path)
{
	GLint linked;

	//m_shaders[0] = LoadShader("vertex.glsl", GL_VERTEX_SHADER);
	//m_shaders[1] = LoadShader("fragment.glsl", GL_FRAGMENT_SHADER);
	
	m_shaders[0] = LoadShader(_vertex_shader_path, GL_VERTEX_SHADER);
	m_shaders[1] = LoadShader(_fragment_shader_path, GL_FRAGMENT_SHADER);

	if (!m_shaders[0] || !m_shaders[1])
		return (RETURN_FAILURE);

	m_programID = glCreateProgram();
	if (!m_programID)
	{
		GLsizei len;
		glGetProgramInfoLog(m_programID, sizeof(m_strError), &len, m_strError);
		Logw("ERROR: c_ShaderObject::CreateShaderProgram(): Could not create shader program (%d)\n", m_programID);
		Logw("ERROR:\n%s\n\n", m_strError);
		return (RETURN_FAILURE);
	}
	Logw("c_ShaderObject::CreateShaderProgram(): Created shader program with id %d\n", m_programID);

	glAttachShader(m_programID, m_shaders[0]);
	glAttachShader(m_programID, m_shaders[1]);

	//glBindAttribLocation(m_programID, 0, "vPosition");
	//glBindAttribLocation(m_programID, 1, "vColor");

	//glProgramParameteri(m_programID, GL_PROGRAM_SEPARABLE, GL_TRUE);
	glLinkProgram(m_programID);
	glGetProgramiv(m_programID, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		GLsizei len;
		glGetProgramInfoLog(m_programID, sizeof(m_strError), &len, m_strError);
		Logw("ERROR: c_ShaderObject::CreateShaderProgram(): Could not link program (%d)\n\t%s\n\t%s\n\n", 
			m_programID, _vertex_shader_path, _fragment_shader_path);
		Logw("ERROR:\n%s\n\n", m_strError);
		return (RETURN_FAILURE);
	}
	Logw("c_ShaderObject::CreateShaderProgram(): Program (%d) linked\n\t%s\n\t%s\n\n", 
		m_programID, _vertex_shader_path, _fragment_shader_path);
	

	//glUseProgram(m_programID);

	// success
	m_bLoaded = true;
	return (m_programID);

} // end CreateShaderProgram()

//////////////////////////////////////////////////////////////////////////
GLvoid c_ShaderObject::Release(void)
{
	if (m_bLoaded)
	{
		Logw("c_ShaderObject::Release(): program %d; shaders %d, %d\t\t", 
			m_programID, m_shaders[0], m_shaders[1]);

		GLenum res = GL_NO_ERROR;

		glDetachShader(m_programID, m_shaders[0]);
		glDetachShader(m_programID, m_shaders[1]);
		glDeleteShader(m_shaders[0]);
		glDeleteShader(m_shaders[1]);

		glUseProgram(0);
		glDeleteProgram(m_programID);

		res = glGetError();
		if (res != GL_NO_ERROR)
		{
			Logw("FAILURE\n");
			Logw("ERROR: c_ShaderObject::Release(): %s\n", gluErrorString(res)); 
		}
		else
			Logw("SUCCESS\n");
	}
	else
		Logw("WARNING: c_ShaderObject::Release(): No program loaded.\n");


} // end cShaderObject()

