#include "fonts.h"

// C_FONT CONSTRUCTOR / DESTRUCTOR ///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

c_Font::c_Font(const char *_filename, int _pixel_size, GLuint _program_id)
{
	// create and load shaders?
	if (_program_id == FONT_CREATE_SHADERS)
	{
		m_bCreateShaders = true;
		Logw("c_Font::c_Font: Compiling new shaders.\n");
	}
	else	// no: reuse the ones in the entered programID
	{
		m_fontProgramID = _program_id;
		m_bCreateShaders = false;
		Logw("c_Font::c_Font: Reusing shaders (id %d).\n", _program_id);
	}

	m_bLoaded = false;

	//InitializeFont(_filename, _pixel_size);
	InitializeAtlas(_filename, _pixel_size);

} // end c_Font::c_Font()

//////////////////////////////////////////////////////////////////////////
c_Font::~c_Font()
{
	//free(m_swizzleMask);
	if (m_pFontShaderObject)
		m_pFontShaderObject->Release();

	glDeleteTextures(1, &m_atlasTextureID);

} // end c_Font::~c_Font()


// C_FONT MEMBER FUNCTIONS ///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void c_Font::SetTextRenderingState()
{
	m_uniformTex = glGetUniformLocation(m_fontProgramID, "tex");
	m_uniformColor = glGetUniformLocation(m_fontProgramID, "color");
	m_attributeCoord = glGetAttribLocation(m_fontProgramID, "coord");

} // end c_Font::SetTextRenderingState()

