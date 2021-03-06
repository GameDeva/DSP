#pragma once

#include "Common.h"
#include "GameWindow.h"

// Classes used in game.  For a new class, declare it here and provide a pointer to an object of this class below.  Then, in Game.cpp, 
// include the header.  In the Game constructor, set the pointer to NULL and in Game::Initialise, create a new object.  Don't forget to 
// delete the object in the destructor.   
class CCamera;
class CSkybox;
class CShader;
class CShaderProgram;
class CPlane;
class CFreeTypeFont;
class CHighResolutionTimer;
class CSphere;
class COpenAssetImportMesh;
class CAudio;
class CWall;

class Game {
private:
	// Three main methods used in the game.  Initialise runs once, while Update and Render run repeatedly in the game loop.
	void Initialise();
	void Update();
	void Render();

	// Pointers to game objects.  They will get allocated in Game::Initialise()
	CSkybox *m_pSkybox;
	CCamera *m_pCamera;
	vector <CShaderProgram *> *m_pShaderPrograms;
	CPlane *m_pPlanarTerrain;
	CFreeTypeFont *m_pFtFont;
	COpenAssetImportMesh *m_pBarrelMesh;
	COpenAssetImportMesh *m_pHorseMesh;
	CSphere *m_pSphere;
	CHighResolutionTimer *m_pHighResolutionTimer;
	CAudio *m_pAudio;
	CWall *m_pWall;

	// Some other member variables
	double m_dtSeconds;
	double deltaTime;
	int m_framesPerSecond;
	bool m_appActive;


public:
	Game();
	~Game();
	static Game& GetInstance();
	LRESULT ProcessEvents(HWND window,UINT message, WPARAM w_param, LPARAM l_param);
	void SetHinstance(HINSTANCE hinstance);
	WPARAM Execute();

private:
	static const int FPS = 60;
	void DisplayFrameRate();
	void GameLoop();
	GameWindow m_gameWindow;
	HINSTANCE m_hInstance;
	int m_frameCount;
	double m_elapsedTime;

	float m_wallHeight;
	float m_wallWidth;
	glm::vec3 m_wallPos;

	float currentTimer;
	const float maxTimer = 1.5f;

	float controlTimer;
	const float maxControlTimer = 1.5f;

	bool toggleMode;
	bool toggleManualControl;

	// Sound cwk
	float shiftFilterValue = 0.0f;
	const float filterChangeSpeed = .4f;
	float currentFilterValue = 0.0f;

	void UpdateFilterValue(bool up);

	// Horse Values
	vector<glm::vec3> movePoints;
	int currentDestinationPoint;
	glm::vec3 moveDirection;
	float moveSpeed;
	float gameElapsedTime;

	glm::vec3 horsePosition;

	vector<CWall> walls;
	
};
