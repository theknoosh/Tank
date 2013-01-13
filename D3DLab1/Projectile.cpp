#include "Projectile.h"


Projectile::Projectile(void)
{
	m_state = InBay;
}


Projectile::~Projectile(void)
{
}
void
Projectile::Init(IDirect3DDevice9* newDevice, ID3DXSprite* newSprite)
{
	HRESULT hr;
	HRESULT lhRc;
	TCHAR *ErrorMessage;
	m_pD3DDevCpy = newDevice;
	//D3DXCreateSprite(m_pD3DDevCpy, &m_pPlaneSprite);
	m_pProjectileSprite = newSprite;
	hr = D3DXCreateTextureFromFile(m_pD3DDevCpy, L"bullet.png", &m_projTex);
}
void
Projectile::Target(D3DXVECTOR3 curLoc, D3DXVECTOR3 targetLoc)
{
	myLoc = curLoc;
	enemyLoc = targetLoc;
}
void
Projectile::Fire(void)
{
	//Render();
}
void
Projectile::Fire(D3DXVECTOR3 gunLoc, D3DXVECTOR3 targetLoc)
{
	if ( m_state == InBay) { 
		m_state = InAir;
		myLoc.x = gunLoc.x + 100;
		myLoc.y = gunLoc.y + 64;
		myLoc.z = gunLoc.z;
		enemyLoc = targetLoc;
	}
	//Render();
}
const float TeraFirma = 500.0f;

// TODO: Modify code here to find contact with tank
bool Projectile::Update(float tankX, float tankY)
{
	contact = false; // default to no contact
	if(m_state == InAir) {
		if(myLoc.y > tankY && myLoc.x > tankX && myLoc.x < (tankX + 186)){
			contact = true;
			m_state = Used;
		}
		myLoc.x += 20.0f;
		myLoc.y += 20.0f;
		if(myLoc.y > TeraFirma) {
			m_state = Used;
		}
	}
	return contact;
}
void
Projectile::Render(D3DXMATRIX *previousMat)
{
	//oldMat = previousMat;
	if(m_state == InAir) {
		m_pProjectileSprite->Draw(m_projTex,NULL,NULL,&myLoc,D3DCOLOR_XRGB(255,255,255));
		/*m_pProjectileSprite->SetTransform(previousMat);*/
	}
}