//////////////////////////////////////////////////////////////////////////
int c_Font::InitializeAtlas(const char *_filename, int _pixel_size)
{
	__int64 t0 = StartCounter();

	m_iPixelSize = _pixel_size;

	// Load the text rendering shaders
	if (m_bCreateShaders)
	{
		m_pFontShaderObject = new c_ShaderObject();
		m_fontProgramID = m_pFontShaderObject->CreateShaderProgram("./GLSL/font_vertex.glsl",
																   "./GLSL/font_fragment.glsl");
	}
	else
		m_pFontShaderObject = NULL;
	
	// Use program for initiation of shader attributes
	glUseProgram(m_fontProgramID);
	SetTextRenderingState();

	if (m_attributeCoord == -1 || m_uniformTex == -1 || m_uniformColor == -1)
	{
		Logw("ERROR: c_Font::InitializeAtlas(): Could not find locations for attribute/uniform:\n");
		Logw("\t\tm_uniformTex = %d\n\t\tm_uniformColor = %d\n\t\tm_attributeCoord = %d\n",
			m_uniformTex, m_uniformColor, m_attributeCoord);
		return (RETURN_FAILURE);
	}

	// Init the FreeType lib
	Logw("c_Font::InitializeAtlas(): FreeType initialization\t\t");
	if (FT_Init_FreeType(&m_ftLib))
	{
		Logw("FAILURE\n\n");
		return (RETURN_FAILURE);
	}
	Logw("SUCCESS\n");
	
	// Load the face
	Logw("c_Font::InitializeAtlas(): Loading %s\t\t", _filename);
	if (FT_New_Face(m_ftLib, _filename, 0, &m_ftFace))
	{
		Logw("FAILURE\n\n");
		return (RETURN_FAILURE);
	}
	Logw("SUCCESS\n");

	// Initialize variables before atlas creation
	FT_Set_Pixel_Sizes(m_ftFace, 0, m_iPixelSize);
	FT_GlyphSlot g = m_ftFace->glyph;
	m_sx = 2.0f / glutGet(GLUT_WINDOW_WIDTH);
	m_sy = 2.0f / glutGet(GLUT_WINDOW_HEIGHT);

	int roww = 0;
	int rowh = 0;

	m_iTextureWidth = 0;
	m_iTextureHeight = 0;

	memset(m_sChars, 0, sizeof(character_info_s));

	// Find the minimum size for a texture holding the 
	// complete ASCII charset
	for (int i = 32; i < 128; i++)
	{
		if (FT_Load_Char(m_ftFace, i, FT_LOAD_RENDER))
		{
			Logw("ERROR: c_Font::InitializeAtlas(): Could not load char %d.\n", i);
			continue;
		}

		if (roww + g->bitmap.width + 1 >= 1024)
		{
			m_iTextureWidth = MAX(m_iTextureWidth, roww);
			m_iTextureHeight += rowh;
			roww = 0;
			rowh = 0;
		}

		roww += g->bitmap.width + 1;
		rowh = MAX(rowh, g->bitmap.rows);

	}

	m_iTextureWidth = MAX(m_iTextureWidth, roww);
	m_iTextureHeight += rowh;

	// Setup swizzle mask for alpha texture shader rendering
	m_swizzleMask = (GLint *)malloc(sizeof(GLint) * 4);
	GLint mask[4] = { GL_ZERO, GL_ZERO, GL_ZERO, GL_RED };
	memcpy(m_swizzleMask, mask, sizeof(GLint) * 4);

	// Create a texture to hold the character set
	glActiveTexture(GL_TEXTURE1);
	glGenTextures(1, &m_atlasTextureID);
	glBindTexture(GL_TEXTURE_2D, m_atlasTextureID);
	glUniform1i(m_uniformTex, 1);

	glTexImage2D(GL_TEXTURE_2D,
				 0,
				 GL_RGBA,
				 m_iTextureWidth,
				 m_iTextureHeight,
				 0,
				 GL_RED,
				 GL_UNSIGNED_BYTE,
				 0);

	// 1 byte alignment
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Clamping to edges and linear filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, m_swizzleMask);

	// Paste glyphs bitmaps into the texture
	//
	int ox = 0;
	int oy = 0;
	rowh = 0;

	for (int i = 32; i < 128; i++)
	{
		if (FT_Load_Char(m_ftFace, i, FT_LOAD_RENDER))
		{
			Logw("ERROR: c_Font::InitializeAtlas(): Could not load char bitmap into texture %d\n", i);
			continue;
		}

		if (ox + g->bitmap.width + 1 >=	1024)
		{
			oy += rowh;
			rowh = 0;
			ox = 0;
		}

		glTexSubImage2D(GL_TEXTURE_2D,
						0,
						ox,
						oy,
						g->bitmap.width,
						g->bitmap.rows,
						GL_RED,
						GL_UNSIGNED_BYTE,
						g->bitmap.buffer);

		m_sChars[i].ax = (float)(g->advance.x >> 6);
		m_sChars[i].ay = (float)(g->advance.y >> 6);
		m_sChars[i].bw = (float)g->bitmap.width;
		m_sChars[i].bh = (float)g->bitmap.rows;
		m_sChars[i].bl = (float)g->bitmap_left;
		m_sChars[i].bt = (float)g->bitmap_top;
		m_sChars[i].tx = ox / (float)m_iTextureWidth;
		m_sChars[i].ty = oy / (float)m_iTextureHeight;

		rowh = MAX(rowh, g->bitmap.rows);
		ox += g->bitmap.width + 1;

	}

	//glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, m_swizzleMask);

	// Generate VAO
	GLenum res = GL_NO_ERROR;
	glGenVertexArrays(1, &m_fontVAO);
	res = glGetError();
	if (res != GL_NO_ERROR)
		Logw("ERROR %d: c_Font::InitializeAtlas(): glGenVertexArrays: %s\n", res, gluErrorString(res));
	glBindVertexArray(m_fontVAO);

	// Generate the VBO for fonts
	res = GL_NO_ERROR;
	glGenBuffers(1, &m_fontVBO);
	res = glGetError();
	if (res != GL_NO_ERROR)
		Logw("ERROR %d: c_Font::InitializeAtlas(): glGenBuffers: %s\n", res, gluErrorString(res));

	// unbind vertex array
	glBindVertexArray(0);


	Logw("c_Font::InitializeAtlas(): Generated a %dx%d text atlas (%.1f kb) in %.4f ms\n\n",
		m_iTextureWidth, m_iTextureHeight, (float)(m_iTextureWidth * m_iTextureHeight) / 1024.0f,
		(double)(GetCounter()-t0)/PCFreq);


	return (RETURN_SUCCESS);

} // end c_Font::InitializeAtlas()

