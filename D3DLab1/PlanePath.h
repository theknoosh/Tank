#pragma once
#include <iostream>
#include <d3d9.h>
#include <d3dx9.h>
#include <D3DX9Math.h>
typedef struct s_path {
	float Yfactor;
	int   PitchFrame;
} PathStep;
const int MaxNumSteps = 58;
const int Altitude = 440;

class PlanePath {
	PathStep AttackPatternAlpha[MaxNumSteps];
public:
	PlanePath();
	~PlanePath();
	void Init(float Dx);
	void Reset(float Dx);
	D3DXVECTOR3 GetPlaneLoc(int StepNo);
	D3DXVECTOR3 GetPlaneLoc(void);
	int m_step;
};
