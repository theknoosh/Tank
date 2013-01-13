#pragma once
#include <iostream>
#include <d3d9.h>
#include <d3dx9.h>
#include <D3DX9Math.h>
#include <vector>
#include <list>
#include "PlanePath.h"
#include "Projectile.h"
using namespace std;
typedef enum e_pdir { Left=-1,Right=1 } PlaneDir;
typedef enum e_Pitch { LeftNone,LeftUp,LeftDown,RightNone,RightUp,RightDown} Pitch;
class BiPlane
{
	IDirect3DDevice9*	m_pD3DDevCpy;
	IDirect3DTexture9*	m_pRightLevelTex;
	IDirect3DTexture9*	m_pLeftLevelTex;
	IDirect3DTexture9*	m_pRightUpTex;
	IDirect3DTexture9*	m_pLeftUpTex;
	IDirect3DTexture9*	m_pRightDownTex;
	IDirect3DTexture9*	m_pLeftDownTex;
	IDirect3DTexture9*	m_pPlaneTex;
	ID3DXSprite*		m_pPlaneSprite;
	Pitch		rotation;
	D3DXVECTOR3	scale;
	D3DXVECTOR3	bullet;
	D3DXVECTOR3	center;
	D3DXCOLOR	color;
	int angle;
	float speed;
	float yInc;
	float et;
	PlaneDir direction;
	int state;
	bool visible;
	Projectile *m_bullet;
	PlanePath *m_path;
	D3DXMATRIX matrix;
	D3DXMATRIX b_matrix;
public:
	BiPlane(void);
	~BiPlane(void);
	void Init(IDirect3DDevice9* newDDevice, ID3DXSprite* newSprite);
	float Update(D3DXVECTOR3 tankPos,float dt);
	void Spawn(void);
	void DeSpawn(void);
	bool IsVisible(void);
	bool Collision(D3DXVECTOR3 obj1, D3DXVECTOR3 obj2);
	void Update(float dt);
	//void Render(void);
	void Render(D3DXMATRIX *tankMat);
	int bulletsInBay;
	int bulletsInFlight;
	int bulletDelay;
	D3DXVECTOR3 pos;
	vector<Projectile* > TheBay;

};