//////////////////////////////////////////////////////////////////////////
void c_Font::RenderString_s(float _x, float _y, const char *_str)
{
	const uint8_t *p;

	float x = -1 + _x * m_sx;
	float y = 1 - _y * m_sy;

	// Bind texture
	glBindTexture(GL_TEXTURE_2D, m_atlasTextureID);
	glUniform1i(m_uniformTex, 0);

	// Select the font VBO
	glBindVertexArray(m_fontVAO);
	glEnableVertexAttribArray(m_attributeCoord);
	glBindBuffer(GL_ARRAY_BUFFER, m_fontVBO);
	glVertexAttribPointer(m_attributeCoord, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	int c = 0;
	//memset(m_sTexCoords, 0, sizeof(font_point) * 256); 
	m_sTexCoords = new font_point[6 * strlen(_str)];

	// Loop through all characters
	for (p = (const uint8_t *)_str; *p; p++)
	{
		// calculate vertex and texture coordinates
		float x2 = x + m_sChars[*p].bl * m_sx;
		float y2 = -y - m_sChars[*p].bt * m_sy;
		float w = m_sChars[*p].bw * m_sx;
		float h = m_sChars[*p].bh * m_sy;

		// advance cursor
		x += m_sChars[*p].ax * m_sx;
		y += m_sChars[*p].ay * m_sy;

		// skip empty chars
		if (!w || !h)
			continue;
		
		m_sTexCoords[c+0].x = x2 + w;
		m_sTexCoords[c+0].y = -y2;
		m_sTexCoords[c+0].s =  m_sChars[*p].tx + m_sChars[*p].bw / m_iTextureWidth;
		m_sTexCoords[c+0].t = m_sChars[*p].ty;

		m_sTexCoords[c+1].x = x2;
		m_sTexCoords[c+1].y = -y2;
		m_sTexCoords[c+1].s = m_sChars[*p].tx;
		m_sTexCoords[c+1].t = m_sChars[*p].ty;

		m_sTexCoords[c+2].x = x2;
		m_sTexCoords[c+2].y = -y2 - h;
		m_sTexCoords[c+2].s = m_sChars[*p].tx;
		m_sTexCoords[c+2].t = m_sChars[*p].ty + m_sChars[*p].bh / m_iTextureHeight;

		m_sTexCoords[c+3].x = x2 + w; 
		m_sTexCoords[c+3].y = -y2;
		m_sTexCoords[c+3].s = m_sChars[*p].tx + m_sChars[*p].bw / m_iTextureWidth;
		m_sTexCoords[c+3].t = m_sChars[*p].ty;

		m_sTexCoords[c+4].x = x2; 
		m_sTexCoords[c+4].y = -y2 - h;
		m_sTexCoords[c+4].s = m_sChars[*p].tx;
		m_sTexCoords[c+4].t = m_sChars[*p].ty + m_sChars[*p].bh / m_iTextureHeight;

		m_sTexCoords[c+5].x = x2 + w;
		m_sTexCoords[c+5].y = -y2 - h;
		m_sTexCoords[c+5].s = m_sChars[*p].tx + m_sChars[*p].bw / m_iTextureWidth;
		m_sTexCoords[c+5].t = m_sChars[*p].ty + m_sChars[*p].bh / m_iTextureHeight;

		c += 6;

	}
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(font_point) * c, m_sTexCoords, GL_DYNAMIC_DRAW);
	glDrawArrays(GL_TRIANGLES, 0, c);

	glDisableVertexAttribArray(m_attributeCoord);
	glBindVertexArray(0);

	// free memory
	delete [] m_sTexCoords;

} // c_Font::RenderString_s()

