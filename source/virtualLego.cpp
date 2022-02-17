////////////////////////////////////////////////////////////////////////////////
//
// File: virtualLego.cpp
//
// Original Author: 박창현 Chang-hyeon Park, 
// Modified by Bong-Soo Sohn and Dong-Jun Kim
// 
// Originally programmed for Virtual LEGO. 
// Modified later to program for Virtual Billiard.
//        
////////////////////////////////////////////////////////////////////////////////

#include "d3dUtility.h"
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <cassert>

#define M_RADIUS 0.21   // ball radius
#define PI 3.14159265
#define M_HEIGHT 0.01
#define DECREASE_RATE 0.9982


IDirect3DDevice9* Device = NULL;

// window size
const int Width  = 1024;
const int Height = 1024;

const int numOfYellowBalls = 54;
const int numOfWalls = 3;
int point = 0;
int lives = 5;
bool gameRunning = false;

// There are 54 balls
// initialize the position (coordinate) of each ball (ball0 ~ ball3)
const float spherePos[numOfYellowBalls][2] = {
	// mouth
	{-(float)M_RADIUS,0} , {(float)M_RADIUS, 0}, {-(float)M_RADIUS*3, 0}, {3.0* (float)M_RADIUS,0},
	{-5.0* (float)M_RADIUS, 2*(float)M_RADIUS}, {5.0 * (float)M_RADIUS, 2 * (float)M_RADIUS}, 
	{-7.0 * (float)M_RADIUS, 4 * (float)M_RADIUS}, {7.0 * (float)M_RADIUS, 4 * (float)M_RADIUS},

	// top edge
	{-7 * (float)M_RADIUS, -1} ,{-5*(float)M_RADIUS, -1} ,{-(float)M_RADIUS, -1} , {(float)M_RADIUS, -1}, 
	{-(float)M_RADIUS * 3, -1}, {3.0 * (float)M_RADIUS, -1}, {5 * (float)M_RADIUS, -1},{7 * (float)M_RADIUS, -1},
	{-9 * (float)M_RADIUS, -1 + 2 * (float)M_RADIUS}, {9 * (float)M_RADIUS, -1 + 2 * (float)M_RADIUS},
	{-11 * (float)M_RADIUS, -1 + 4 * (float)M_RADIUS}, {11 * (float)M_RADIUS, -1 + 4 * (float)M_RADIUS},

	// botton edge
	{-7 * (float)M_RADIUS, -1 + 26 * (float)M_RADIUS} ,{-5 * (float)M_RADIUS, -1 + 26 * (float)M_RADIUS} ,{-(float)M_RADIUS, -1 + 26 * (float)M_RADIUS} , {(float)M_RADIUS, -1 + 26 * (float)M_RADIUS},
	{-(float)M_RADIUS * 3, -1 + 26 * (float)M_RADIUS}, {3.0 * (float)M_RADIUS, -1 + 26 * (float)M_RADIUS}, {5 * (float)M_RADIUS, -1 + 26 * (float)M_RADIUS},{7 * (float)M_RADIUS, -1 + 26 * (float)M_RADIUS},
	{-9 * (float)M_RADIUS, -1 + 24 * (float)M_RADIUS}, {9 * (float)M_RADIUS, -1 + 24 * (float)M_RADIUS},

	// left edge
	{-11 * (float)M_RADIUS, -1 + 6 * (float)M_RADIUS},{-11 * (float)M_RADIUS, -1 + 8 * (float)M_RADIUS},{-11 * (float)M_RADIUS, -1 + 10 * (float)M_RADIUS},
	{-11 * (float)M_RADIUS, -1 + 12 * (float)M_RADIUS},{-11 * (float)M_RADIUS, -1 + 14 * (float)M_RADIUS},{-11 * (float)M_RADIUS, -1 + 16 * (float)M_RADIUS},
	{-11 * (float)M_RADIUS, -1 + 18 * (float)M_RADIUS},{-11 * (float)M_RADIUS, -1 + 20 * (float)M_RADIUS},{-11 * (float)M_RADIUS, -1 + 22 * (float)M_RADIUS},

	// right edge
	{11 * (float)M_RADIUS, -1 + 6 * (float)M_RADIUS},{11 * (float)M_RADIUS, -1 + 8 * (float)M_RADIUS},{11 * (float)M_RADIUS, -1 + 10 * (float)M_RADIUS},
	{11 * (float)M_RADIUS, -1 + 12 * (float)M_RADIUS},{11 * (float)M_RADIUS, -1 + 14 * (float)M_RADIUS},{11 * (float)M_RADIUS, -1 + 16 * (float)M_RADIUS},
	{11 * (float)M_RADIUS, -1 + 18 * (float)M_RADIUS},{11 * (float)M_RADIUS, -1 + 20 * (float)M_RADIUS},{11 * (float)M_RADIUS, -1 + 22 * (float)M_RADIUS},

	// left eye
	{-5 * (float)M_RADIUS, -1 + 18 * (float)M_RADIUS},{-5 * (float)M_RADIUS, -1 + 20 * (float)M_RADIUS},

	// right eye
	{5 * (float)M_RADIUS, -1 + 18 * (float)M_RADIUS},{5 * (float)M_RADIUS, -1 + 20 * (float)M_RADIUS},
	
	// nose
	{0,-1 + 14 * (float)M_RADIUS},{0,-1 + 12 * (float)M_RADIUS}
}; 
// initialize the color of each ball (ball0 ~ ball53)
//const D3DXCOLOR sphereColor = d3d::YELLOW;


