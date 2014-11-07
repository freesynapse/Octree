
#include "object.h"
#include "vxtypes.h"
#include <algorithm>



// c_OGLObject CONSTRUCTOR ///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

c_OGLObject::c_OGLObject(c_GLSLProgram *_program, std::string _obj_file)
{
	m_sFileName = _obj_file;

	m_bObjLoaded = false;

	m_pProgram = _program;

	m_matRotation.Identity();
	m_matTranslation.Identity();
	m_matScale.Identity();

	m_vWorldPos = Vector3t<float>(0.0f, 0.0f, 0.0f);

	m_vVertices.clear();
	m_vNormals.clear();

	if (m_sFileName != "")
	{
		LoadBlenderObject(m_sFileName, m_vVertices, m_vNormals);
		m_bObjLoaded = true;
	}

	InitializeObject();

} // end c_OGLObject::c_OGLObject()



// c_OGLObject MEMBER FUNCTIONS //////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void c_OGLObject::InitializeObject()
{
	GLenum res;

	if (!m_bObjLoaded)
	{
		Logw("c_OGLObject::InitializeObject(): No object file loaded. Exit.\n\n");
		CloseLogFile();
		exit(EXIT_FAILURE);
	}

	// Setup the vertex array object
	res = GL_NO_ERROR;

	glGenVertexArrays(1, &m_vaoObject);
	res = glGetError();
	if (res != GL_NO_ERROR)
		Logw("c_OGLObject::InitializeObject(): ERROR %d: glGenVertexArrays vaoNodes: %s\n", res, gluErrorString(res));

	glBindVertexArray(m_vaoObject);

	
	//  Search for position and normal attributes in the GLSL code
	std::map<std::string, GLSL_variable_location> attributes = m_pProgram->Attributes();
	std::map<std::string, GLSL_variable_location>::iterator it;
	m_attributePosition = -1;
	m_attributeNormal = -1;

	for (it = attributes.begin(); it != attributes.end(); it++)
	{
		std::string name = it->first;
		std::transform(name.begin(), name.end(), name.begin(), ::tolower);	// convert variable name to lower case

		if (name.find("position") != std::string::npos)
			m_attributePosition = it->second.iLocation;
		else if (name.find("normal") != std::string::npos)
			m_attributeNormal = it->second.iLocation;
	}



	// Setup the vertex buffer object for vertices
	if (m_attributePosition >= 0)
	{
		res = GL_NO_ERROR;

		glGenBuffers(1, &m_vboObjectVertices);
		res = glGetError();
		if (res != GL_NO_ERROR)
			Logw("c_OGLObject::InitializeObject(): ERROR %d: glGenBuffers m_vboObjectVertices: %s\n", res, gluErrorString(res));

		glBindBuffer(GL_ARRAY_BUFFER, m_vboObjectVertices);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3t<float>) * m_vVertices.size(), &m_vVertices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(m_attributePosition);
		glVertexAttribPointer(m_attributePosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	}

	if (m_attributeNormal >= 0)
	{
		res = GL_NO_ERROR;

		glGenBuffers(1, &m_vboObjectNormals);
		res = glGetError();
		if (res != GL_NO_ERROR)
			Logw("c_OGLObject::InitializeObject(): ERROR %d: glGenBuffers m_vboObjectNormals: %s\n", res, gluErrorString(res));

		glBindBuffer(GL_ARRAY_BUFFER, m_vboObjectNormals);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3t<float>) * m_vNormals.size(), &m_vNormals[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(m_attributeNormal);
		glVertexAttribPointer(m_attributeNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	}

	if (m_attributePosition >= 0)	glDisableVertexAttribArray(m_vboObjectVertices);
	if (m_attributeNormal >= 0)		glDisableVertexAttribArray(m_vboObjectNormals);
	glBindVertexArray(0);


	// Find the location of the model matrix in the shader
	std::map<std::string, GLSL_variable_location> uniforms = m_pProgram->Uniforms();
	m_uniformModelMatrix = -1;

	for (it = uniforms.begin(); it != uniforms.end(); it++)
	{
		std::string name = it->first;
		std::transform(name.begin(), name.end(), name.begin(), ::tolower);	// convert variable name to lower case

		if (name.find("model") != std::string::npos)
			m_uniformModelMatrix = it->second.iLocation;
	}


	// Store the number of vertices in the object
	m_nVertices = (int)m_vVertices.size();


} // end c_OGLObject::InitializeObject()

//........................................................................
void c_OGLObject::LoadObjectOBJ(std::string _filename)
{
	LoadBlenderObject(_filename, m_vVertices, m_vNormals);

} // end c_OGLObject::LoadObjectOBJ()

//........................................................................
void c_OGLObject::Render()
{
	Matrix4f matModel;
	matModel.Identity();
	matModel.TranslationTransform(m_vWorldPos.x, m_vWorldPos.y, m_vWorldPos.z);

	glUniformMatrix4fv(m_uniformModelMatrix, 1, GL_TRUE, (const GLfloat *)&matModel);

	glBindVertexArray(m_vaoObject);

	if (m_attributePosition >= 0)	glEnableVertexAttribArray(m_attributePosition);
	if (m_attributeNormal >= 0)		glEnableVertexAttribArray(m_attributeNormal);

	glDrawArrays(GL_TRIANGLES, 0, m_nVertices);

	glBindVertexArray(0);


} // end c_OGLObject::Render()





// LOAD BLENDER OBJJ FILES ///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void LoadBlenderObject(std::string _filename,
					   std::vector<Vector3t<float>> &_vertices,
					   std::vector<Vector3t<float>> &_normals)
{
	std::ifstream infile;
	infile.open(_filename.c_str(), std::ios::in);

	Logw("LoadBlenderObject(): Loading .OBJ file '%s'.\n", _filename.c_str());

	if (!infile)
	{
		Logw("ERROR: LoadBlenderObject(): file '%s' not found. Exit.\n", _filename.c_str());
		exit(-1);
	}

	std::string line;
	std::vector<Index3ui> vElements;
	std::vector<Vector3t<float>> vVertices;
	unsigned int nlines = 0;

	while (std::getline(infile, line))
	{
		if (line.substr(0, 2) == "v ")
		{
			// vertex found
			std::istringstream s(line.substr(2));
			Vector3t<float> v;
			s >> v.x;
			s >> v.y;
			s >> v.z;
			vVertices.push_back(v);
		}
		else if (line.substr(0, 2) == "f ")
		{
			// element found
			std::istringstream s(line.substr(2));
			unsigned int a, b, c;
			s >> a;
			s >> b;
			s >> c;
			vElements.push_back(Index3ui(a, b, c));
		}
		else if (line[0] == '#')
		{
			// comment: do nothing
		}
		else
		{
			// blank line: do nothing
		}

		nlines++;
	}
	Logw("Read %d lines, reformatting data and computing normals:\n", nlines);

	// get rid of the element buffer and compute normals
	for (size_t i = 0; i < vElements.size(); i++)
	{
		Vector3t<float> v0, v1, v2;
		v0.x = vVertices[vElements[i].v0 - 1].x;
		v0.y = vVertices[vElements[i].v0 - 1].y;
		v0.z = vVertices[vElements[i].v0 - 1].z;

		v1.x = vVertices[vElements[i].v1 - 1].x;
		v1.y = vVertices[vElements[i].v1 - 1].y;
		v1.z = vVertices[vElements[i].v1 - 1].z;

		v2.x = vVertices[vElements[i].v2 - 1].x;
		v2.y = vVertices[vElements[i].v2 - 1].y;
		v2.z = vVertices[vElements[i].v2 - 1].z;

		_vertices.push_back(v2);
		_vertices.push_back(v1);
		_vertices.push_back(v0);

		Vector3t<float> n = Cross((v1 - v2), (v0 - v2));
		n.Normalize();
		_normals.push_back(n);
		_normals.push_back(n);
		_normals.push_back(n);

	}

	// number of vertices (i.e. Vector3t<float> structs)
	Logw("\tnumber of vertices\t%d\n\tnumber of normals\t%d\n\n", _vertices.size(), _normals.size());

} // end LoadBlenderObject()





