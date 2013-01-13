#pragma once
#include <iostream>
#include <vector>
#include <d3d9.h>
#include <d3dx9.h>
#include <D3DX9Math.h>

using std::vector;
typedef enum e_State {Used,InBay,InAir} theState;
class Projectile
{
	IDirect3DDevice9* m_pD3DDevCpy;
	IDirect3DTexture9*	m_projTex;
	ID3DXSprite* m_pProjectileSprite;
	D3DXVECTOR3 enemyLoc;
	D3DXMATRIX oldMat;
	bool contact;
public:
	theState m_state;
	D3DXVECTOR3 myLoc;
	Projectile(void);
	Projectile(IDirect3DDevice9* newDDevice);
	~Projectile(void);
	void Fire(void);
	void Fire(D3DXVECTOR3 gunLoc, D3DXVECTOR3 tankPos);
	void Init(IDirect3DDevice9* newDevice, ID3DXSprite* newSprite);
	void Target(D3DXVECTOR3 curLoc, D3DXVECTOR3 targetLoc);
	bool Update(float, float);
	void Render(D3DXMATRIX *previousMat);
};