// -----------------------------------------------------------------------------
// Transform matrices
// -----------------------------------------------------------------------------
D3DXMATRIX g_mWorld;
D3DXMATRIX g_mView;
D3DXMATRIX g_mProj;

// -----------------------------------------------------------------------------
// CSphere class definition
// -----------------------------------------------------------------------------
class CSphere {
private :
	float center_x, center_y, center_z;
    float m_radius;
	float m_velocity_x;
	float m_velocity_z;

public:
    CSphere(void)
    {
        D3DXMatrixIdentity(&m_mLocal);
        ZeroMemory(&m_mtrl, sizeof(m_mtrl));
        m_radius = 0;
		m_velocity_x = 0;
		m_velocity_z = 0;
        m_pSphereMesh = NULL;
    }
    ~CSphere(void) {}

public:
    bool create(IDirect3DDevice9* pDevice, D3DXCOLOR color = d3d::WHITE)
    {
        if (NULL == pDevice)
            return false;
		
        m_mtrl.Ambient  = color;
        m_mtrl.Diffuse  = color;
        m_mtrl.Specular = color;
        m_mtrl.Emissive = d3d::BLACK;
        m_mtrl.Power    = 5.0f;
		
        if (FAILED(D3DXCreateSphere(pDevice, getRadius(), 50, 50, &m_pSphereMesh, NULL)))
            return false;
        return true;
    }
	
    void destroy(void)
    {
        if (m_pSphereMesh != NULL) {
            m_pSphereMesh->Release();
            m_pSphereMesh = NULL;
        }
    }

    void draw(IDirect3DDevice9* pDevice, const D3DXMATRIX& mWorld)
    {
        if (NULL == pDevice)
            return;
        pDevice->SetTransform(D3DTS_WORLD, &mWorld);
        pDevice->MultiplyTransform(D3DTS_WORLD, &m_mLocal);
        pDevice->SetMaterial(&m_mtrl);
		m_pSphereMesh->DrawSubset(0);
    }
	
    bool hasIntersected(CSphere& ball) 
	{
		// Insert your code here.

		return false;
	}
	
	void hitBy(CSphere& ball, bool isYellow = true) 
	{ 
		D3DXVECTOR3 cord = this->getCenter();
		D3DXVECTOR3 cord2 = ball.getCenter();

		float dx = abs(cord.x - cord2.x);
		float dz = abs(cord.z - cord2.z);

		if (dx * dx + dz * dz < 4* (float)M_RADIUS * (float)M_RADIUS) {
			double theta = acos(sqrt(pow(cord.x - cord2.x, 2)) / sqrt(pow(cord.x - cord2.x, 2) + pow(cord.z - cord2.z, 2)));		// 기본 1 사분면
			if (cord.z - cord2.z <= 0 && cord.x - cord2.x >= 0) { theta = -theta; }	//4 사분면
			if (cord.z - cord2.z >= 0 && cord.x - cord2.x <= 0) { theta = PI - theta; } //2 사분면
			if (cord.z - cord2.z <= 0 && cord.x - cord2.x <= 0){ theta = PI + theta; } // 3 사분면
			//double distance = sqrt(pow(cord.x - cord2.x, 2) + pow(cord.z - cord2.z, 2));
			this->setPower(2 * cos(theta), 2 * sin(theta));

			if (isYellow) {
				ball.setCenter(-10.0f, 0.0f, 0.0f);
				point++;
			}
		}
	}

