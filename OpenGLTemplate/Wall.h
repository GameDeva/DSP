#pragma once

#include "Texture.h"
#include "VertexBufferObject.h"

// Class for generating a wall in the xy plane
class CWall
{
public:
	CWall();
	~CWall();
	void Create(string sFilename, float fWidth, float fHeight, glm::vec3 pos, glm::vec3 up, glm::vec3 fwd);
	void Render();
	void Release();

	float m_width;
	float m_height;

	glm::vec3 pos;
	glm::vec3 up;
	glm::vec3 fwd;

private:
	UINT m_vao;
	CVertexBufferObject m_vbo;
	CTexture m_texture;
	string m_filename;
	
};
