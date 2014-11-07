
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



// c_GLSLProgram CONSTRUCTOR, DESTRUCTOR /////////////////////////////////
//////////////////////////////////////////////////////////////////////////

c_GLSLProgram::c_GLSLProgram(std::string _vertex_shader, std::string _fragment_shader)
{
	Logw("c_GLSLProgram::c_GLSLProgram(): Creating program...\n");
	m_mapAttributes.clear();
	m_mapUniforms.clear();

	if (_vertex_shader != "" && _fragment_shader != "")
	{
		m_pShader = new c_ShaderObject();
		m_programID = m_pShader->CreateShaderProgram(_vertex_shader.c_str(), _fragment_shader.c_str());
	}
	else
	{
		m_pShader = NULL;
		Logw("c_GLSLProgram::c_GLSLProgram(): No shader sources. Exit.\n\n");
		CloseLogFile();
		exit(EXIT_FAILURE);
	}
	
} // end c_GLSLProgram::c_GLSLProgram()

//........................................................................
c_GLSLProgram::~c_GLSLProgram()
{
	if (m_pShader)
	{
		m_pShader->Release();
		m_pShader = NULL;
	}

} // end c_GLSLProgram::~c_GLSLProgram()




// c_GLSLProgram MEMBER FUNCTIONS ////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void c_GLSLProgram::AddVariable(int _type, std::string _name, std::string _variable)
{
	/*
	Adds a new variable to the program. Separate for uniforms and attributes,
	specified by the _type parameter, GLSL_ATTRIBUTE or GLSL_UNIFORM. The name
	is the <key> in the corresponding std::map and used for Get():ing the locations
	of attributes and uniforms, e.g. 'position' or 'attributePosition'. 
	
	The _variable parameter specifies the name of the variable as it is defined in 
	the GLSL code, e.g. 'a_vPosition' or 'u_VertexColor'. Each <key> corresponds to
	a GLSL_variable_location (struct) value, containing both the _variable parameter
	and the GLint of that variable as found by glGetAttributeLocation() and
	glGetUniformLocation().
	*/

	switch (_type)
	{
	case GLSL_ATTRIBUTE:
		m_mapAttributes[_name] = GLSL_variable_location(_variable, glGetAttribLocation(m_programID, _variable.c_str()));
		break;

	case GLSL_UNIFORM:
		m_mapUniforms[_name] = GLSL_variable_location(_variable, glGetUniformLocation(m_programID, _variable.c_str()));
		break;
	}

} // end c_GLSLProgram::AddVariable()

//........................................................................
GLSL_variable_location c_GLSLProgram::GetGLSLVariable(int _type, std::string _name)
{
	switch (_type)
	{
	case GLSL_ATTRIBUTE:
		return (m_mapAttributes[_name]);
		break;

	case GLSL_UNIFORM:
		return (m_mapUniforms[_name]);
		break;

	default:
		return (GLSL_variable_location("", -1));
	}

} // end c_GLSLProgram::GetGLSLVariable()

//........................................................................
GLint c_GLSLProgram::GetLocation(int _type, std::string _name)
{
	switch (_type)
	{
	case GLSL_ATTRIBUTE:
		return (m_mapAttributes[_name].iLocation);
		break;

	case GLSL_UNIFORM:
		return (m_mapUniforms[_name].iLocation);
		break;

	default:
		return (-1);
	}

} // end c_GLSLProgram::GetVariableLocation()

//........................................................................
void c_GLSLProgram::FinalizeProgram()
{
	// Iterate over all attributes and check for errors
	std::map<std::string, GLSL_variable_location>::iterator it;
	std::vector<std::string> attributeErrors;
	std::vector<std::string> uniformErrors;
	attributeErrors.clear();
	uniformErrors.clear();

	Logw("c_GLSLProgram::FinalizeProgram():\n");
	Logw("GLSL_ATTRIBUTE\n");
	for (it = m_mapAttributes.begin(); it != m_mapAttributes.end(); it++)
	{
		Logw("\t%s = %s (%d)\n",it->first.c_str(), it->second.sVariableName.c_str(), it->second.iLocation);
		if (it->second.iLocation == -1)
			attributeErrors.push_back(it->first);
	}

	Logw("\nGLSL_UNIFORM\n");
	for (it = m_mapUniforms.begin(); it != m_mapUniforms.end(); it++)
	{
		Logw("\t%s = %s (%d)\n", it->first.c_str(), it->second.sVariableName.c_str(), it->second.iLocation);
		if (it->second.iLocation == -1)
			uniformErrors.push_back(it->first);
	}
	
	Logw("\n");

	if (attributeErrors.size() > 0 ||
		uniformErrors.size() > 0)
	{
		Logw("c_GLSLProgram::FinalizeProgram(): Errors encoutered in attribute/uniform locations. Exit.\n\n");
		CloseLogFile();
		exit(EXIT_FAILURE);
	}
	else
		Logw("c_GLSLProgram::FinalizeProgram(): attribute/uniform locations found.\n");

} // end c_GLSLProgram::FinalizeProgram()