	void ballUpdate(float timeDiff) 
	{
		const float TIME_SCALE = 3.3;
		D3DXVECTOR3 cord = this->getCenter();
		double vx = abs(this->getVelocity_X());
		double vz = abs(this->getVelocity_Z());

		if(vx > 0.01 || vz > 0.01)
		{
			float tX = cord.x + TIME_SCALE*timeDiff*m_velocity_x;
			float tZ = cord.z + TIME_SCALE*timeDiff*m_velocity_z;

			// correction of position of ball
			// Please uncomment this part because this correction of ball position is necessary when a ball collides with a wall
			if(tX >= (4.56 - M_RADIUS))
				tX = 4.56 - M_RADIUS;
			else if(tX <=(-4.56 + M_RADIUS))
				tX = -4.56 + M_RADIUS;
			else if(tZ <= (-6.06 + M_RADIUS))
				tZ = -6.06 + M_RADIUS;
			else if(tZ >= (6.06 - M_RADIUS))
				tZ = 6.06 - M_RADIUS;
			
			this->setCenter(tX, 0.0f, tZ);
		}
		else { this->setPower(0,0);}
		//this->setPower(this->getVelocity_X() * DECREASE_RATE, this->getVelocity_Z() * DECREASE_RATE);
		//double rate = 1 -  (1 - DECREASE_RATE)*timeDiff * 400;
		//if(rate < 0 )
			//rate = 0;
		this->setPower(getVelocity_X(), getVelocity_Z());
	}

	double getVelocity_X() { return this->m_velocity_x;	}
	double getVelocity_Z() { return this->m_velocity_z; }

	void setPower(double vx, double vz)
	{
		this->m_velocity_x = vx;
		this->m_velocity_z = vz;
	}

	void setCenter(float x, float y, float z)
	{
		D3DXMATRIX m;
		center_x=x;	center_y=y;	center_z=z;
		D3DXMatrixTranslation(&m, x, y, z);
		setLocalTransform(m);
	}
	
	float getRadius(void)  const { return (float)(M_RADIUS);  }
    const D3DXMATRIX& getLocalTransform(void) const { return m_mLocal; }
    void setLocalTransform(const D3DXMATRIX& mLocal) { m_mLocal = mLocal; }
    D3DXVECTOR3 getCenter(void) const
    {
        D3DXVECTOR3 org(center_x, center_y, center_z);
        return org;
    }

	float getXPos() const
	{
		return this->center_x;
	}
	
private:
    D3DXMATRIX              m_mLocal;
    D3DMATERIAL9            m_mtrl;
    ID3DXMesh*              m_pSphereMesh;
	
};

// -----------------------------------------------------------------------------
// CWall class definition
// -----------------------------------------------------------------------------

class CWall {

private:	
    float					m_x;
	float					m_z;
	float                   m_width;
    float                   m_depth;
	float					m_height;
	
public:
    CWall(void)
    {
        D3DXMatrixIdentity(&m_mLocal);
        ZeroMemory(&m_mtrl, sizeof(m_mtrl));
        m_width = 0;
        m_depth = 0;
        m_pBoundMesh = NULL;
    }
    ~CWall(void) {}
public:
    bool create(IDirect3DDevice9* pDevice, float ix, float iz, float iwidth, float iheight, float idepth, D3DXCOLOR color = d3d::WHITE)
    {
        if (NULL == pDevice)
            return false;
		
        m_mtrl.Ambient  = color;
        m_mtrl.Diffuse  = color;
        m_mtrl.Specular = color;
        m_mtrl.Emissive = d3d::BLACK;
        m_mtrl.Power    = 5.0f;
		
        m_width = iwidth;
        m_depth = idepth;
		
        if (FAILED(D3DXCreateBox(pDevice, iwidth, iheight, idepth, &m_pBoundMesh, NULL)))
            return false;
        return true;
    }
    void destroy(void)
    {
        if (m_pBoundMesh != NULL) {
            m_pBoundMesh->Release();
            m_pBoundMesh = NULL;
        }
    }
    void draw(IDirect3DDevice9* pDevice, const D3DXMATRIX& mWorld)
    {
        if (NULL == pDevice)
            return;
        pDevice->SetTransform(D3DTS_WORLD, &mWorld);
        pDevice->MultiplyTransform(D3DTS_WORLD, &m_mLocal);
        pDevice->SetMaterial(&m_mtrl);
		m_pBoundMesh->DrawSubset(0);
    }
	