//////////////////////////////////////////////////////////////////////////
void c_Font::RenderString_ss(float _x, float _y, char *_str, ...)
{
	// extract arguments
	char buffer[1024];
	memset(buffer, 0, 1024);

	va_list arglist;

	if (!_str)
		return;

	va_start(arglist, _str);
	vsprintf(buffer, _str, arglist);
	va_end(arglist);
	
	const uint8_t *p;

	float x = -1 + _x * m_sx;
	float y = 1 - _y * m_sy;

	// Bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_atlasTextureID);
	glUniform1i(m_uniformTex, 0);

	// Select the font VBO
	glBindVertexArray(m_fontVAO);
	glEnableVertexAttribArray(m_attributeCoord);
	glBindBuffer(GL_ARRAY_BUFFER, m_fontVBO);
	glVertexAttribPointer(m_attributeCoord, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	int c = 0;
	//memset(m_sTexCoords, 0, sizeof(font_point) * 256); 
	m_sTexCoords = new font_point[6 * strlen(buffer)];

	// Loop through all characters
	for (p = (const uint8_t *)buffer; *p; p++)
	{
		// calculate vertex and texture coordinates
		float x2 = x + m_sChars[*p].bl * m_sx;
		float y2 = -y - m_sChars[*p].bt * m_sy;
		float w = m_sChars[*p].bw * m_sx;
		float h = m_sChars[*p].bh * m_sy;

		// advance cursor
		x += m_sChars[*p].ax * m_sx;
		y += m_sChars[*p].ay * m_sy;

		// skip empty chars
		if (!w || !h)
			continue;
		
		m_sTexCoords[c+0].x = x2 + w;
		m_sTexCoords[c+0].y = -y2;
		m_sTexCoords[c+0].s =  m_sChars[*p].tx + m_sChars[*p].bw / m_iTextureWidth;
		m_sTexCoords[c+0].t = m_sChars[*p].ty;

		m_sTexCoords[c+1].x = x2;
		m_sTexCoords[c+1].y = -y2;
		m_sTexCoords[c+1].s = m_sChars[*p].tx;
		m_sTexCoords[c+1].t = m_sChars[*p].ty;

		m_sTexCoords[c+2].x = x2;
		m_sTexCoords[c+2].y = -y2 - h;
		m_sTexCoords[c+2].s = m_sChars[*p].tx;
		m_sTexCoords[c+2].t = m_sChars[*p].ty + m_sChars[*p].bh / m_iTextureHeight;

		m_sTexCoords[c+3].x = x2 + w; 
		m_sTexCoords[c+3].y = -y2;
		m_sTexCoords[c+3].s = m_sChars[*p].tx + m_sChars[*p].bw / m_iTextureWidth;
		m_sTexCoords[c+3].t = m_sChars[*p].ty;

		m_sTexCoords[c+4].x = x2; 
		m_sTexCoords[c+4].y = -y2 - h;
		m_sTexCoords[c+4].s = m_sChars[*p].tx;
		m_sTexCoords[c+4].t = m_sChars[*p].ty + m_sChars[*p].bh / m_iTextureHeight;

		m_sTexCoords[c+5].x = x2 + w;
		m_sTexCoords[c+5].y = -y2 - h;
		m_sTexCoords[c+5].s = m_sChars[*p].tx + m_sChars[*p].bw / m_iTextureWidth;
		m_sTexCoords[c+5].t = m_sChars[*p].ty + m_sChars[*p].bh / m_iTextureHeight;

		c += 6;

	}
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(font_point) * c, m_sTexCoords, GL_DYNAMIC_DRAW);
	glDrawArrays(GL_TRIANGLES, 0, c);

	glDisableVertexAttribArray(m_attributeCoord);
	glBindVertexArray(0);

	// free memory
	delete [] m_sTexCoords;

} // c_Font::RenderString_ss()

