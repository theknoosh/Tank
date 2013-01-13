#include "BiPlane.h"
#include <string>
#include <DxErr.h>

//D3DXVECTOR2 m_lbullet[] = {D3DXVECTOR2(pos.x,pos.y),D3DXVECTOR2(350, 350)};

BiPlane::BiPlane(void)
{
	pos = D3DXVECTOR3(0,050,0);
	speed = 1.0f;
	et = 0.0f;
	direction = Right;
	visible = false;
	bulletsInBay = 10;
}

BiPlane::~BiPlane(void)
{
	//m_bullet->Release();
}
void
BiPlane::Init(IDirect3DDevice9* newDevice, ID3DXSprite* newSprite)
{
	int i;
	HRESULT hr;
	HRESULT lhRc;
	TCHAR *ErrorMessage;
	m_pD3DDevCpy = newDevice;
	//D3DXCreateSprite(m_pD3DDevCpy, &m_pPlaneSprite);
	m_pPlaneSprite = newSprite;
	for(i=0;i<bulletsInBay;i++) {
		//m_path = new PlanePath;
		//m_path->Init(600);
		m_bullet = new Projectile;
		m_bullet->m_state = InBay;
		m_bullet->Init(m_pD3DDevCpy, m_pPlaneSprite);
		TheBay.push_back(m_bullet);
	}
	hr = D3DXCreateTextureFromFile(m_pD3DDevCpy, L"PlaneRightLevel2.png", &m_pRightLevelTex);
	//hr = D3DXCreateTextureFromFile(m_pD3DDevCpy, L"Stuka_ls.png", &m_pLeftLevelTex);
	//hr = D3DXCreateTextureFromFile(m_pD3DDevCpy, L"stuka_rx75.png", &m_pRightUpTex);
	//hr = D3DXCreateTextureFromFile(m_pD3DDevCpy, L"stuka_lx75.png", &m_pLeftDownTex);
	//hr = D3DXCreateTextureFromFile(m_pD3DDevCpy, L"stuka_rx75.png", &m_pRightDownTex);
	//hr = D3DXCreateTextureFromFile(m_pD3DDevCpy, L"stuka_lx75.png", &m_pLeftDownTex);
	if(hr != D3D_OK) {
		ErrorMessage = (TCHAR *)DXGetErrorString(hr);
	}
	m_pPlaneTex = m_pRightLevelTex;
	rotation = RightNone;
	direction = Right;
	yInc = 0.0f;
	bulletsInFlight = 0;
	bulletDelay = 5;
}
void BiPlane::Spawn(void)
{
	visible = true;
}
void BiPlane::DeSpawn(void)
{
	visible = false;
}

bool BiPlane::IsVisible(void)
{
	return visible;
}

bool BiPlane::Collision(D3DXVECTOR3 obj1, D3DXVECTOR3 obj2)
{
	float _proximity = 5.0;
	float _dx = 0.0f;
	float _dy = 0.0f;
	bool _collide = false;
	_dx = abs(obj1.x - obj2.x);
	_dy = abs(obj1.y - obj2.y);
	_collide =  ((_dx < _proximity) && (_dy < _proximity));
	return _collide;
}

// Texture being used is 1357 by 512:
//D3DXVECTOR2 spriteCentre=D3DXVECTOR2(55.0f,26.0f);
D3DXVECTOR2 spriteCentre=D3DXVECTOR2(120.0f,64.0f);
//D3DXVECTOR2 scaling(1.0f,1.0f);
float m_rotation;


float BiPlane::Update(D3DXVECTOR3 tankPos,float dt)
{
	float _damage = 0.0;
	static float m_dt = 0.0f;
	static float m_bdt = 0.0f;
	D3DXVECTOR3 posIncr = D3DXVECTOR3(6.0f,0.0f,0);
	if(visible) {
		m_dt += dt;
		m_bdt += dt;

		// 15 frames per second
		//if(m_dt > 0.330f) {
		//	m_dt = 0.0;
		//	return _damage;
		//}
		//int baySize = TheBay.size();
		//if(bulletsInFlight < baySize ) {
		//	if(m_bdt > 0.900f) {
		//		int i;
		//		for(i=0;i<baySize;++i) {
		//			m_bullet =  TheBay[i];
		//			if(m_bullet->m_state == InBay) {
		//				m_bullet->Fire(pos,tankPos);
		//				m_bullet->m_state = InAir;
		//				++bulletsInFlight;
		//				break;
		//			}

		//		}
		//		m_bdt = 0.0f;
		//	}
		//} else {
		//	int mbcnt = TheBay.size();
		//	int i;
		//	for(i=0;i<mbcnt;++i) {
		//		TheBay[i]->Update();
		//		if(TheBay[i]->m_state == Used) {
		//			m_bullet =  TheBay[i];
		//			m_bullet->m_state = InBay;
		//			--bulletsInFlight;
		//		}
		//	}
		//}
		// Rotate based on the position
		if(pos.x > 100 && pos.x < 250) rotation = LeftDown;
		if(pos.x > 250 && pos.x < 400) rotation = LeftNone;
		if(pos.x > 400 && pos.x < 700) rotation = LeftUp;
		
		// Plane has flown off the screen to the right
		if(pos.x > 700) {
			rotation = LeftNone; 
			pos = D3DXVECTOR3(0.0f,40.0f,0.0f);
			visible = false; // Once off screen make plane not visible
		}
		// Adjust the pitch of the plane
		switch(rotation) {
			case LeftNone:	m_rotation=0.01f; posIncr.y = 0.0f; break;
			case LeftUp:	m_rotation=0.00f; posIncr.y = -12.0f; break; 
			case LeftDown:	m_rotation=0.50f; posIncr.y = 12.0f; break;
			default:		m_rotation=0.01f; posIncr.y = 0.0f; break;
		}
		pos = pos + posIncr; 

		//// Screen position of the sprite
		//D3DXVECTOR2 trans=D3DXVECTOR2(pos.x,pos.y);
		//// out, scaling centre, scaling rotation, scaling, rotation centre, rotation, translation
		///*D3DXMatrixTransformation2D(&matrix,NULL,0.0,&scaling,&spriteCentre,m_rotation,&trans);*/
		//D3DXMatrixTransformation2D(&matrix,NULL,0.0,NULL,&spriteCentre,m_rotation,&trans);
	}
	return(_damage);
}
void BiPlane::Update(float dt)
{
	D3DXVECTOR3 newPath = D3DXVECTOR3(100,40,0);
	static int nextStep = 1;
	static float m_dt = 0.0f;
	if(visible) {
		m_dt += dt;
		if(m_dt > 0.00125f) {
			newPath = m_path->GetPlaneLoc(1);
			pos = newPath;
			m_dt = 0.0f;
		}
	}
}
void
BiPlane::Render(D3DXMATRIX *tankMat)
{

	if(visible) {
		//int mbcnt = TheBay.size();
		//int i;
		//for(i=0;i<mbcnt;++i) {
		//	m_bullet = TheBay[i];
		//	if(m_bullet->m_state == InAir) {
		//		m_bullet->Render(tankMat);
		//	}
		//}
		//m_pPlaneSprite->Begin(D3DXSPRITE_ALPHABLEND);
		//m_pPlaneSprite->SetTransform(&matrix);
		m_pPlaneSprite->Draw(m_pPlaneTex,NULL,NULL,&pos,D3DCOLOR_XRGB(255,255,255));

		//m_pPlaneSprite->SetTransform(tankMat);
		//m_pPlaneSprite->End();
	}
}