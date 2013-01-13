#include "PlanePath.h"
//double AttackPatternAlpha1[29] = {1.0, 0.974927912, 0.900968868, 0.781831482, 0.623489802, 0.433883739, 0.222520934, 1.22515E-16,
//-0.222520934, -0.433883739, -0.623489802, -0.781831482, -0.900968868, -0.974927912, -1, -0.974927912, -0.900968868, -0.781831482,
//-0.623489802, -0.433883739, -0.222520934, 0, 0.222520934, 0.433883739, 0.623489802, 0.781831482, 0.900968868, 0.974927912, 1.0};
//
//PathStep AttackPatternAlpha[60] = {
//	 1.00000f,0,
//	 0.97492f,0,
//	 0.90096f,0,
//	 0.78183f,0,
//	 0.62348f,0,
//	 0.43388f,0,
//	 0.22252f,0,
//	 0.00000f,0,
//	-0.22252f,0,
//	-0.43388f,0,
//	-0.62348f,0,
//	-0.78183f,0,
//	-0.90096f,0,
//	-0.97492f,0,
//	-1.0f,0,
//	-0.97492f,0,
//	-0.90096f,0,
//	-0.78183f,0,
//	-0.62348f,0,
//	-0.43388f,0,
//	-0.22252f,0,
//	 0.00000f,0,
//	 0.22252f,0,
//	 0.43388f,0,
//	 0.62348f,0,
//	 0.78183f,0,
//	 0.90096f,0,
//	 0.97492f,0,
//	 1.00000f,0
//};
float Start	= 0.3;
float Length =	200;
float Deltax = 	10;
float Deltay = 	10;
PathStep P1[MaxNumSteps] = {
1.955336489,0,
1.921060994,0,
1.877582562,0,
1.825335615,0,
1.764842187,0,
1.696706709,0,
1.621609968,0,
1.540302306,0,
1.453596121,0,
1.362357754,0,
1.267498829,0,
1.169967143,0,
1.070737202,0,
0.970800478,0,
0.871155506,0,
0.772797905,0,
0.676710433,0,
0.583853163,0,
0.495153895,0,
0.411498883,0,
0.333723979,0,
0.262606284,0,
0.198856384,0,
0.143111247,0,
0.095927858,0,
0.057777659,0,
0.029041835,0,
0.010007503,0,
0.00086485,0,
0.001705224,0,
0.01252023,0,
0.033201807,0,
0.063543313,0,
0.103241584,0,
0.151899968,0,
0.209032288,0,
0.274067696,0,
0.346356379,0,
0.425176053,0,
0.509739179,0,
0.599200828,0,
0.69266713,0,
0.789204201,0,
0.887847473,0,
0.987611337,0,
1.087498983,0,
1.186512369,0,
1.283662185,0,
1.377977743,0,
1.468516671,0,
1.554374336,0,
1.634692876,0,
1.708669774,0,
1.775565879,0,
1.834712785,0,
1.885519517,0,
1.927478431,0,
1.960170287,0,
};
PlanePath::PlanePath()
{
}
PlanePath::~PlanePath()
{
}
void
PlanePath::Init(float Dx)
{
	int i;
	Deltax = Dx / MaxNumSteps;
	for(i=0;i<MaxNumSteps;++i) {
		AttackPatternAlpha[i].PitchFrame = P1[i].PitchFrame;
		AttackPatternAlpha[i].Yfactor = P1[i].Yfactor;
	}
	m_step = 1;
}
void
PlanePath::Reset(float Dx)
{
	m_step = 1;
	Deltax = Dx / MaxNumSteps;
}
//D3DXVECTOR3
//PlanePath::GetPlaneLoc(int StepNo)
//{
//	D3DXVECTOR3 pRc;
//	pRc.x = ((StepNo - 1) * Deltax) + 100;
//	pRc.y = Altitude - (AttackPatternAlpha[StepNo - 1].Yfactor * Length);
//	pRc.z = 0;
//	return pRc;
//}
D3DXVECTOR3
PlanePath::GetPlaneLoc(void)
{
	D3DXVECTOR3 pRc;
	static int stepCount = -1;
	static float DeltaY = 1.0f;
	float y1,y2;

	y1 = Altitude - (AttackPatternAlpha[m_step - 1].Yfactor * Length);
	y2 = Altitude - (AttackPatternAlpha[m_step].Yfactor * Length);
	if(stepCount == -1) stepCount = y2 - y1;
	if(stepCount == 0) {
		++m_step > MaxNumSteps? m_step = 1: m_step = m_step;
		y1 = y2;
		y2 = Altitude - (AttackPatternAlpha[m_step].Yfactor * Length);
		stepCount = y2 - y1;
	}
	pRc.x = ((m_step - 1) * Deltax) + 100;
	pRc.y = y1 + DeltaY;
	pRc.z = 0;
	--stepCount;
	if(y1 > 300 ) {
		/*y1=y1;*/
		y1=300;
	}
	return pRc;
}
const float StartAngle = 0.3f;
const float AngleIncrement = 0.1f;
const float Ceiling = 440.0f;
const float m_length = 200.0f;
D3DXVECTOR3
PlanePath::GetPlaneLoc(int StepNo)
{
	D3DXVECTOR3 pRc;
	static double m_angle = StartAngle;
	static float m_x = 100;
	double m_f;
	double m_y;
	double m_lenght = 200;
	m_f = cos(m_angle) + 1.0f;
	m_y = Ceiling - (m_f * m_length);
	m_x += 1.0f;
	pRc.x = 100 + 1;
	pRc.y = m_y;
	pRc.z = 0;
	//static int stepCount = -1;
	//static float DeltaY = 1.0f;
	//float y1,y2;

	//y1 = Altitude - (AttackPatternAlpha[m_step - 1].Yfactor * Length);
	//y2 = Altitude - (AttackPatternAlpha[m_step].Yfactor * Length);
	//if(stepCount == -1) stepCount = y2 - y1;
	//if(stepCount == 0) {
	//	++m_step > MaxNumSteps? m_step = 1: m_step = m_step;
	//	y1 = y2;
	//	y2 = Altitude - (AttackPatternAlpha[m_step].Yfactor * Length);
	//	stepCount = y2 - y1;
	//}
	//pRc.x = ((m_step - 1) * Deltax) + 100;
	//pRc.y = y1 + DeltaY;
	//pRc.z = 0;
	//--stepCount;
	//if(y1 > 300 ) {
	//	/*y1=y1;*/
	//	y1=300;
	//}
	return pRc;
}