	bool hasIntersected(CSphere& ball) 
	{
		// Insert your code here.
		return false;
	}

	void hitBy(CSphere& ball) 
	{
		D3DXVECTOR3 cord = ball.getCenter();
		double vx = ball.getVelocity_X();
		double vz = ball.getVelocity_Z();

		if ((-4.56 + M_RADIUS) < cord.x && cord.x < (4.56 - M_RADIUS) && cord.z <= -6.05 + M_RADIUS) {	// bottom line
			OutputDebugStringW(L"gameover\n");
			ball.setPower(0, 0);
		}
		else if ((-4.56 + M_RADIUS) <= cord.x && cord.x <= (4.56 - M_RADIUS) && cord.z >= 6.05f - M_RADIUS) {	// top line
			ball.setPower(vx, -vz);
		}
		else if (-6.06 + M_RADIUS < cord.z && cord.z < 6.06f - M_RADIUS && cord.x >= (4.50 - M_RADIUS)) {	// right line
			ball.setPower(-vx, vz);
		}
		else if (-6.06 + M_RADIUS < cord.z && cord.z < 6.06f - M_RADIUS && cord.x <= (-4.50 + M_RADIUS)) {	// left line
			ball.setPower(-vx, vz);
		}
	}    
	
	void setPosition(float x, float y, float z)
	{
		D3DXMATRIX m;
		this->m_x = x;
		this->m_z = z;

		D3DXMatrixTranslation(&m, x, y, z);
		setLocalTransform(m);
	}
	
    float getHeight(void) const { return M_HEIGHT; }
	
	
	
private :
    void setLocalTransform(const D3DXMATRIX& mLocal) { m_mLocal = mLocal; }
	
	D3DXMATRIX              m_mLocal;
    D3DMATERIAL9            m_mtrl;
    ID3DXMesh*              m_pBoundMesh;
};

// -----------------------------------------------------------------------------
// CLight class definition
// -----------------------------------------------------------------------------

class CLight {
public:
    CLight(void)
    {
        static DWORD i = 0;
        m_index = i++;
        D3DXMatrixIdentity(&m_mLocal);
        ::ZeroMemory(&m_lit, sizeof(m_lit));
        m_pMesh = NULL;
        m_bound._center = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
        m_bound._radius = 0.0f;
    }
    ~CLight(void) {}
public:
    bool create(IDirect3DDevice9* pDevice, const D3DLIGHT9& lit, float radius = 0.1f)
    {
        if (NULL == pDevice)
            return false;
        if (FAILED(D3DXCreateSphere(pDevice, radius, 10, 10, &m_pMesh, NULL)))
            return false;
		
        m_bound._center = lit.Position;
        m_bound._radius = radius;
		
        m_lit.Type          = lit.Type;
        m_lit.Diffuse       = lit.Diffuse;
        m_lit.Specular      = lit.Specular;
        m_lit.Ambient       = lit.Ambient;
        m_lit.Position      = lit.Position;
        m_lit.Direction     = lit.Direction;
        m_lit.Range         = lit.Range;
        m_lit.Falloff       = lit.Falloff;
        m_lit.Attenuation0  = lit.Attenuation0;
        m_lit.Attenuation1  = lit.Attenuation1;
        m_lit.Attenuation2  = lit.Attenuation2;
        m_lit.Theta         = lit.Theta;
        m_lit.Phi           = lit.Phi;
        return true;
    }
    void destroy(void)
    {
        if (m_pMesh != NULL) {
            m_pMesh->Release();
            m_pMesh = NULL;
        }
    }
    bool setLight(IDirect3DDevice9* pDevice, const D3DXMATRIX& mWorld)
    {
        if (NULL == pDevice)
            return false;
		
        D3DXVECTOR3 pos(m_bound._center);
        D3DXVec3TransformCoord(&pos, &pos, &m_mLocal);
        D3DXVec3TransformCoord(&pos, &pos, &mWorld);
        m_lit.Position = pos;
		
        pDevice->SetLight(m_index, &m_lit);
        pDevice->LightEnable(m_index, TRUE);
        return true;
    }

