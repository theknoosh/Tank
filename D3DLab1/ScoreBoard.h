#pragma once
#include <windows.h>
#include <vector>
// basic file operations
#include <iostream>
#include <fstream>
#include <string>
using namespace std;
typedef struct s_ScoreEntry {
	wstring	PlayerName;
	int	score;
} ScoreEntry;
class ScoreBoard
{
	vector<ScoreEntry *> PlayerBoard;
	ScoreEntry PlayerEntry;
	wofstream myfile;
	//////////////////////////////////////////////////////////////////////////
	HWND				sb_hWnd;			// Handle to the window
public:
	ScoreBoard(void);
	~ScoreBoard(void);
	void Init(HWND& hWnd, HINSTANCE& hInst, bool bWindowed);
	void Init(HWND& hWnd);
	//void PostScore(float newScore);
	void PostScore(int newScore);
	void SetPlayerName(LPCWSTR newPlayer);
	void Flush(void);
	void ShowScoreTable(int MaxRows);
};
