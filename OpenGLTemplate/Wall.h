#pragma once

#include "Texture.h"
#include "VertexBufferObject.h"

// Class for generating a wall in the xy plane
class CWall
{
public:
	CWall();
	~CWall();
	void Create(string sFilename, float fWidth, float fHeight);
	void Render();
	void Release();
private:
	UINT m_vao;
	CVertexBufferObject m_vbo;
	CTexture m_texture;
	string m_filename;
	float m_width;
	float m_height;
};