    void draw(IDirect3DDevice9* pDevice)
    {
        if (NULL == pDevice)
            return;
        D3DXMATRIX m;
        D3DXMatrixTranslation(&m, m_lit.Position.x, m_lit.Position.y, m_lit.Position.z);
        pDevice->SetTransform(D3DTS_WORLD, &m);
        pDevice->SetMaterial(&d3d::WHITE_MTRL);
        m_pMesh->DrawSubset(0);
    }

    D3DXVECTOR3 getPosition(void) const { return D3DXVECTOR3(m_lit.Position); }

private:
    DWORD               m_index;
    D3DXMATRIX          m_mLocal;
    D3DLIGHT9           m_lit;
    ID3DXMesh*          m_pMesh;
    d3d::BoundingSphere m_bound;
};


// -----------------------------------------------------------------------------
// Global variables
// -----------------------------------------------------------------------------
CWall	g_legoPlane;
CWall	g_legowall[3];
CSphere	g_sphere[numOfYellowBalls];
CSphere g_player_whiteball;
CSphere	g_target_redball;
CLight	g_light;

double g_camera_pos[3] = {0.0, 5.0, -8.0};

// -----------------------------------------------------------------------------
// Functions
// -----------------------------------------------------------------------------


void destroyAllLegoBlock(void)
{
}

// initialization
bool Setup()
{
	int i;
	
    D3DXMatrixIdentity(&g_mWorld);
    D3DXMatrixIdentity(&g_mView);
    D3DXMatrixIdentity(&g_mProj);
		
	// create plane and set the position
    if (false == g_legoPlane.create(Device, -1, -1, 9, 0.03f, 12, d3d::GREEN)) return false;
    g_legoPlane.setPosition(0.0f, -0.0006f / 5, 0.0f);
	
	// create walls and set the position. note that there are four walls
	if (false == g_legowall[0].create(Device, -1, -1, 9, 0.3f, 0.12f, d3d::DARKRED)) return false;
	g_legowall[0].setPosition(0.0f, 0.12f, 6.06f);
	if (false == g_legowall[1].create(Device, -1, -1, 0.12f, 0.3f, 12.24f, d3d::DARKRED)) return false;
	g_legowall[1].setPosition(4.56f, 0.12f, 0.0f);
	if (false == g_legowall[2].create(Device, -1, -1, 0.12f, 0.3f, 12.24f, d3d::DARKRED)) return false;
	g_legowall[2].setPosition(-4.56f, 0.12f, 0.0f);


	/*if (false == g_legowall[1].create(Device, -1, -1, 9, 0.3f, 0.12f, d3d::DARKRED)) return false;
	g_legowall[1].setPosition(0.0f, 0.12f, -6.06f);*/

	// create 54 balls and set the position
	for (i=0;i<numOfYellowBalls;i++) {
		if (false == g_sphere[i].create(Device, d3d::YELLOW)) return false;
		g_sphere[i].setCenter(spherePos[i][0], (float)M_RADIUS , spherePos[i][1]);
		g_sphere[i].setPower(0,0);
	}
	
	// create red ball for set direction
    if (false == g_target_redball.create(Device, d3d::RED)) return false;
	g_target_redball.setCenter(.0f, 0.f, -5.3f);

	// create white ball for set direction
	if (false == g_player_whiteball.create(Device, d3d::WHITE)) return false;
	g_player_whiteball.setCenter(0.0f, 0.f, -5.8f);
	
	// light setting 
    D3DLIGHT9 lit;
    ::ZeroMemory(&lit, sizeof(lit));
    lit.Type         = D3DLIGHT_POINT;
    lit.Diffuse      = d3d::WHITE; 
	lit.Specular     = d3d::WHITE * 0.9f;
    lit.Ambient      = d3d::WHITE * 0.9f;
    lit.Position     = D3DXVECTOR3(0.0f, 3.0f, 0.0f);
    lit.Range        = 100.0f;
    lit.Attenuation0 = 0.0f;
    lit.Attenuation1 = 0.9f;
    lit.Attenuation2 = 0.0f;
    if (false == g_light.create(Device, lit))
        return false;
	
	// Position and aim the camera.
	D3DXVECTOR3 pos(0.0f, 15.0f, -10.0f);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 2.0f, 0.0f);
	D3DXMatrixLookAtLH(&g_mView, &pos, &target, &up);
	Device->SetTransform(D3DTS_VIEW, &g_mView);
	
	// Set the projection matrix.
	D3DXMatrixPerspectiveFovLH(&g_mProj, D3DX_PI / 4,
        (float)Width / (float)Height, 1.0f, 100.0f);
	Device->SetTransform(D3DTS_PROJECTION, &g_mProj);
	
    // Set render states.
    Device->SetRenderState(D3DRS_LIGHTING, TRUE);
    Device->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
    Device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
	
	g_light.setLight(Device, g_mWorld);
	return true;
}

