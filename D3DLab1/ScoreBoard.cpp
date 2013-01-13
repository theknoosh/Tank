#include "ScoreBoard.h"

string FileName = "ScoreBoard.txt";

ScoreBoard::ScoreBoard(void)
{
	int i;
	wchar_t Player[80],Score[80];
	bool have_coma;
	int strIndex;
	ScoreEntry *NewEntry;
	wstring InLine;
	wifstream infile;
	infile.open ("ScoreBoard.txt");
	infile.seekg(0,ios::beg);
	//infile.getline(InLine,80);
	while(getline(infile,InLine)) {
		strIndex = 0;
		have_coma = false;
		for(i=0;i<InLine.length()+1;++i) {
			if(have_coma) {
				Score[strIndex] = InLine[i];
			} else {
				Player[strIndex] = InLine[i];
			}
			if(InLine[i] == L',') {
				Player[strIndex] = L'\0';
				have_coma = true;
				strIndex = 0;
			} else {
				++strIndex;
			}
		}
		NewEntry = new ScoreEntry;
		NewEntry->PlayerName = Player;
		//wcscpy(NewEntry->PlayerName,Player);
		NewEntry->score = _wtoi(Score);
		PlayerBoard.push_back(NewEntry);
	}
	infile.close();
}

ScoreBoard::~ScoreBoard(void)
{
}
//void ScoreBoard::PostScore(float newScore)
//{	PlayerEntry.score = newScore;
//}
void ScoreBoard::PostScore(int newScore)
{
	PlayerEntry.score = static_cast<float>(newScore);
}
void ScoreBoard::SetPlayerName(LPCWSTR newPlayer)
{
	PlayerEntry.PlayerName =  newPlayer;
}
void ScoreBoard::Flush(void)
{
	wofstream outfile;
	char Name[80];
	
	HWND hwnd_ed_1;
	int PlayerCount;
	int	index;
	
	ScoreEntry *Score = new ScoreEntry;

	Score->score = PlayerEntry.score;
	Score->PlayerName = PlayerEntry.PlayerName;
	PlayerBoard.push_back(Score);
	// char array to hold the text from the textbox
	PlayerCount = PlayerBoard.size();
	outfile.open(FileName.c_str());
	for(index=0;index<PlayerCount;index++) {
		Score = PlayerBoard[index];
		
		//wcstombs(Name,Score->PlayerName,80);
		outfile << Score->PlayerName << "," << Score->score << "\n";
	}
	outfile.flush();
	outfile.close();
}
void ScoreBoard::Init(HWND& hWnd)
{
	sb_hWnd = hWnd;
}
void ScoreBoard::Init(HWND& hWnd, HINSTANCE& hInst, bool bWindowed)
{
	sb_hWnd = hWnd;
}
void ScoreBoard::ShowScoreTable(int MaxRows)
{
	char	playersScore[10];
	wchar_t	printableScore[10];	
	wstring ScoreList;
	//ScoreList = new LPCWSTR(1024);
	int PlayerCount;
	int i;
	PlayerCount = PlayerBoard.size();
	if(PlayerCount > 0) {
		for(i=0;i<PlayerCount;++i) {
			ScoreList += PlayerBoard[i]->PlayerName;
			ScoreList += L"\t --- \t";
			sprintf(playersScore,"%06d",PlayerBoard[i]->score);
			mbstowcs(printableScore, playersScore, 10);
			ScoreList += printableScore;
			ScoreList += L"\n";
		}
	} else {
		//sprintf(ScoreList,"No Scores Yet\n");
		ScoreList = L"No Scores Yet\n";
	}
	int msgboxID = MessageBox(
		NULL,
        (LPCWSTR)ScoreList.c_str(),
        (LPCWSTR)L"Score Board",
        MB_ICONINFORMATION | MB_OK
    );
}