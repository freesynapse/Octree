#ifndef __FONTS_H
#define __FONTS_H


#pragma warning(disable : 4005)


// INCLUSIONS ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include <ft2build.h>
#include FT_FREETYPE_H

#include "vxtypes.h"
#include "utility.h"
#include "shader.h"


// DEFINITIONS ///////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#define FONT_CREATE_SHADERS							-1



// TYPEDEFS / ENUMERATIONS /STRUCTS //////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

struct font_point
{
	GLfloat x;
	GLfloat y;
	GLfloat s;
	GLfloat t;
};

struct font_point_s
{
	GLfloat x;
	GLfloat y;
	GLfloat s;
	GLfloat t;

	font_point_s(float _x, float _y, float _s, float _t) : x(_x), y(_y), s(_s), t(_t) {}
	font_point_s() {}
};

struct character_info_s
{
	float ax, ay;
	float bw, bh;
	float bl, bt;
	float tx, ty;

};


// CLASSES ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

// Font library class ....................................................
class c_Font
{
public:
	// Constructor / destructor
	c_Font(const char *_filename, int _pixel_size=12, GLuint _program_id=FONT_CREATE_SHADERS);
	~c_Font();

	// Accessors
	GLuint					fontTextureID()			{	return (m_fontTextureID);			}
	GLuint					fontProgramID()			{	return (m_fontProgramID);			}
	GLuint					attributeCoord()		{	return (m_attributeCoord);			}
	GLuint					uniformTex()			{	return (m_uniformTex);				}
	GLuint					uniformColor()			{	return (m_uniformColor);			}

	FT_Face					FontFace()				{	return (m_ftFace);					}
	FT_Library				FontLib()				{	return (m_ftLib);					}
	
	float					sx()					{	return (m_sx);						}
	float					sy()					{	return (m_sy);						}
	void					set_sx(float _sx)		{	m_sx = _sx;							}
	void					set_sy(float _sy)		{	m_sy = _sy;							}

	int						FontHeight()			{	return (m_iTextureHeight);			}

	void SetColor(rgba4f *color)
	{	glUniform4fv(m_uniformColor, 1, (GLfloat *)color);	}

	c_ShaderObject			*pFontShader()			{	return (m_pFontShaderObject);		}


	// Member functions
	void					SetTextRenderingState();
	int						InitializeAtlas(const char *_filename, int _pixel_size);
	void					RenderString_s(float _x, float _y, const char *_str);
	void					RenderString_ss(float _x, float _y, char *_str, ...);

private:
	// .FreeType
	FT_Library				m_ftLib;
	FT_Face					m_ftFace;
	FT_GlyphSlot			m_glyph;

	// .texture
	GLuint					m_fontTextureID;
	GLuint					m_atlasTextureID;
	int						m_iTextureWidth;
	int						m_iTextureHeight;
	GLint					*m_swizzleMask;

	// .atlas
	character_info_s		m_sChars[128];
	font_point				*m_sTexCoords;

	// .vbo
	GLuint					m_fontVAO;
	GLuint					m_fontVBO;

	// .shaders
	c_ShaderObject			*m_pFontShaderObject;
	GLuint					m_fontProgramID;

	// .shader parameter locations
	GLuint					m_uniformTex;
	GLuint					m_uniformColor;
	GLuint					m_attributeCoord;

	// .text rendering attributes
	int						m_iPixelSize;
	float					m_sx;
	float					m_sy;
	bool					m_bLoaded;
	bool					m_bCreateShaders;

};

#endif // __FONTS_H