void Cleanup(void)
{
    g_legoPlane.destroy();
	for(int i = 0 ; i < numOfWalls; i++) {
		g_legowall[i].destroy();
	}
    destroyAllLegoBlock();
    g_light.destroy();
}


// timeDelta represents the time between the current image frame and the last image frame.
// the distance of moving balls should be "velocity * timeDelta"
bool Display(float timeDelta)
{
	int i = 0;
	int j = 0;

	if( Device )
	{
		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00afafaf, 1.0f, 0);
		Device->BeginScene();
		
		// update the position of each ball. during update, check whether each ball hit by walls.
		g_target_redball.ballUpdate(timeDelta);
		for(j = 0; j < numOfYellowBalls; j++) {
			g_target_redball.hitBy(g_sphere[j]); 
		}
		g_target_redball.hitBy(g_player_whiteball, false);
		
		for (i = 0; i < numOfWalls; i++) {
			g_legowall[i].hitBy(g_target_redball);
		}


		// draw plane, walls, and spheres
		g_legoPlane.draw(Device, g_mWorld);
		for (i=0;i< numOfWalls;i++) 	{
			g_legowall[i].draw(Device, g_mWorld);
		}

		for (i = 0; i < numOfYellowBalls; i++) {
			g_sphere[i].draw(Device, g_mWorld);
		}

		g_player_whiteball.draw(Device, g_mWorld);
		g_target_redball.draw(Device, g_mWorld);
		
		Device->EndScene();
		Device->Present(0, 0, 0, 0);
		Device->SetTexture( 0, NULL );
	}

	return true;
}

LRESULT CALLBACK d3d::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static bool wire = false;
	static bool isReset = true;
    static int old_x = 0;
    static int old_y = 0;
    static enum { WORLD_MOVE, LIGHT_MOVE, BLOCK_MOVE } move = WORLD_MOVE;
	
	switch( msg ) {
	case WM_DESTROY:
        {
			::PostQuitMessage(0);
			break;
        }
	case WM_KEYDOWN:
        {
            switch (wParam) {
            case VK_ESCAPE:
				::DestroyWindow(hwnd);
                break;
            case VK_RETURN:
                if (NULL != Device) {
                    wire = !wire;
                    Device->SetRenderState(D3DRS_FILLMODE,
                        (wire ? D3DFILL_WIREFRAME : D3DFILL_SOLID));
                }
                break;
            case VK_SPACE:
				g_player_whiteball.setCenter(0.0f, 0.f, -5.8f);
				g_target_redball.setCenter(.0f, 0.f, -5.3f);
				g_target_redball.setPower(0.0f, 5.0f);
				gameRunning = true;
				break;

			}
			break;
        }

	case WM_MOUSEMOVE:        
        {
			int new_x = LOWORD(lParam);
			float dx;

			dx = (old_x - new_x);// * 0.01f;

			D3DXVECTOR3 coord3d = g_player_whiteball.getCenter();
			if (coord3d.x > 4.56f) {
				g_player_whiteball.setCenter(4.35f, 0.0f, -5.8f);
			} else if (coord3d.x < -4.56f) {
				g_player_whiteball.setCenter(-4.35f, 0.0f, -5.8f);
			}
			else {
				g_player_whiteball.setCenter(coord3d.x + dx * (-0.017f), 0.0f, -5.8f);
			}
			
			old_x = new_x;
        }
	}
	
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hinstance,
				   HINSTANCE prevInstance, 
				   PSTR cmdLine,
				   int showCmd)
{
    srand(static_cast<unsigned int>(time(NULL)));
	

	if(!d3d::InitD3D(hinstance,
		Width, Height, true, D3DDEVTYPE_HAL, &Device))
	{
		::MessageBox(0, "InitD3D() - FAILED", 0, 0);
		return 0;
	}
	
	if(!Setup())
	{
		::MessageBox(0, "Setup() - FAILED", 0, 0);
		return 0;
	}
	
	d3d::EnterMsgLoop( Display );
	
	Cleanup();
	
	Device->Release();
	
	return 0;
}