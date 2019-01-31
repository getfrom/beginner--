#pragma once

#ifndef _STDLIB_H_
#define _STDLIB_H_
#include <stdlib.h>
#endif

#ifndef _TIME_H_
#define _TIME_H_
#include <time.h>
#endif

#ifndef _STRSAFE_H_
#define _STRSAFE_H_
#include <strsafe.h>
#pragma comment(lib,"strsafe.lib")
#endif

#ifndef _OE_LINK_H_
#define _OE_LINK_H_
#include "_OE_Link.h"
#endif

#ifndef _CDT_DRAW_H_
#define _CDT_DRAW_H_
#include "_cdt_Draw.h"
#endif

#ifndef _SNAKEGAME_H_
#define _SNAKEGAME_H_

// definition of GAME STATES
#define _SNAKEGAME_GAME_STATES_NOT_YET (0)
#define _SNAKEGAME_GAME_STATES_RUNNING (1)
#define _SNAKEGAME_GAME_STATES_PAUSE (2)

// definition of the default difficulty of game (range from 1 to 10)
#define _SANKEGAME_GAME_DIFFICULTY_DEFAULT (5)

#define _SNAKEGAME_RANKING_FILE ("./ranking.dat")
#define _SNAKEGAME_SETTING_FILE ("./settings.info")

#define _SNAKEGAME_RANK_MAX_NAME_NUM (0x40)

#define SNAKE_INIT_LENGTH (3)
#define SNAKE_NODE_CHAR (_T('*'))
#define SNAKE_FOOD_CHAR (_T('#'))
#define SNAKE_MOVE_DEFAULT_ELAPSE_MILLSECOND (100)
#define SNAKE_DEFAULT_COLOR (FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_RED)
#define SNAKE_FOOD_DEFAULT_COLOR (SNAKE_DEFAULT_COLOR)

class SnakeGame {
public:
	SnakeGame();
	~SnakeGame();

	int initGame();	// used for showing something logo or video
	int GUIMain();
	int setGameDefaultDifficulty(int);
	int exitGame();

private:
	typedef struct _tag_Map_Point {
		DWORD dwX;
		DWORD dwY;
	}Map_Point, *PMap_Point;
	typedef struct _tag_Map_Rect {
		Map_Point mpLeftUp;
		Map_Point mpRightDown;
	}Map_Rect, *PMap_Rect;

	struct _tag_Game_Record {
		unsigned int uiScore;
		unsigned int uiStrSize;
		char *ptName;
	};
	typedef _tag_Game_Record Game_Record, *PGame_Record;

	typedef	struct _tag_Game_Settings {
		unsigned int uiGameMode;
		unsigned int uiSnakeSpeed;
		unsigned int uiReserve3;
		unsigned int uiReserve4;
	}Game_Settings, *PGame_Settings;
	typedef struct _tag_Voice_Settings {
		unsigned int uiAnimationVoice;
		unsigned int uiClickVoice;
		unsigned int uiReserve3;
		unsigned int uiReserve4;
	}Voice_Settings, *PVoice_Settings;
	typedef struct _tag_Settings {
		Game_Settings gsSettings;
		Voice_Settings vsSettings;
	}Settings, *PSettings;

	const char m_cc_GameSettings[2][20] = {
		"Game Mode",
		"Snake Speed",
	};
	const char m_cc_VoiceSettings[2][20] = {
		"Animation Volumn",
		"Click Volumn",
	};
	const char m_cc_Settings[2][20] = {
		"Game",
		"Voice",
	};
	const char m_cui_SubTitleSize[2] = { 2,2 };


	HANDLE m_hConsole_input;
	HANDLE m_hConsole_output;
	HANDLE m_hHeapData;
	HANDLE m_hHeapTmp;

	DWORD m_dwConsole_input_mode_store;
	DWORD m_dwConsole_output_mode_store;

	DWORD m_dwGameStates;	// running, pause, not yet
	Settings m_Settings;	// Game Settings

	BOOL m_bLoadSettings;


	_CDT_DRAW m_cdt_Draw;
	_OE_Link<Map_Point> m_oe_Link;

	Map_Point m_MapSize;

	Map_Rect m_mrGUIArray[5];
	
	int ClearMap();
	int OpeningCinematic();
	
	int LoadSettings();
	int LoadDefaultSettings();
	int SaveSettings();
	
	int GUISettings();
	int GUIRank();
	int GUIHelp();
	int GUIExit();

	int newGame();
	int endGame();
};

SnakeGame::SnakeGame()
{
	DWORD dwTmp;
	BOOL bRet;

	m_hConsole_input = ::GetStdHandle(STD_INPUT_HANDLE);
	if (m_hConsole_input) {
		if (bRet = ::GetConsoleMode(m_hConsole_input, &dwTmp))
			m_dwConsole_input_mode_store = dwTmp;
	}
	m_hConsole_output = ::GetStdHandle(STD_OUTPUT_HANDLE);
	if (m_hConsole_output) {
		if (bRet = ::GetConsoleMode(m_hConsole_output, &dwTmp))
			m_dwConsole_output_mode_store = dwTmp;
	}
	dwTmp = m_dwConsole_input_mode_store & ~(ENABLE_QUICK_EDIT_MODE);
	bRet = ::SetConsoleMode(m_hConsole_input, dwTmp);

	m_hHeapData = ::HeapCreate(0, 0, 0);
	m_hHeapTmp = ::HeapCreate(0, 0, 0);

	m_dwGameStates = _SNAKEGAME_GAME_STATES_NOT_YET;
}

SnakeGame::~SnakeGame()
{
	BOOL bRet;

	bRet = ::SetConsoleMode(m_hConsole_input, m_dwConsole_input_mode_store);
	bRet = ::SetConsoleMode(m_hConsole_output, m_dwConsole_output_mode_store);

	if (m_hHeapData != NULL) {
		::HeapDestroy(m_hHeapData);
		m_hHeapData = NULL;
	}

	if (m_hHeapTmp != NULL) {
		::HeapDestroy(m_hHeapTmp);
		m_hHeapTmp = NULL;
	}

}

int SnakeGame::initGame()
{
	m_MapSize.dwX = 60;
	m_MapSize.dwY = 20;
	m_bLoadSettings = FALSE;

	Unit uTmp;
	int iRet;

	iRet = m_cdt_Draw.SetHandle(m_hConsole_output);
	iRet = m_cdt_Draw.SetArea(m_MapSize.dwY, m_MapSize.dwX);
	iRet = m_cdt_Draw.SetDefaultClear(uTmp = {_T(' '), FOREGROUND_BLUE});


	this->OpeningCinematic();

	if (this->LoadSettings())
		m_bLoadSettings = TRUE;
	
	return 1;
}

int SnakeGame::OpeningCinematic()
{

	const char ppArray_S[][20] = {
		"    _==%_ ",
		"   //   \\\\",
		"  (*      ",
		"   ~*+_   ",
		"      =>  ",
		" @*.  //  ",
		"   ~~=~   ",
	};
	const char ppArray_n[][20] = {
		" *&+_=+=_ ",
		"   $~    u",
		"  ,$    {!",
		"  @!    ? ",
		" //    /$ ",
	};
	const char ppArray_a[][20] = {
		"     ap_   ",
		"  l@    @  ",
		" i$    *   ",
		")$    oP_  ",
		"  ~#\"~  tP~",
	};
	const char ppArray_k[][20] = {
		"   /}   ",
		"  .?    ",
		"  {}    ",
		" .(  *~*",
		" ,$@~ _%",
		" @! &*~ ",
		"/}    #_",
	};
	const char ppArray_e[][20] = {
		"    _o,. ",
		"  +9   _P",
		" # __+=~ ",
		"@P~~ _,* ",
		" PioP~   ",
	};
	const char pStrSkip[] = "Press any key to Skip";
	DWORD dwSkipPosX, dwSkipPosY;
	DWORD dwSkipNum = ::strlen(pStrSkip);
	dwSkipPosX = (m_MapSize.dwX - dwSkipNum) / 2;
	dwSkipPosY = m_MapSize.dwY - 2;

	const unsigned int cuiColumnTime = 15;
	const unsigned int cuiArraySize[5] = { 10, 10, 11, 8, 9 };
	unsigned int uiArray_Offset_X = (m_MapSize.dwX - 48) / 2;
	unsigned int uiArray_Offset_Y = (m_MapSize.dwY) / 2 + 4;

	int i, j, n;

	typedef struct _tag_OA {
		DWORD dwDirect;		// 1 for up, 2 for down, 3 for left, 4 for right
		DWORD dwElapse;		// millisecond time
	}OA_DATA, *POA_DATA;

	const unsigned int cuiSnakeALength = 30;
	const unsigned int cuiSnakeBLength = 20;
	Map_Point mpAPos = { m_MapSize.dwX - 1, m_MapSize.dwY / 2 - 4};
	Map_Point mpBPos = { m_MapSize.dwX / 2, 0 };
	OA_DATA oa_SnakeMoveA[] = {
		{3, 50}, {3, 50}, {3, 60}, {3, 100}, {3, 100},
		{3, 50}, {3, 50}, {3, 60},
		{3, 50}, {3, 50}, {3, 60}, {3, 100}, {1, 100},
		{3, 50}, {1, 50}, {3, 60}, {3, 100}, {3, 100},
		{3, 50}, {3, 50}, {3, 60}, {3, 100}, {2, 100},
		{3, 50}, {2, 50}, {3, 60}, {3, 100}, {3, 100},
		{3, 50}, {2, 50}, {3, 60}, {2, 100}, {3, 100},
		{3, 50}, {3, 50}, {3, 60}, {1, 100}, {3, 100},
		{3, 50}, {3, 50}, {1, 60}, {3, 100}, {3, 100},
		{3, 50}, {3, 50}, {3, 60}, {3, 100}, {3, 100},
		{2, 50}, {3, 50}, {3, 60}, {3, 100}, {3, 100},
		{3, 50}, {3, 50}, {1, 60}, {3, 100}, {1, 100},
		{3, 50}, {3, 50}, {1, 60}, {3, 100}, {3, 100},
		{1, 50}, {3, 50}, {3, 60}, {1, 100}, {3, 100},
		{3, 50}, {3, 50}, {2, 60}, {3, 100}, {3, 100},
		{2, 50}, {3, 50}, {3, 60}, {2, 100}, {3, 100},
		{2, 50}, {3, 50}, {2, 60}, {4, 100}, {4, 100},
		{4, 50}, {2, 50}, {4, 60}, {4, 100}, {2, 100},
		{4, 50}, {4, 50}, {4, 60}, {4, 100}, {2, 100},
		{4, 50}, {4, 50}, {2, 60}, {4, 100}, {2, 100},
		{3, 50}, {2, 50}, {3, 60}, {3, 100}, {3, 100},
		{2, 50}, {3, 50}, {3, 60}, {3, 100}, {3, 100},
		{1, 50}, {3, 50}, {3, 60}, {3, 100}, {1, 100},
		{1, 50}, {1, 50}, {3, 60}, {3, 100}, {1, 100},
		{4, 50}, {4, 50}, {4, 60}, {1, 100}, {4, 100},
		{4, 50}, {4, 50}, {4, 60}, {4, 100}, {4, 100},
		{4, 50}, {4, 50}, {4, 60}, {4, 100}, {4, 100},
		{4, 50}, {4, 50}, {4, 60}, {4, 100}, {4, 100},
		{4, 50}, {4, 50}, {4, 60}, {4, 100}, {4, 100},
		{2, 50}, {3, 50}, {2, 60}, {3, 100}, {2, 100},
		{2, 50}, {2, 50}, {4, 60}, {2, 100}, {4, 100},
		{4, 50}, {4, 50}, {4, 60}, {4, 100}, {4, 100},
		{4, 50}, {4, 50}, {4, 60}, {4, 100}, {1, 100},
		{4, 50}, {4, 50}, {4, 60}, {1, 100}, {4, 100},
		{4, 50}, {4, 50}, {1, 60}, {4, 100}, {4, 100},
		{1, 50}, {3, 50}, {3, 60}, {1, 100}, {3, 100},
		{3, 50}, {1, 50}, {3, 60}, {3, 100}, {3, 100},
		{2, 50}, {3, 50}, {3, 60}, {2, 100}, {3, 100},
		{2, 50}, {2, 50}, {2, 60}, {4, 100}, {4, 100},
		{2, 50}, {4, 50}, {4, 60}, {4, 100}, {4, 100},
		{2, 50}, {4, 50}, {4, 60}, {4, 100}, {4, 100},
		{2, 50}, 
		{4, 50}, {4, 50}, {4, 60}, {4, 40}, {4, 60},
		{4, 50}, {4, 50}, {4, 60}, {4, 40}, {1, 60},
		{4, 50}, {4, 50}, {4, 60}, {4, 40}, {4, 60},
		{4, 50}, {4, 50}, {4, 60}, {4, 40}, {4, 60},
		{4, 50}, {4, 50}, {4, 60}, {4, 40}, {4, 60},
		{4, 50}, {4, 50}, {4, 60}, {4, 40}, {4, 60},
		{4, 50}, {4, 50}, {4, 60}, {4, 40}, {4, 60},
		{4, 50}, {4, 50}, {4, 60}, {4, 40}, {4, 60},
		{4, 50}, {4, 50}, {4, 60}, {4, 40}, {4, 60},
	};
	OA_DATA oa_SnakeMoveB[] = {
		{2, 200}, {4, 2300}, {4, 1800}, {4, 1400}, {4, 1100},
		{2, 800}, {4, 500}, {4, 300}, {4, 200}, {2, 100},
		{4, 50}, {4, 50}, {4, 40}, {4, 60}, {4, 100},
		{4, 50}, {2, 50}, {2, 40}, {3, 60}, {2, 100},
		{2, 50}, {3, 50}, {2, 40}, {2, 60}, {3, 100},
		{2, 50}, {2, 50}, {3, 40}, {2, 60}, {2, 100},
		{3, 50}, {2, 50}, {2, 40}, {4, 60}, {4, 100},
		{4, 50}, {1, 50}, {4, 40}, {4, 60}, {4, 100},
		{4, 50}, {4, 50}, {1, 40}, {4, 60}, {4, 100},
		{4, 50}, {1, 50}, {1, 40}, {3, 60}, {3, 100}, 
		{1, 50}, {3, 50}, {1, 40}, {1, 60}, {3, 100},
		{1, 50}, {3, 50}, {3, 40}, {3, 60}, {3, 100},
		{2, 50}, {3, 50}, {3, 40}, {2, 60}, {2, 100},
		{2, 50}, {4, 50}, {4, 40}, {4, 60}, {2, 100},
		{4, 50}, {4, 50}, {4, 40}, {4, 60}, {4, 100},
		{4, 50}, {4, 50}, {4, 40}, {4, 60}, {4, 100},
		{4, 50}, {4, 50}, {4, 40}, {4, 60}, {4, 100},
		{4, 50}, {4, 50}, {1, 40}, {1, 60}, {3, 100},
		{3, 50}, {3, 50}, {1, 40}, {3, 60}, {3, 100},
		{3, 50}, {3, 50}, {2, 40}, {3, 60}, {2, 100},
		{2, 50}, {2, 50}, {4, 40}, {4, 60}, {4, 100},
		{2, 50}, {4, 50}, {4, 40}, {4, 60}, {2, 100},
		{4, 50}, {4, 50}, {4, 40}, {4, 60}, {4, 100},
		{4, 50}, {4, 50}, {4, 40}, {4, 60}, {4, 100},
		{4, 100}, {4, 100}, {4, 100}, {4, 100}, {4, 100},
		{4, 100}, {4, 100}, {4, 100}, {4, 100}, {4, 100},
		{4, 50}, {4, 50},
	};
	DWORD dwMoveANum = sizeof(oa_SnakeMoveA) / sizeof(OA_DATA);
	DWORD dwMoveBNum = sizeof(oa_SnakeMoveB) / sizeof(OA_DATA);
	DWORD iA, iB;
	DWORD dwATimeBefore, dwATimeCur;
	DWORD dwBTimeBefore, dwBTimeCur;

	DWORD dwMinTime, dwMinTmp;

	dwMinTmp = 0xffff;
	for (i = 0; i < dwMoveANum; i++)
		if (oa_SnakeMoveA[i].dwElapse < dwMinTmp)
			dwMinTmp = oa_SnakeMoveA[i].dwElapse;
	dwMinTime = dwMinTmp;
	dwMinTmp = 0xffff;
	for (i = 0; i < dwMoveBNum; i++)
		if (oa_SnakeMoveB[i].dwElapse < dwMinTmp)
			dwMinTmp = oa_SnakeMoveB[i].dwElapse;
	if (dwMinTmp < dwMinTime)
		dwMinTmp = dwMinTmp;
	

	_OE_Link<Map_Point> oeLink_A;
	oeLink_A.new_link();
	_OE_Link<Map_Point> oeLink_B;
	oeLink_B.new_link();

	MList mlTmp;
	PUnit puTmp;
	MOne moTmp;
	Unit uTmp;
	Map_Point *pmpData, mpTmp, mpTmp2;

	INPUT_RECORD input_record[5];
	MOUSE_EVENT_RECORD mouse_record;
	KEY_EVENT_RECORD key_record;
	DWORD dwRead;

	int iTmp;
	DWORD dwTmp;
	BOOL bRet;

	iA = iB = 0;
	mpTmp.dwX = mpAPos.dwX - 1;
	mpTmp.dwY = mpAPos.dwY;
	oeLink_A.appendAtBack(mpTmp);
	iA++;
	mpTmp.dwX = mpBPos.dwX;
	mpTmp.dwY = mpBPos.dwY + 1;
	oeLink_B.appendAtBack(mpTmp);
	iB++;

	BOOL bComplete = FALSE;
	BOOL bSkip = FALSE;
	BOOL bReDraw = FALSE;

	::Sleep(500);

	dwATimeBefore = dwBTimeBefore = ::GetTickCount();
	while (!bComplete) {
		// get the any key
		if (::GetNumberOfConsoleInputEvents(m_hConsole_input, &dwTmp) && dwTmp > 0) {
			bRet = ::ReadConsoleInput(m_hConsole_input, input_record, 5, &dwRead);
			if (bRet) {
				for (DWORD i = 0; i < dwRead; i++) {
					switch (input_record[i].EventType)
					{
					case MOUSE_EVENT:
						mouse_record = input_record[i].Event.MouseEvent;
						if (mouse_record.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
							bSkip = TRUE;
						break;
					case KEY_EVENT:
						key_record = input_record[i].Event.KeyEvent;
						if (key_record.bKeyDown && key_record.wVirtualKeyCode > 0x20)
							bSkip = TRUE;
						break;
					default:
						break;
					}
					if (bSkip) {
						puTmp = (PUnit)::HeapAlloc(m_hHeapTmp, HEAP_ZERO_MEMORY, sizeof(Unit)*dwSkipNum);
						for (DWORD i = 0; i < dwSkipNum; i++) {
							puTmp[i].a = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
							puTmp[i].c = pStrSkip[i];
						}
						mlTmp.uiLeft = dwSkipPosX;
						mlTmp.uiUp = dwSkipPosY;
						mlTmp.uiNum = dwSkipNum;
						m_cdt_Draw.ModifyList(mlTmp, puTmp);
						::Sleep(500);
						for (DWORD i = 0; i < dwSkipNum; i++) {
							puTmp[i].a = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
						}
						m_cdt_Draw.ModifyList(mlTmp, puTmp);
						::Sleep(200);
						for (DWORD i = 0; i < dwSkipNum; i++) {
							puTmp[i].a = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
						}
						m_cdt_Draw.ModifyList(mlTmp, puTmp);
						::Sleep(300);
						for (DWORD i = 0; i < dwSkipNum; i++) {
							puTmp[i].a = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
						}
						m_cdt_Draw.ModifyList(mlTmp, puTmp);
						::Sleep(200);
						::HeapFree(m_hHeapTmp, 0, puTmp);

						bComplete = TRUE;
						break;
					}
				}
			}
		}
		if (bSkip)
			continue;

		// move the snake
		dwATimeCur = ::GetTickCount();
		if (iA < dwMoveANum && dwATimeCur-dwATimeBefore >= oa_SnakeMoveA[iA].dwElapse) {
			dwATimeBefore = dwATimeCur;
			iTmp = 1;
			oeLink_A.output_links(&mpTmp, &iTmp);
			switch (oa_SnakeMoveA[iA].dwDirect)
			{
			case 1:		// Upwards
				if (oa_SnakeMoveA[iA - 1].dwDirect != 2 && mpTmp.dwY > 0) {
					mpTmp.dwY--;
					oeLink_A.insertAtFront(mpTmp);
				}
				break;
			case 2:		// Downwards
				if (oa_SnakeMoveA[iA - 1].dwDirect != 1 && mpTmp.dwY < m_MapSize.dwY - 1) {
					mpTmp.dwY++;
					oeLink_A.insertAtFront(mpTmp);
				}
				break;
			case 3:		// Leftwards
				if (oa_SnakeMoveA[iA - 1].dwDirect != 4 && mpTmp.dwX > 0) {
					mpTmp.dwX--;
					oeLink_A.insertAtFront(mpTmp);
				}
				break;
			case 4:;	// Rightwards
				if (oa_SnakeMoveA[iA - 1].dwDirect != 3 && mpTmp.dwX < m_MapSize.dwX - 1) {
					mpTmp.dwX++;
					oeLink_A.insertAtFront(mpTmp);
				}
				break;
			default:
				break;
			}
			
			if (iA >= cuiSnakeALength)
				oeLink_A.removeLast();

			bReDraw = TRUE;
			iA++;
		}
		dwBTimeCur = ::GetTickCount();
		if (iB < dwMoveBNum && dwBTimeCur-dwBTimeBefore >= oa_SnakeMoveB[iB].dwElapse) {
			dwBTimeBefore = dwBTimeCur;
			iTmp = 1;
			oeLink_B.output_links(&mpTmp, &iTmp);
			switch (oa_SnakeMoveB[iB].dwDirect)
			{
			case 1:		// Upwards
				if (oa_SnakeMoveB[iB - 1].dwDirect != 2 && mpTmp.dwY > 0) {
					mpTmp.dwY--;
					oeLink_B.insertAtFront(mpTmp);
				}
				break;
			case 2:		// Downwards
				if (oa_SnakeMoveB[iB - 1].dwDirect != 1 && mpTmp.dwY < m_MapSize.dwY - 1) {
					mpTmp.dwY++;
					oeLink_B.insertAtFront(mpTmp);
				}
				break;
			case 3:		// Leftwards
				if (oa_SnakeMoveB[iB - 1].dwDirect != 4 && mpTmp.dwX > 0) {
					mpTmp.dwX--;
					oeLink_B.insertAtFront(mpTmp);
				}
				break;
			case 4:;	// Rightwards
				if (oa_SnakeMoveB[iB - 1].dwDirect != 3 && mpTmp.dwX < m_MapSize.dwX - 1) {
					mpTmp.dwX++;
					oeLink_B.insertAtFront(mpTmp);
				}
				break;
			default:
				break;
			}

			if (iB >= cuiSnakeBLength)
				oeLink_B.removeLast();
			bReDraw = TRUE;
			iB++;
		}

		// show current snake and skip info
		if (bReDraw) {
			this->ClearMap();

			iTmp = 0;
			oeLink_A.output_links(NULL, &iTmp);
			pmpData = (PMap_Point)::HeapAlloc(m_hHeapData, HEAP_ZERO_MEMORY, sizeof(Map_Point)*iTmp);
			oeLink_A.output_links(pmpData, &iTmp);
			for (i = 0; i < iTmp; i++) {
				moTmp.uiLeft = pmpData[i].dwX;
				moTmp.uiUp = pmpData[i].dwY;
				uTmp.a = FOREGROUND_INTENSITY | FOREGROUND_GREEN;
				uTmp.c = SNAKE_NODE_CHAR;
				m_cdt_Draw.ModifyOne(moTmp, uTmp);
			}
			::HeapFree(m_hHeapData, 0, pmpData);

			iTmp = 0;
			oeLink_B.output_links(NULL, &iTmp);
			pmpData = (PMap_Point)::HeapAlloc(m_hHeapData, HEAP_ZERO_MEMORY, sizeof(Map_Point)*iTmp);
			oeLink_B.output_links(pmpData, &iTmp);
			for (i = 0; i < iTmp; i++) {
				moTmp.uiLeft = pmpData[i].dwX;
				moTmp.uiUp = pmpData[i].dwY;
				uTmp.a = FOREGROUND_INTENSITY | FOREGROUND_RED;
				uTmp.c = SNAKE_NODE_CHAR;
				m_cdt_Draw.ModifyOne(moTmp, uTmp);
			}
			::HeapFree(m_hHeapData, 0, pmpData);

			mlTmp.uiLeft = dwSkipPosX;
			mlTmp.uiUp = dwSkipPosY;
			mlTmp.uiNum = dwSkipNum;
			puTmp = (PUnit)::HeapAlloc(m_hHeapTmp, HEAP_ZERO_MEMORY, sizeof(Unit)*dwSkipNum);
			for (i = 0; i < dwSkipNum; i++) {
				puTmp[i].a = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
				puTmp[i].c = pStrSkip[i];
			}
			m_cdt_Draw.ModifyList(mlTmp, puTmp);
			::HeapFree(m_hHeapTmp, 0, puTmp);

			bReDraw = FALSE;
		}

		if (iA >= dwMoveANum && iB >= dwMoveBNum)
			bComplete = TRUE;

		::Sleep(dwMinTime / 5);

	}


	::Sleep(50);
	this->ClearMap();
	// show the Game Name ---- Snake
	uTmp.a = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
	for (i = 0; i < cuiArraySize[0]; i++) {
		for (j = 0; j < 7; j++) {
			moTmp.uiLeft = i + uiArray_Offset_X;
			moTmp.uiUp = j + uiArray_Offset_Y - 7;
			uTmp.c = ppArray_S[j][i];
			m_cdt_Draw.ModifyOne(moTmp, uTmp);
		}
		::Sleep(cuiColumnTime);
	}
	uiArray_Offset_X += cuiArraySize[0] - 2;
	for (i = 0; i < cuiArraySize[1]; i++) {
		for (j = 0; j < 5; j++) {
			moTmp.uiLeft = i + uiArray_Offset_X;
			moTmp.uiUp = j + uiArray_Offset_Y - 5;
			uTmp.c = ppArray_n[j][i];
			m_cdt_Draw.ModifyOne(moTmp, uTmp);
		}
		::Sleep(cuiColumnTime);
	}
	uiArray_Offset_X += cuiArraySize[1];
	for (i = 0; i < cuiArraySize[2]; i++) {
		for (j = 0; j < 5; j++) {
			moTmp.uiLeft = i + uiArray_Offset_X;
			moTmp.uiUp = j + uiArray_Offset_Y - 5;
			uTmp.c = ppArray_a[j][i];
			m_cdt_Draw.ModifyOne(moTmp, uTmp);
		}
		::Sleep(cuiColumnTime);
	}
	uiArray_Offset_X += cuiArraySize[2];
	for (i = 0; i < cuiArraySize[3]; i++) {
		for (j = 0; j < 7; j++) {
			moTmp.uiLeft = i + uiArray_Offset_X;
			moTmp.uiUp = j + uiArray_Offset_Y - 7;
			uTmp.c = ppArray_k[j][i];
			m_cdt_Draw.ModifyOne(moTmp, uTmp);
		}
		::Sleep(cuiColumnTime);
	}
	uiArray_Offset_X += cuiArraySize[3] + 2;
	for (i = 0; i < cuiArraySize[4]; i++) {
		for (j = 0; j < 5; j++) {
			moTmp.uiLeft = i + uiArray_Offset_X;
			moTmp.uiUp = j + uiArray_Offset_Y - 5;
			uTmp.c = ppArray_e[j][i];
			m_cdt_Draw.ModifyOne(moTmp, uTmp);
		}
		::Sleep(cuiColumnTime);
	}

	::Sleep(700);
	this->ClearMap();
	return 1;
}

int SnakeGame::GUIMain()
{
	const TCHAR _cc_Menus[][20] = { {_T("New Game") }, 
									{_T("Settings") }, 
									{_T("Ranking")}, 
									{_T("Help")}, 
									{_T("Exit")} };

	while (true) {
		/********************************
		*	Wait for the choice of meun	*
		*********************************/
#define NUM_PER_INPUT (5)

		INPUT_RECORD input_record[NUM_PER_INPUT];
		FOCUS_EVENT_RECORD focus_record;
		KEY_EVENT_RECORD key_record;
		MOUSE_EVENT_RECORD mouse_record;
		COORD coord_tmp;
		BOOL bRet;
		DWORD dwTmp;
		DWORD dwError;

		static unsigned int uiCurIndex = 0;
		BOOL bCurIndexChange = FALSE;
		HRESULT hRet;
		size_t stTmp;
		PUnit _pUnit;
		MList _mList;
		Map_Point _mpTmp;
		int _x, _y, i, iTmp;

		_y = (m_MapSize.dwY - 9) / 2 - 1;

		for (int j = 0; j < 5; j++, _y+=2) {
			hRet = ::StringCchLength(_cc_Menus[j], 20, &stTmp);
			_x = (m_MapSize.dwX - stTmp) / 2 - 1;
			_pUnit = (PUnit)::HeapAlloc(m_hHeapTmp, HEAP_ZERO_MEMORY, sizeof(Unit)*stTmp);
			if (j != uiCurIndex) {
				for (i = 0; i < stTmp; i++)
				{
					_pUnit[i].a = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
					_pUnit[i].c = _cc_Menus[j][i];
				}
			}
			else {
				for (i = 0; i < stTmp; i++) {
					_pUnit[i].a = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
					_pUnit[i].c = _cc_Menus[j][i];
				}
			}
			_mList.uiLeft = _x;
			_mList.uiUp = _y;
			_mList.uiNum = stTmp;
			m_cdt_Draw.ModifyList(_mList, _pUnit);

			_mpTmp.dwX = _x; _mpTmp.dwY = _y;
			m_mrGUIArray[j].mpLeftUp = _mpTmp;
			_mpTmp.dwX = _x + stTmp - 1;
			m_mrGUIArray[j].mpRightDown = _mpTmp;
			
			::HeapFree(m_hHeapTmp, 0, _pUnit);
		}

		bRet = ::ReadConsoleInput(m_hConsole_input, input_record, NUM_PER_INPUT, &dwTmp);
		if (bRet) {
			for (int i = 0; i < dwTmp; i++) {
				// all of these are used for choose the menu
				switch (input_record[i].EventType)
				{
				case KEY_EVENT:
					key_record = input_record[i].Event.KeyEvent;
					if (key_record.bKeyDown) {
						switch (key_record.wVirtualKeyCode) {
						case VK_UP:
							if (uiCurIndex > 0)
								uiCurIndex--;
							break;
						case VK_DOWN:
							if (uiCurIndex < 4)
								uiCurIndex++;
							break;
						case VK_RETURN:
							switch (uiCurIndex)
							{
							case 0:
								this->newGame();
								break;
							case 1:
								this->GUISettings();
								break;
							case 2:
								this->GUIRank();
								break;
							case 3:
								this->GUIHelp();
								break;
							case 4:
								this->GUIExit();
								break;
							default:
								break;
							}
							break;
						default:
							break;
						}
					}

					break;
				case MOUSE_EVENT:
					mouse_record = input_record[i].Event.MouseEvent;
					coord_tmp = mouse_record.dwMousePosition;
					coord_tmp.X -= m_cdt_Draw.GetOffsetX();
					coord_tmp.Y -= m_cdt_Draw.GetOffsetY();

					for (int j = 0; j < 5; j++) {
						if (m_mrGUIArray[j].mpLeftUp.dwX <= coord_tmp.X && coord_tmp.X <= m_mrGUIArray[j].mpRightDown.dwX && \
							m_mrGUIArray[j].mpLeftUp.dwY <= coord_tmp.Y && coord_tmp.Y <= m_mrGUIArray[j].mpRightDown.dwY) {
							uiCurIndex = j;
							if (mouse_record.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) {
								switch (uiCurIndex)
								{
								case 0:
									this->newGame();
									break;
								case 1:
									this->GUISettings();
									break;
								case 2:
									this->GUIRank();
									break;
								case 3:
									this->GUIHelp();
									break;
								case 4:
									this->GUIExit();
									break;
								default:
									break;
								}
							}
							break;
						}
					}

					break;

				default:
					break;
				}
			}
		}
		else {
			dwError = ::GetLastError();
		}

		::Sleep(10);
	}

	return 0;
}

int SnakeGame::LoadSettings()
{
	const unsigned int cuiMaxCharPerRow = 40;
	int i, j, iSize, iSize2;
	Settings sSettings;
	FILE *pFileSettings;
	size_t stRet;

	::ZeroMemory(&sSettings, sizeof(Settings));
	pFileSettings = ::fopen(_SNAKEGAME_SETTING_FILE, "r");
	if (pFileSettings == NULL) {
		// do not exist settings file, create the default settings
		this->LoadDefaultSettings();
	}
	else {
		// load the settings file
		stRet = ::fread(&sSettings, sizeof(Settings), 1, pFileSettings);
		if (stRet != 1) {
			printf("Error of Loading Settings Failed!\n");
		}
		else
			m_Settings = sSettings;
		::fclose(pFileSettings);
	}
	

	return 1;
}

int SnakeGame::LoadDefaultSettings()
{
	::ZeroMemory(&m_Settings, sizeof(Settings));
	m_Settings.gsSettings.uiGameMode = 0;
	m_Settings.gsSettings.uiSnakeSpeed = 200;
	m_Settings.vsSettings.uiAnimationVoice = 40;
	m_Settings.vsSettings.uiClickVoice = 30;

	return 1;
}

int SnakeGame::SaveSettings()
{
	FILE *pFileSettings;
	size_t stRet;

	pFileSettings = ::fopen(_SNAKEGAME_SETTING_FILE, "w");
	if (pFileSettings != NULL) {
		stRet = ::fwrite(&m_Settings, sizeof(Settings), 1, pFileSettings);
		if (stRet != 1) {
			printf("Error of Savign Settings Failed!\n");
		}
		::fclose(pFileSettings);
	}

	return 1;
}

int SnakeGame::newGame()
{
	this->ClearMap();

	DWORD dwMapPointFirst_X = m_MapSize.dwX/3*2;
	DWORD dwMapPointFirst_Y = m_MapSize.dwY/4*3;
	Map_Point mpTmp, mpCurFoodPos;
	PMap_Point pmpData = NULL;
	int i, j, iRandX, iRandY, iSize, iRet, iTmp;
	time_t ttTmp;
	BOOL bEndGame = FALSE;
	BOOL bVictory = FALSE;
	BOOL bConflict = TRUE;
	BOOL bReDraw = FALSE;
	
	MOne mOneMof;
	MList mListMof;
	Unit uTmp;
	PUnit puData;

	DWORD dwTimeLast;
	DWORD dwTimeCur;

	INPUT_RECORD input_record[5];
	FOCUS_EVENT_RECORD focus_record;
	KEY_EVENT_RECORD key_record;
	BOOL bRet;
	DWORD dwRead;
	DWORD dwCurDirect;	// 1 for up, 2 for down, 3 for left, 4 for right
	DWORD dwTmp;

	BOOL bPause = FALSE;
	BOOL bFocus = TRUE;

	m_oe_Link.new_link();
	mpTmp.dwY = dwMapPointFirst_Y;
	mpTmp.dwX = dwMapPointFirst_X;
	for (i = 0; i < SNAKE_INIT_LENGTH; i++) {
		mpTmp.dwX++;
		m_oe_Link.appendAtBack(mpTmp);
	}
	dwCurDirect = 3;

	dwTimeLast = ::GetTickCount();

	while (!bEndGame) {
		srand(time(&ttTmp));

		// Get the correct food position
		if (!bPause) {
			while (bConflict) {
				iRandX = rand() % m_MapSize.dwX;
				iRandY = rand() % m_MapSize.dwY;

				if (m_oe_Link.output_links(NULL, &iSize) == 0) {
					pmpData = (PMap_Point)::HeapAlloc(m_hHeapData, HEAP_ZERO_MEMORY, sizeof(Map_Point)*iSize);
					iRet = m_oe_Link.output_links(pmpData, &iSize);
					if (iRet > 0) {
						for (i = 0; i < iSize; i++) {
							if (iRandX == pmpData[i].dwX && iRandY == pmpData[i].dwY)
								bConflict = TRUE;
							else {
								bConflict = FALSE;
								mpCurFoodPos.dwX = iRandX;
								mpCurFoodPos.dwY = iRandY;
								break;
							}
						}
					}
					if (pmpData != NULL) {
						::HeapFree(m_hHeapData, 0, pmpData);
						pmpData = NULL;
					}
				}

			}
		}

		// move the snake including eat food
		if (!bPause) {
			dwTimeCur = ::GetTickCount();
			if (dwTimeCur - dwTimeLast >= m_Settings.gsSettings.uiSnakeSpeed) {
				dwTimeLast = dwTimeCur;

				bReDraw = TRUE;

				iTmp = 1;
				m_oe_Link.output_links(&mpTmp, &iTmp);

				if (m_oe_Link.output_links(NULL, &iSize) == 0) {
					pmpData = (PMap_Point)::HeapAlloc(m_hHeapData, HEAP_ZERO_MEMORY, sizeof(Map_Point)*iSize);
					iRet = m_oe_Link.output_links(pmpData, &iSize);
				}

				switch (dwCurDirect)
				{
				case 1:		// up
					if (mpTmp.dwY == 0) {
						// failed
						bVictory = FALSE;
						bEndGame = TRUE;
					}
					else {
						mpTmp.dwY--;
						m_oe_Link.insertAtFront(mpTmp);
						if (mpTmp.dwX == mpCurFoodPos.dwX && mpTmp.dwY == mpCurFoodPos.dwY)
							bConflict = TRUE;
						else {
							for (i = 0; i < iSize; i++)
								if (pmpData[i].dwX == mpTmp.dwX && pmpData[i].dwY == mpTmp.dwY)
								{
									bVictory = FALSE;
									bEndGame = TRUE;
								}
						}
					}

					break;
				case 2:		// down
					if (++mpTmp.dwY >= m_MapSize.dwY) {
						// failed
						bVictory = FALSE;
						bEndGame = TRUE;
					}
					else {
						m_oe_Link.insertAtFront(mpTmp);
						if (mpTmp.dwX == mpCurFoodPos.dwX && mpTmp.dwY == mpCurFoodPos.dwY)
							bConflict = TRUE;
						else {
							for (i = 0; i < iSize; i++)
								if (pmpData[i].dwX == mpTmp.dwX && pmpData[i].dwY == mpTmp.dwY)
								{
									bVictory = FALSE;
									bEndGame = TRUE;
								}
						}
					}

					break;
				case 3:		// left
					if (mpTmp.dwX == 0) {
						// failed
						bVictory = FALSE;
						bEndGame = TRUE;
					}
					else {
						mpTmp.dwX--;
						m_oe_Link.insertAtFront(mpTmp);
						if (mpTmp.dwX == mpCurFoodPos.dwX && mpTmp.dwY == mpCurFoodPos.dwY)
							bConflict = TRUE;
						else {
							for (i = 0; i < iSize; i++)
								if (pmpData[i].dwX == mpTmp.dwX && pmpData[i].dwY == mpTmp.dwY)
								{
									bVictory = FALSE;
									bEndGame = TRUE;
								}
						}
					}

					break;
				case 4:		// right
					if (++mpTmp.dwX >= m_MapSize.dwX) {
						// failed
						bVictory = FALSE;
						bEndGame = TRUE;
					}
					else {
						m_oe_Link.insertAtFront(mpTmp);
						if (mpTmp.dwX == mpCurFoodPos.dwX && mpTmp.dwY == mpCurFoodPos.dwY)
							bConflict = TRUE;
						else {
							for (i = 0; i < iSize; i++)
								if (pmpData[i].dwX == mpTmp.dwX && pmpData[i].dwY == mpTmp.dwY)
								{
									bVictory = FALSE;
									bEndGame = TRUE;
								}
						}
					}

					break;
				default:
					break;
				}

				if (pmpData != NULL) {
					::HeapFree(m_hHeapData, 0, pmpData);
					pmpData = NULL;

					if (bConflict == FALSE)
						m_oe_Link.removeLast();
				}
			}
		}
			// show the food and snake
		if (!bPause && bReDraw) {
			bReDraw = FALSE;
			this->ClearMap();
			if (m_oe_Link.output_links(NULL, &iSize) == 0) {
				pmpData = (PMap_Point)::HeapAlloc(m_hHeapData, HEAP_ZERO_MEMORY, sizeof(Map_Point)*iSize);
				iRet = m_oe_Link.output_links(pmpData, &iSize);
				if (iRet > 0) {
					uTmp.c = SNAKE_NODE_CHAR;
					uTmp.a = SNAKE_DEFAULT_COLOR;

					for (i = 0; i < iSize; i++) {
						mOneMof.uiLeft = pmpData[i].dwX;
						mOneMof.uiUp = pmpData[i].dwY;
						m_cdt_Draw.ModifyOne(mOneMof, uTmp);
					}

					uTmp.c = SNAKE_FOOD_CHAR;
					mOneMof.uiLeft = mpCurFoodPos.dwX;
					mOneMof.uiUp = mpCurFoodPos.dwY;
					m_cdt_Draw.ModifyOne(mOneMof, uTmp);
				}
				if (pmpData != NULL) {
					::HeapFree(m_hHeapData, 0, pmpData);
					pmpData = NULL;
				}
			}
		}

		// get the key click and calculate
		if (::GetNumberOfConsoleInputEvents(m_hConsole_input, &dwTmp) && dwTmp > 0)
		{
			bRet = ::ReadConsoleInput(m_hConsole_input, input_record, 5, &dwRead);
			if (bRet) {
				for (i = 0; i < dwRead; i++) {
					switch (input_record[i].EventType)
					{
					case FOCUS_EVENT:
						focus_record = input_record[i].Event.FocusEvent;
						if (focus_record.bSetFocus)
							bFocus = TRUE;
						else
							bFocus = FALSE;

						break;
					case KEY_EVENT:
						key_record = input_record[i].Event.KeyEvent;
						if (key_record.bKeyDown) {
							switch (key_record.wVirtualKeyCode)
							{
							case VK_LEFT:
							case 0x41:	// A
								if (dwCurDirect != 4)
									dwCurDirect = 3;

								break;
							case VK_RIGHT:
							case 0x44:	// D
								if (dwCurDirect != 3)
									dwCurDirect = 4;
								break;
							case VK_UP:
							case 0x57:	// W
								if (dwCurDirect != 2)
									dwCurDirect = 1;
								break;
							case VK_DOWN:
							case 0x53:	// S
								if (dwCurDirect != 1)
									dwCurDirect = 2;
								break;
							case 0x50: // P
								if (!bPause)
									bPause = TRUE;
								else
									bPause = FALSE;
								break;
							default:
								break;
							}
						}
						break;
					default:
						break;
					}
				}
			}
		}

		if (!bPause)
			::Sleep(m_Settings.gsSettings.uiSnakeSpeed / 10);
		else
			::Sleep(m_Settings.gsSettings.uiSnakeSpeed * 5);
		
	}
	this->ClearMap();

	this->endGame();

	return 0;
}

int SnakeGame::endGame() {

	int iTmp, i, j;
	char _pcScore[10];
	char *_pStr;
	const TCHAR _cc_Notice_Space[] = _T("SPACE is not supported.");
	const TCHAR _cc_Notice_Input[] = _T("Please input your name.");
	const TCHAR endGamePrompt[][40] = { _T("Game Over"),
										_T("Hero, Your Score is:         "), 
										_T("Can I know your name? (Only ascii)") };
	const TCHAR choosePress[][20] = {	_T(" Tell You "), 
										_T(" Skip ") };
	char inputName[_SNAKEGAME_RANK_MAX_NAME_NUM];
	DWORD dwCurNameSize = 0;
	const DWORD dwInputNameReserve = 20;
	const DWORD dwPressYesNoBlank = 6;

	DWORD _xInputName;
	DWORD dwTmp;
	unsigned int uiCurChoose = 0;	// 0 for yes, 1 for no
	FILE *pFileRank = NULL;

	DWORD dwX, dwY;
	DWORD _x, _y, _xInput, _yInput;
	
	size_t stTmp, stTmp2, stSize[2];

	Map_Rect mrYesNo[2];

	HRESULT hRet;
	MRect mrTmp;
	MList mlTmp;
	Unit *puTmp, uTmp;
	BOOL bRet;
	BOOL bReturn = FALSE;
	BOOL bSkip = FALSE;
	BOOL bThisInput = FALSE;

	CONSOLE_SCREEN_BUFFER_INFO csBufInfo;
	COORD crdCurPos, crdInitPos;

	INPUT_RECORD input_record[5];
	KEY_EVENT_RECORD key_record;
	MOUSE_EVENT_RECORD mouse_record;
	COORD crdTmp;
	DWORD dwRead;
	WORD wVirCode;

	bRet = ::GetConsoleScreenBufferInfo(m_hConsole_input, &csBufInfo);
	if (bRet)
		crdInitPos = csBufInfo.dwCursorPosition;
	else
		::ZeroMemory(&crdInitPos, sizeof(COORD));

	dwX = m_MapSize.dwX;
	dwY = m_MapSize.dwY;

	_y = dwY / 10 * 2;


	for (i = 0; i < 3; i++) {
		hRet = ::StringCchLength(endGamePrompt[i], 40, &stTmp);
		_x = (dwX - stTmp) / 2;
		mlTmp.uiLeft = _x;
		mlTmp.uiUp = _y;
		mlTmp.uiNum = stTmp;
		puTmp = (Unit*)::HeapAlloc(m_hHeapTmp, HEAP_ZERO_MEMORY, sizeof(Unit)*stTmp);
		for (j = 0; j < stTmp; j++) {
			puTmp[j].a = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
			puTmp[j].c = endGamePrompt[i][j];
		}
		m_cdt_Draw.ModifyList(mlTmp, puTmp);
		::HeapFree(m_hHeapTmp, 0, puTmp);
		_y += 2;
	}

	for (i = 0; i < 2; i++)
		hRet = ::StringCchLength(choosePress[i], 20, &(stSize[i]));

	m_oe_Link.output_links(NULL, &iTmp);
	iTmp -= 2;
	::_itoa_s(iTmp, _pcScore, sizeof(_pcScore) - 1, 10);
	stTmp2 = ::strlen(_pcScore);
	hRet = ::StringCchLength(endGamePrompt[1], 40, &stTmp);
	_x = (dwX - stTmp) / 2 + stTmp - 6;
	mlTmp.uiLeft = _x;
	mlTmp.uiUp = _y - 4;
	mlTmp.uiNum = stTmp2;
	puTmp = (Unit*)::HeapAlloc(m_hHeapTmp, HEAP_ZERO_MEMORY, sizeof(Unit)*stTmp2);
	for (i = 0; i < stTmp2; i++) {
		puTmp[i].a = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
		puTmp[i].c = _pcScore[i];

	}
	m_cdt_Draw.ModifyList(mlTmp, puTmp);
	::HeapFree(m_hHeapTmp, 0, puTmp);


	_y += 1;
	_y += 3;
	_x = (dwX - stSize[0] - stSize[1] - dwPressYesNoBlank) / 2;
	mrYesNo[0].mpLeftUp.dwX = _x;
	_x += stSize[0];
	mrYesNo[0].mpRightDown.dwX = _x;
	_x += dwPressYesNoBlank;
	mrYesNo[1].mpLeftUp.dwX = _x;
	_x += stSize[1];
	mrYesNo[1].mpRightDown.dwX = _x;

	mrYesNo[0].mpLeftUp.dwY = _y;
	mrYesNo[0].mpRightDown.dwY = _y;
	mrYesNo[1].mpLeftUp.dwY = _y;
	mrYesNo[1].mpRightDown.dwY = _y;

	_yInput = _y - 3;
	_xInput = (dwX - dwInputNameReserve) / 2;
	dwCurNameSize = 0;

	crdCurPos.X = _xInput + m_cdt_Draw.GetOffsetX();
	crdCurPos.Y = _yInput + m_cdt_Draw.GetOffsetY();
	bRet = ::SetConsoleCursorPosition(m_hConsole_output, crdCurPos);
	
	while (!bReturn) {
		// show the "choose" press 
		for (i = 0; i < 2; i++) {
			mlTmp.uiLeft = mrYesNo[i].mpLeftUp.dwX;
			mlTmp.uiUp = mrYesNo[i].mpLeftUp.dwY;
			mlTmp.uiNum = stSize[i];
			puTmp = (Unit*)::HeapAlloc(m_hHeapTmp, HEAP_ZERO_MEMORY, sizeof(Unit)*stSize[i]);
			for (j = 0; j < stSize[i]; j++) {
				puTmp[j].c = choosePress[i][j];
				if (i == uiCurChoose)
					puTmp[j].a = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
				else
					puTmp[j].a = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
			}
			m_cdt_Draw.ModifyList(mlTmp, puTmp);
			::HeapFree(m_hHeapTmp, 0, puTmp);
		}

		// get the key input
		if (::GetNumberOfConsoleInputEvents(m_hConsole_input, &dwTmp) && dwTmp > 0) {
			bRet = ::ReadConsoleInput(m_hConsole_input, input_record, 5, &dwRead);
			if (bRet) {
				for (i = 0; i < dwRead; i++) {
					switch (input_record[i].EventType)
					{
					case KEY_EVENT:
						key_record = input_record[i].Event.KeyEvent;
						if (key_record.bKeyDown) {
							wVirCode = key_record.wVirtualKeyCode;
							bThisInput = FALSE;
							switch (wVirCode)
							{
							case VK_RIGHT:
								if (uiCurChoose < 1)
									uiCurChoose = 1;
								break;

							case VK_LEFT:
								if (uiCurChoose > 0)
									uiCurChoose = 0;

							case VK_RETURN:	// enter
								if (uiCurChoose)
									bSkip = TRUE;
								else 
									bSkip = FALSE;

								bReturn = TRUE;

								break;
							case VK_BACK:	//	delete
								if (dwCurNameSize > 0) {
									dwCurNameSize--;
									crdCurPos.X--;
									bRet = ::SetConsoleCursorPosition(m_hConsole_output, crdCurPos);
								}

								break;
							case VK_SPACE:	// notice that can't use SPACE
								_x = (dwX - sizeof(_cc_Notice_Space)/sizeof(TCHAR)) / 2;
								_y = dwY - 1;
								mlTmp.uiLeft = _x;
								mlTmp.uiUp = _y;
								mlTmp.uiNum = sizeof(_cc_Notice_Space)/sizeof(TCHAR)-1;
								puTmp = (Unit*)::HeapAlloc(m_hHeapTmp, HEAP_ZERO_MEMORY, sizeof(Unit) * sizeof(_cc_Notice_Space));
								for (i = 0; i < mlTmp.uiNum; i++) {
									puTmp[i].a = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
									puTmp[i].c = _cc_Notice_Space[i];
								}
								m_cdt_Draw.ModifyList(mlTmp, puTmp);
								::HeapFree(m_hHeapTmp, 0, puTmp);

								break;
							default:
								bThisInput = FALSE;
								if (0x30 <= wVirCode && wVirCode <= 0x39) {
									inputName[dwCurNameSize++] = wVirCode;
									bThisInput = TRUE;
								}
								else if (0x41 <= wVirCode && wVirCode <= 0x5a) {
									if ((key_record.dwControlKeyState & SHIFT_PRESSED) || \
										key_record.dwControlKeyState & CAPSLOCK_ON){
										inputName[dwCurNameSize++] = wVirCode;
										bThisInput = TRUE;
									}
									else {
										inputName[dwCurNameSize++] = wVirCode + 'a' - 'A';
										bThisInput = TRUE;
									}
								}
								else if (0x60 <= wVirCode && wVirCode <= 0x69) {
									inputName[dwCurNameSize++] = wVirCode - (0x60 - 0x30);
									bThisInput = TRUE;
								}

								if (bThisInput) {
									if (dwCurNameSize >= dwInputNameReserve)
										dwCurNameSize--;
									else {
										crdCurPos.X++;
										bRet = ::SetConsoleCursorPosition(m_hConsole_output, crdCurPos);
									}
								}
								bThisInput = FALSE;

								break;
							}
						}

						break;
					case MOUSE_EVENT:
						mouse_record = input_record[i].Event.MouseEvent;
						crdTmp = mouse_record.dwMousePosition;
						crdTmp.X -= m_cdt_Draw.GetOffsetX();
						crdTmp.Y -= m_cdt_Draw.GetOffsetY();

						for (i = 0; i < 2; i++) {
							if (mrYesNo[i].mpLeftUp.dwX <= crdTmp.X && crdTmp.X <= mrYesNo[i].mpRightDown.dwX && \
								mrYesNo[i].mpLeftUp.dwY <= crdTmp.Y && crdTmp.Y <= mrYesNo[i].mpRightDown.dwY) {
								uiCurChoose = i;
								if (mouse_record.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) {
									bReturn = TRUE;
									switch (uiCurChoose) {
									case 0:
										bSkip = FALSE;
										break;
									case 1:
										bSkip = TRUE;
										break;
									default:
										break;
									}
								}
							}
						}
							
						break;
					default:
						break;
					}
				}
			}
		}

		// show the input characters
		mlTmp.uiLeft = _xInput;
		mlTmp.uiUp = _yInput;
		mlTmp.uiNum = dwInputNameReserve;
		uTmp.a = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
		uTmp.c = _T(' ');
		m_cdt_Draw.ClearList(mlTmp, uTmp);

		mlTmp.uiNum = dwCurNameSize;
		puTmp = (Unit*)::HeapAlloc(m_hHeapTmp, HEAP_ZERO_MEMORY, sizeof(Unit)*dwCurNameSize);
		for (i = 0; i < dwCurNameSize; i++) {
			puTmp[i].a = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
			puTmp[i].c = inputName[i];
		}
		m_cdt_Draw.ModifyList(mlTmp, puTmp);
		::HeapFree(m_hHeapTmp, 0, puTmp);

		if (bReturn) {
			if (!bSkip) {
				if (dwCurNameSize) {
					// open file and append rank to it
					inputName[dwCurNameSize] = '\0';
					pFileRank = ::fopen(_SNAKEGAME_RANKING_FILE, "a+");
					if (pFileRank != NULL) {
						m_oe_Link.output_links(NULL, &iTmp);
						iTmp -= 2;
						::fprintf(pFileRank, "%s %u\n", inputName, iTmp);
						::fclose(pFileRank);
					}
				}
				else {
					// notice the name you should click
					bReturn = FALSE;

					_x = (dwX - sizeof(_cc_Notice_Input) / sizeof(TCHAR)) / 2;
					_y = dwY - 1;
					mlTmp.uiLeft = _x;
					mlTmp.uiUp = _y;
					mlTmp.uiNum = sizeof(_cc_Notice_Input) / sizeof(TCHAR) - 1;
					puTmp = (Unit*)::HeapAlloc(m_hHeapTmp, HEAP_ZERO_MEMORY, sizeof(Unit) * sizeof(_cc_Notice_Input));
					for (i = 0; i < mlTmp.uiNum; i++) {
						puTmp[i].a = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
						puTmp[i].c = _cc_Notice_Input[i];
					}
					m_cdt_Draw.ModifyList(mlTmp, puTmp);
					::HeapFree(m_hHeapTmp, 0, puTmp);

				}
			}
		}

		::Sleep(10);
	}

	bRet = ::SetConsoleCursorPosition(m_hConsole_output, crdInitPos);
	this->ClearMap();
	return 1;
}

int SnakeGame::GUISettings()
{
	printf("Settings ok\n");

	const char pStrSettings[] = "Settings";
	const char pStrReturn[] = "Return";
	const char pLeftArrow = '<';
	const char pRightArrow = '>';
	char pStrNumber[20];
	Game_Settings gsTmp;
	FILE *pFileSettings;

	unsigned int uiOffset_X;
	unsigned int uiOffset_Y;

	unsigned int uiMaxIndex_X;
	unsigned int uiMaxIndex_Y;
	unsigned int uiCurIndex_X;
	unsigned int uiCurIndex_Y;

	const unsigned int cuiMaxCharPerRow = 40;
	const unsigned int cuiIntervalSubTitle = 5;

	unsigned int _x;
	unsigned int _y;
	unsigned int uiTmp;
	size_t stSize, stSize2;

	INPUT_RECORD input_record[5];
	KEY_EVENT_RECORD key_record;
	MOUSE_EVENT_RECORD mouse_record;

	MList mlTmp;
	PUnit puTmp;
	MOne moTmp;
	Unit uTmp;

	int i, j, k, m, n;
	BOOL bRet;
	DWORD dwRead;
	DWORD dwTmp;

	BOOL bComplete = FALSE;
	BOOL bReDraw = TRUE;

	uiMaxIndex_X = 1;
	uiMaxIndex_Y = 4;
	uiCurIndex_X = 0;
	uiCurIndex_Y = 1;

	this->ClearMap();

	while (!bComplete) {

		// get the input information
		if (::GetNumberOfConsoleInputEvents(m_hConsole_input, &dwTmp) && dwTmp > 0) {
			bRet = ::ReadConsoleInput(m_hConsole_input, input_record, 5, &dwRead);
			if (bRet) {
				for (i = 0; i < dwRead; i++) {
					switch (input_record[i].EventType)
					{
					case KEY_EVENT:
						key_record = input_record[i].Event.KeyEvent;
						if (key_record.bKeyDown) {
							switch (key_record.wVirtualKeyCode) {
							case VK_UP:
								if (uiCurIndex_Y > 1) {
									uiCurIndex_Y--;
									bReDraw = TRUE;
								}
								break;
							case VK_DOWN:
								if (uiCurIndex_Y <= 3) {
									uiCurIndex_Y++;
									bReDraw = TRUE;
								}
								break;
							case VK_RETURN:

								if (uiCurIndex_Y == 4)
									bComplete = TRUE;

								break;
							case VK_LEFT:
								if (uiCurIndex_Y == 1) {
									if (uiCurIndex_X >= 1) {
										uiCurIndex_X--;
										bReDraw = TRUE;
									}
								}
								else if (2 <= uiCurIndex_Y && uiCurIndex_Y <= 3) {
									bReDraw = TRUE;
									switch (uiCurIndex_X) {
									case 0:	// Game
										switch (uiCurIndex_Y) {
										case 2:	// not used currently

											break;
										case 3:
											if (m_Settings.gsSettings.uiSnakeSpeed < 50 + 10 * key_record.wRepeatCount)
												m_Settings.gsSettings.uiSnakeSpeed = 50;
											else
												m_Settings.gsSettings.uiSnakeSpeed -= 10 * key_record.wRepeatCount;

											break;
										default:
											break;
										}
										break;
									case 1:	// Voice
										switch (uiCurIndex_Y) {
										case 2:	
											if (m_Settings.vsSettings.uiAnimationVoice < 5 * key_record.wRepeatCount)
												m_Settings.vsSettings.uiAnimationVoice = 0;
											else
												m_Settings.vsSettings.uiAnimationVoice -= 5 * key_record.wRepeatCount;

											break;
										case 3:
											if (m_Settings.vsSettings.uiClickVoice < 5 * key_record.wRepeatCount)
												m_Settings.vsSettings.uiClickVoice = 0;
											else
												m_Settings.vsSettings.uiClickVoice -= 5 * key_record.wRepeatCount;

											break;
										default:
											break;
										}

										break;
									default:
										break;
									}

								}

								break;
							case VK_RIGHT:
								if (uiCurIndex_Y == 1) {
									if (uiCurIndex_X < 1) {
										uiCurIndex_X++;
										bReDraw = TRUE;
									}
								}
								else if (2 <= uiCurIndex_Y && uiCurIndex_Y <= 3) {
									bReDraw = TRUE;
									switch (uiCurIndex_X) {
									case 0:	// Game
										switch (uiCurIndex_Y) {
										case 2:	// not used currently

											break;
										case 3:
											if (m_Settings.gsSettings.uiSnakeSpeed + 10 * key_record.wRepeatCount > 10000)
												m_Settings.gsSettings.uiSnakeSpeed == 10000;
											else
												m_Settings.gsSettings.uiSnakeSpeed += 10 * key_record.wRepeatCount;
											
											break;
										default:
											break;
										}

										break;
									case 1:	// Voice
										switch (uiCurIndex_Y) {
										case 2:
											if (m_Settings.vsSettings.uiAnimationVoice + 5 * key_record.wRepeatCount > 100)
												m_Settings.vsSettings.uiAnimationVoice = 100;
											else
												m_Settings.vsSettings.uiAnimationVoice += 5 * key_record.wRepeatCount;

											break;

										case 3:
											if (m_Settings.vsSettings.uiClickVoice + 5 * key_record.wRepeatCount > 100)
												m_Settings.vsSettings.uiClickVoice = 100;
											else
												m_Settings.vsSettings.uiClickVoice += 5 * key_record.wRepeatCount;

											break;
										default:
											break;
										}
										break;
									default:
										break;
									}
								}
								
								break;

							default:
								break;
							}
						}

						break;
					case MOUSE_EVENT:
						mouse_record = input_record[i].Event.MouseEvent;

						break;
					default:
						break;
					}
				}
			}
		}

		// draw the current interface
		if (bReDraw) {
			this->ClearMap();
				// show Main Title
			stSize = ::strlen(pStrSettings);
			_x = (m_MapSize.dwX - stSize) / 2;
			_y = (m_MapSize.dwY - 9) / 2;
			mlTmp.uiLeft = _x;
			mlTmp.uiUp = _y;
			mlTmp.uiNum = stSize;
			puTmp = (PUnit)::HeapAlloc(m_hHeapTmp, HEAP_ZERO_MEMORY, sizeof(Unit)*stSize);
			for (i = 0; i < stSize; i++) {
				puTmp[i].a = FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN;
				puTmp[i].c = pStrSettings[i];
			}
			m_cdt_Draw.ModifyList(mlTmp, puTmp);
			::HeapFree(m_hHeapTmp, 0, puTmp);

				// show Sub Title
			_y += 2;

			stSize = ::strlen(m_cc_Settings[0]);
			stSize2 = ::strlen(m_cc_Settings[1]);
			_x = (m_MapSize.dwX - stSize - stSize2 - cuiIntervalSubTitle) / 2;
			mlTmp.uiUp = _y;
			mlTmp.uiLeft = _x;
			mlTmp.uiNum = stSize;
			puTmp = (PUnit)::HeapAlloc(m_hHeapTmp, HEAP_ZERO_MEMORY, sizeof(Unit)*stSize);
			for (i = 0; i < stSize; i++) {
				puTmp[i].c = m_cc_Settings[0][i];
				if (uiCurIndex_X == 0)
					puTmp[i].a = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
				else
					puTmp[i].a = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
			}
			m_cdt_Draw.ModifyList(mlTmp, puTmp);
			::HeapFree(m_hHeapTmp, 0, puTmp);

			_x += stSize2 + cuiIntervalSubTitle;
			mlTmp.uiLeft = _x;
			mlTmp.uiNum = stSize2;
			puTmp = (PUnit)::HeapAlloc(m_hHeapTmp, HEAP_ZERO_MEMORY, sizeof(Unit)*stSize2);
			for (i = 0; i < stSize2; i++) {
				puTmp[i].c = m_cc_Settings[1][i];
				if (uiCurIndex_X == 1)
					puTmp[i].a = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
				else
					puTmp[i].a = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
			}
			m_cdt_Draw.ModifyList(mlTmp, puTmp);
			::HeapFree(m_hHeapTmp, 0, puTmp);

				// show Menu

			switch (uiCurIndex_X) {
			case 0:		// Game Settings
							// index 0
				_y += 2;
				stSize = ::strlen(m_cc_GameSettings[0]);
				_x = (m_MapSize.dwX - cuiMaxCharPerRow) / 2;
				mlTmp.uiLeft = _x;
				mlTmp.uiUp = _y;
				mlTmp.uiNum = stSize;
				puTmp = (PUnit)::HeapAlloc(m_hHeapTmp, HEAP_ZERO_MEMORY, sizeof(Unit)*stSize);
				for (j = 0; j < stSize; j++) {
					puTmp[j].c = m_cc_GameSettings[0][j];
					puTmp[j].a = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
				}
				m_cdt_Draw.ModifyList(mlTmp, puTmp);
				::HeapFree(m_hHeapTmp, 0, puTmp);

				uiTmp = m_Settings.gsSettings.uiGameMode;
				::_itoa(uiTmp, pStrNumber, 10);
				stSize2 = ::strlen(pStrNumber);

				_x = _x + cuiMaxCharPerRow - stSize2 - 2;
				moTmp.uiLeft = _x;
				moTmp.uiUp = _y;
				uTmp.a = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
				uTmp.c = pLeftArrow;
				m_cdt_Draw.ModifyOne(moTmp, uTmp);

				_x++;
				mlTmp.uiLeft = _x;
				mlTmp.uiNum = stSize2;
				puTmp = (PUnit)::HeapAlloc(m_hHeapTmp, HEAP_ZERO_MEMORY, sizeof(Unit)*stSize2);
				for (j = 0; j < stSize2; j++)
					puTmp[j].c = pStrNumber[j];
				if (uiCurIndex_Y == 2)
					for (j = 0; j < stSize2; j++)
						puTmp[j].a = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
				else
					for (j = 0; j < stSize2; j++)
						puTmp[j].a = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
				m_cdt_Draw.ModifyList(mlTmp, puTmp);
				::HeapFree(m_hHeapTmp, 0, puTmp);

				_x += stSize2;
				moTmp.uiLeft = _x;
				moTmp.uiUp = _y;
				uTmp.c = pRightArrow;
				m_cdt_Draw.ModifyOne(moTmp, uTmp);
							// index 1
				_y += 2;
				_x = (m_MapSize.dwX - cuiMaxCharPerRow) / 2;
				stSize = ::strlen(m_cc_GameSettings[1]);
				mlTmp.uiLeft = _x;
				mlTmp.uiUp = _y;
				mlTmp.uiNum = stSize;
				puTmp = (PUnit)::HeapAlloc(m_hHeapTmp, HEAP_ZERO_MEMORY, sizeof(Unit)*stSize);
				for (i = 0; i < stSize; i++) {
					puTmp[i].a = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
					puTmp[i].c = m_cc_GameSettings[1][i];
				}
				m_cdt_Draw.ModifyList(mlTmp, puTmp);
				::HeapFree(m_hHeapTmp, 0, puTmp);

				_x = _x + cuiMaxCharPerRow - stSize2 - 2;
				moTmp.uiLeft = _x;
				moTmp.uiUp = _y;
				uTmp.a = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
				uTmp.c = pLeftArrow;
				m_cdt_Draw.ModifyOne(moTmp, uTmp);

				uiTmp = m_Settings.gsSettings.uiSnakeSpeed;
				::_itoa(uiTmp, pStrNumber, 10);
				stSize2 = ::strlen(pStrNumber);
				_x++;
				mlTmp.uiLeft = _x;
				mlTmp.uiNum = stSize2;
				puTmp = (PUnit)::HeapAlloc(m_hHeapTmp, HEAP_ZERO_MEMORY, sizeof(Unit)*stSize2);
				for (j = 0; j < stSize2; j++)
					puTmp[j].c = pStrNumber[j];
				if (uiCurIndex_Y == 3)
					for (j = 0; j < stSize2; j++)
						puTmp[j].a = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
				else
					for (j = 0; j < stSize2; j++)
						puTmp[j].a = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
				m_cdt_Draw.ModifyList(mlTmp, puTmp);
				::HeapFree(m_hHeapTmp, 0, puTmp);

				_x += stSize2;
				moTmp.uiLeft = _x;
				moTmp.uiUp = _y;
				uTmp.c = pRightArrow;
				m_cdt_Draw.ModifyOne(moTmp, uTmp);

				break;
			case 1:		// Voice Settings
				
					// index 0
				_y += 2;
				stSize = ::strlen(m_cc_VoiceSettings[0]);
				_x = (m_MapSize.dwX - cuiMaxCharPerRow) / 2;
				mlTmp.uiLeft = _x;
				mlTmp.uiUp = _y;
				mlTmp.uiNum = stSize;
				puTmp = (PUnit)::HeapAlloc(m_hHeapTmp, HEAP_ZERO_MEMORY, sizeof(Unit)*stSize);
				for (j = 0; j < stSize; j++) {
					puTmp[j].c = m_cc_VoiceSettings[0][j];
					puTmp[j].a = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
				}
				m_cdt_Draw.ModifyList(mlTmp, puTmp);
				::HeapFree(m_hHeapTmp, 0, puTmp);

				uiTmp = m_Settings.vsSettings.uiAnimationVoice;
				::_itoa(uiTmp, pStrNumber, 10);
				stSize2 = ::strlen(pStrNumber);

				_x = _x + cuiMaxCharPerRow - stSize2 - 2;
				moTmp.uiLeft = _x;
				moTmp.uiUp = _y;
				uTmp.a = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
				uTmp.c = pLeftArrow;
				m_cdt_Draw.ModifyOne(moTmp, uTmp);

				_x++;
				mlTmp.uiLeft = _x;
				mlTmp.uiNum = stSize2;
				puTmp = (PUnit)::HeapAlloc(m_hHeapTmp, HEAP_ZERO_MEMORY, sizeof(Unit)*stSize2);
				for (j = 0; j < stSize2; j++)
					puTmp[j].c = pStrNumber[j];
				if (uiCurIndex_Y == 2)
					for (j = 0; j < stSize2; j++)
						puTmp[j].a = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
				else
					for (j = 0; j < stSize2; j++)
						puTmp[j].a = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
				m_cdt_Draw.ModifyList(mlTmp, puTmp);
				::HeapFree(m_hHeapTmp, 0, puTmp);

				_x += stSize2;
				moTmp.uiLeft = _x;
				moTmp.uiUp = _y;
				uTmp.c = pRightArrow;
				m_cdt_Draw.ModifyOne(moTmp, uTmp);
					// index 1
				_y += 2;
				_x = (m_MapSize.dwX - cuiMaxCharPerRow) / 2;
				stSize = ::strlen(m_cc_VoiceSettings[1]);
				mlTmp.uiLeft = _x;
				mlTmp.uiUp = _y;
				mlTmp.uiNum = stSize;
				puTmp = (PUnit)::HeapAlloc(m_hHeapTmp, HEAP_ZERO_MEMORY, sizeof(Unit)*stSize);
				for (i = 0; i < stSize; i++) {
					puTmp[i].a = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
					puTmp[i].c = m_cc_VoiceSettings[1][i];
				}
				m_cdt_Draw.ModifyList(mlTmp, puTmp);
				::HeapFree(m_hHeapTmp, 0, puTmp);

				_x = _x + cuiMaxCharPerRow - stSize2 - 2;
				moTmp.uiLeft = _x;
				moTmp.uiUp = _y;
				uTmp.a = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
				uTmp.c = pLeftArrow;
				m_cdt_Draw.ModifyOne(moTmp, uTmp);

				uiTmp = m_Settings.vsSettings.uiClickVoice;
				::_itoa(uiTmp, pStrNumber, 10);
				stSize2 = ::strlen(pStrNumber);
				_x++;
				mlTmp.uiLeft = _x;
				mlTmp.uiNum = stSize2;
				puTmp = (PUnit)::HeapAlloc(m_hHeapTmp, HEAP_ZERO_MEMORY, sizeof(Unit)*stSize2);
				for (j = 0; j < stSize2; j++)
					puTmp[j].c = pStrNumber[j];
				if (uiCurIndex_Y == 3)
					for (j = 0; j < stSize2; j++)
						puTmp[j].a = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
				else
					for (j = 0; j < stSize2; j++)
						puTmp[j].a = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
				m_cdt_Draw.ModifyList(mlTmp, puTmp);
				::HeapFree(m_hHeapTmp, 0, puTmp);

				_x += stSize2;
				moTmp.uiLeft = _x;
				moTmp.uiUp = _y;
				uTmp.c = pRightArrow;
				m_cdt_Draw.ModifyOne(moTmp, uTmp);

				break;
			default:
				break;
			}

				// Show Return
			_y += 2;
			stSize = ::strlen(pStrReturn);
			_x = (m_MapSize.dwX - stSize) / 2;
			mlTmp.uiLeft = _x;
			mlTmp.uiUp = _y;
			mlTmp.uiNum = stSize;
			puTmp = (PUnit)::HeapAlloc(m_hHeapTmp, HEAP_ZERO_MEMORY, sizeof(Unit)*stSize);
			for (i = 0; i < stSize; i++)
				puTmp[i].c = pStrReturn[i];
			if (uiCurIndex_Y == 4)
				for (i = 0; i < stSize; i++)
					puTmp[i].a = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
			else
				for (i = 0; i < stSize; i++)
					puTmp[i].a = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
			m_cdt_Draw.ModifyList(mlTmp, puTmp);
			::HeapFree(m_hHeapTmp, 0, puTmp);

			bReDraw = FALSE;
		}

		::Sleep(10);
	}

	this->SaveSettings();
	this->ClearMap();
	return 0;
}

int SnakeGame::GUIRank()
{
	this->ClearMap();

	const TCHAR _cc_RankingList[] = _T("Ranking List");
	const TCHAR _cc_Return[] = _T(" Return ");
	BOOL bReturn = FALSE;
	Map_Rect mrReturn;
	unsigned int uiCurPageIndex;
	unsigned int uiAllPageIndex;
	unsigned int uiCurItemNum;
	const unsigned int cuiItemsShowPerPage = 5;

	MList mlTmp;
	Unit *puTmp;
	char _pcScore[10];
	const unsigned int cuiScoreMaxLength = 5;
	char _pcStrPage[40];

	FILE *pFileRank;

	unsigned int uiRankListNum = 0;
	unsigned int uiRankListNumReserve = 0;
	const unsigned int cuiRankListNumReserveDelta = 20;
	const unsigned int cuiRankItemMarginWidth = 10;
	const unsigned int cuiRankItemInterval = 2;
	unsigned int _x, _y;

	int i, j;

	size_t stTmp, stTmp2, stNameMaxSize = 0;

	Game_Record *ptGame_Record = NULL, grTmp;

	INPUT_RECORD input_record[5];
	KEY_EVENT_RECORD key_record;
	MOUSE_EVENT_RECORD mouse_record;
	COORD crdTmp;
	BOOL bRet;
	DWORD dwSize;

	pFileRank = ::fopen(_SNAKEGAME_RANKING_FILE, "a+");
	if (pFileRank != NULL) {
		uiRankListNumReserve += cuiRankListNumReserveDelta;
		ptGame_Record = (Game_Record*)::HeapAlloc(m_hHeapTmp, HEAP_ZERO_MEMORY, sizeof(Game_Record)*uiRankListNumReserve);
		for (i = 0; i < uiRankListNumReserve; i++) {
			ptGame_Record[i].ptName = (char*)::HeapAlloc(m_hHeapTmp, HEAP_ZERO_MEMORY, sizeof(char)*_SNAKEGAME_RANK_MAX_NAME_NUM);
			ptGame_Record[i].uiScore = 0;
		}

		while (!feof(pFileRank)) {
			if (uiRankListNum + 1 >= uiRankListNumReserve) {
				uiRankListNumReserve += cuiRankListNumReserveDelta;
				ptGame_Record = (Game_Record*)::HeapReAlloc(m_hHeapTmp, HEAP_ZERO_MEMORY, ptGame_Record, sizeof(Game_Record)*uiRankListNumReserve);
				for (i = uiRankListNumReserve - cuiRankListNumReserveDelta; i < uiRankListNumReserve; i++)
					ptGame_Record[i].ptName = (char*)::HeapAlloc(m_hHeapTmp, HEAP_ZERO_MEMORY, sizeof(char)*_SNAKEGAME_RANK_MAX_NAME_NUM);
			}
			::fscanf(pFileRank, "%s %u\n", ptGame_Record[uiRankListNum].ptName, &(ptGame_Record[uiRankListNum].uiScore));
			uiRankListNum++;
		}

		if (uiRankListNum == 1 && ptGame_Record[0].uiScore == 0) {
			// there is no rank list existing.
			printf("There is no rank list existing.\n");

		}
		else {
			// firstly, should delete the repeat record
			for (i = 0; i < uiRankListNum - 1; i++)
				for (j = i+1; j < uiRankListNum; j++) {
					if (::strcmp(ptGame_Record[i].ptName, ptGame_Record[j].ptName) == 0) {
						if (ptGame_Record[i].uiScore < ptGame_Record[j].uiScore) {
							grTmp = ptGame_Record[i];
							ptGame_Record[i] = ptGame_Record[j];
							ptGame_Record[j] = grTmp;
						}

						grTmp = ptGame_Record[j];
						ptGame_Record[j] = ptGame_Record[uiRankListNum - 1];
						ptGame_Record[uiRankListNum - 1] = grTmp;
						j--;
						uiRankListNum--;
					}
				}

			// secondly, sort the rank list in order
			for (i = 0; i < uiRankListNum - 1; i++)
				for (j = i + 1; j < uiRankListNum; j++)
					if (ptGame_Record[i].uiScore < ptGame_Record[j].uiScore) {
						grTmp = ptGame_Record[i];
						ptGame_Record[i] = ptGame_Record[j];
						ptGame_Record[j] = grTmp;
					}

			// thirdly, display the ranking
				// get the max name length
			uiCurItemNum = cuiItemsShowPerPage;
			for (i = 0; i < uiRankListNum; i++) {
				stTmp = ::strlen(ptGame_Record[i].ptName);
				if (stTmp > stNameMaxSize)
					stNameMaxSize = stTmp;
			}
			bReturn = FALSE;
			uiCurPageIndex = 0;
			if (uiRankListNum % cuiItemsShowPerPage == 0)
				uiAllPageIndex = uiRankListNum / cuiItemsShowPerPage;
			else
				uiAllPageIndex = uiRankListNum / cuiItemsShowPerPage + 1;
				// enter the cycle
			uiAllPageIndex--;
			while (!bReturn) {
				// show current page
				_y = (m_MapSize.dwY - (7 * 2 - 1)) / 2;

				bRet = ::StringCchLength(_cc_RankingList, 40, &stTmp);
				_x = (m_MapSize.dwX - stTmp) / 2;
				mlTmp.uiLeft = _x;
				mlTmp.uiUp = _y;
				mlTmp.uiNum = stTmp;
				puTmp = (Unit*)::HeapAlloc(m_hHeapTmp, HEAP_ZERO_MEMORY, sizeof(Unit)*stTmp);
				for (i = 0; i < stTmp; i++) {
					puTmp[i].a = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
					puTmp[i].c = _cc_RankingList[i];
				}
				m_cdt_Draw.ModifyList(mlTmp, puTmp);
				::HeapFree(m_hHeapTmp, 0, puTmp);

				_y += 2;
				if (uiAllPageIndex == 0)
					uiCurItemNum = uiRankListNum;
				else if (uiCurPageIndex == uiAllPageIndex)
					uiCurItemNum = uiRankListNum - cuiItemsShowPerPage * uiAllPageIndex;
				else
					uiCurItemNum = cuiItemsShowPerPage;
				
				for (i = uiCurPageIndex * cuiItemsShowPerPage; i < uiCurItemNum + uiCurPageIndex * cuiItemsShowPerPage; i++) {
					::_itoa_s(ptGame_Record[i].uiScore, _pcScore, sizeof(_pcScore) - 1, 10);
					stTmp = ::strlen(_pcScore);
					_x = (m_MapSize.dwX - cuiScoreMaxLength - cuiRankItemInterval - stNameMaxSize) / 2;
					mlTmp.uiLeft = _x;
					mlTmp.uiUp = _y + 2 * (i - uiCurPageIndex * cuiItemsShowPerPage);
					mlTmp.uiNum = stTmp;
					puTmp = (Unit*)::HeapAlloc(m_hHeapTmp, HEAP_ZERO_MEMORY, sizeof(Unit)*stTmp);
					for (j = 0; j < stTmp; j++) {
						puTmp[j].a = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
						puTmp[j].c = _pcScore[j];
					}
					m_cdt_Draw.ModifyList(mlTmp, puTmp);
					::HeapFree(m_hHeapTmp, 0, puTmp);

					stTmp2 = ::strlen(ptGame_Record[i].ptName);
					_x = (m_MapSize.dwX - _x - stTmp2);
					mlTmp.uiLeft = _x;
					mlTmp.uiUp = _y + 2 * (i - uiCurPageIndex * cuiItemsShowPerPage);
					mlTmp.uiNum = stTmp2;
					puTmp = (Unit*)::HeapAlloc(m_hHeapTmp, HEAP_ZERO_MEMORY, sizeof(Unit)*stTmp2);
					for (j = 0; j < stTmp2; j++) {
						puTmp[j].a = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
						puTmp[j].c = ((ptGame_Record[i]).ptName)[j];
					}
					m_cdt_Draw.ModifyList(mlTmp, puTmp);
					::HeapFree(m_hHeapTmp, 0, puTmp);

				}

				// show current page / all page
				_y += cuiItemsShowPerPage * 2;
				if (uiAllPageIndex > 0)
					wsprintfA(_pcStrPage, "Page %u / All %u Pages\0", uiCurPageIndex + 1, uiAllPageIndex + 1);
				else
					wsprintfA(_pcStrPage, "Page %u / All %u Page", uiCurPageIndex + 1, uiAllPageIndex + 1);
				stTmp = ::strlen(_pcStrPage);
				_x = (m_MapSize.dwX - stTmp) / 2;
				mlTmp.uiLeft = _x;
				mlTmp.uiUp = _y;
				mlTmp.uiNum = stTmp;
				puTmp = (Unit*)::HeapAlloc(m_hHeapTmp, HEAP_ZERO_MEMORY, sizeof(Unit)*stTmp);
				for (j = 0; j < stTmp; j++) {
					puTmp[j].a = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
					puTmp[j].c = _pcStrPage[j];
				}
				m_cdt_Draw.ModifyList(mlTmp, puTmp);
				::HeapFree(m_hHeapTmp, 0, puTmp);

				// show return
				_y += 2;
				bRet = ::StringCchLength(_cc_Return, 40, &stTmp);
				_x = (m_MapSize.dwX - stTmp) / 2;
				mlTmp.uiLeft = _x;
				mlTmp.uiUp = _y;
				mlTmp.uiNum = stTmp;
				mrReturn.mpLeftUp.dwX = _x;
				mrReturn.mpLeftUp.dwY = _y;
				mrReturn.mpRightDown.dwX = _x + stTmp - 1;
				mrReturn.mpRightDown.dwY = _y;
				puTmp = (Unit*)::HeapAlloc(m_hHeapTmp, HEAP_ZERO_MEMORY, sizeof(Unit)*stTmp);
				for (j = 0; j < stTmp; j++) {
					puTmp[j].a = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
					puTmp[j].c = _cc_Return[j];
				}
				m_cdt_Draw.ModifyList(mlTmp, puTmp);
				::HeapFree(m_hHeapTmp, 0, puTmp);

				// get the input event
				if (::GetNumberOfConsoleInputEvents(m_hConsole_input, &dwSize) && dwSize > 0) {
					bRet = ::ReadConsoleInput(m_hConsole_input, input_record, 5, &dwSize);
					if (bRet) {
						for (i = 0; i < dwSize; i++) {
							switch (input_record[i].EventType)
							{
							case KEY_EVENT:
								key_record = input_record[i].Event.KeyEvent;
								if (key_record.bKeyDown) {
									switch (key_record.wVirtualKeyCode)
									{
									case VK_LEFT:
										if (uiCurPageIndex > 0)
											uiCurPageIndex--;
										this->ClearMap();
										break;
									case VK_RIGHT:
										if (uiCurPageIndex < uiAllPageIndex)
											uiCurPageIndex++;
										this->ClearMap();
										break;
									case VK_RETURN:
										bReturn = TRUE;
										break;
									default:
										break;
									}
								}
								
								break;
							case MOUSE_EVENT:
								mouse_record = input_record[i].Event.MouseEvent;
								crdTmp = mouse_record.dwMousePosition;
								crdTmp.X -= m_cdt_Draw.GetOffsetX();
								crdTmp.Y -= m_cdt_Draw.GetOffsetY();
								if (mrReturn.mpLeftUp.dwX <= crdTmp.X && \
									crdTmp.X <= mrReturn.mpRightDown.dwX && \
									mrReturn.mpLeftUp.dwY <= crdTmp.Y && \
									crdTmp.Y <= mrReturn.mpRightDown.dwY) {
									if (mouse_record.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED) {
										bReturn = TRUE;
									}
								}
								break;
							default:
								break;
							}
						}
					}
				}

				::Sleep(10);
			}

		}
		::fclose(pFileRank);

		pFileRank = ::fopen(_SNAKEGAME_RANKING_FILE, "w+");
		for (i = 0; i < uiRankListNum; i++)
			::fprintf(pFileRank, "%s %u\n", ptGame_Record[i].ptName, ptGame_Record[i].uiScore);
		::fclose(pFileRank);

		for (i = 0; i < uiRankListNumReserve; i++)
			::HeapFree(m_hHeapTmp, 0, ptGame_Record[i].ptName);
		::HeapFree(m_hHeapTmp, 0, ptGame_Record);
	}

	this->ClearMap();

	return 1;
}

int SnakeGame::GUIHelp()
{
	this->ClearMap();

	int i, j, _x, _y;
	INPUT_RECORD input_record[5];
	KEY_EVENT_RECORD key_record;
	MOUSE_EVENT_RECORD mouse_record;
	COORD crdTmp;

	Map_Rect mrReturnPos;

	MList mListTmp;
	PUnit puTmp = NULL;
	size_t stTmp;
	HRESULT hRet;
	
	BOOL bReturn = FALSE;
	BOOL bRet;
	DWORD dwTmp;

	const TCHAR _cc_ReturnText[] = _T("  Return  ");
	const TCHAR _cc_HelpText[7][40] = {
		{_T("How to Operate")},
		{_T(" ")},
		{_T("Leftward:        <LEFT>  or <A>")},
		{_T("Rightward:       <RIGHT> or <D>")},
		{_T("Upward:          <UP>    or <W>")},
		{_T("Downward:        <DOWN>  or <S>")},
		{_T("Pause/Continue:  <P>           ")}
	};

	_y = (m_MapSize.dwY - 7) / 2 - 1;

	for (i = 0; i < 7; i++) {
		hRet = ::StringCchLength(_cc_HelpText[i], 40, &stTmp);
		_x = (m_MapSize.dwX - stTmp) / 2 - 1;

		mListTmp.uiLeft = _x;
		mListTmp.uiUp = _y;
		mListTmp.uiNum = stTmp;

		puTmp = (PUnit)::HeapAlloc(m_hHeapTmp, HEAP_ZERO_MEMORY, stTmp * sizeof(Unit));
		for (j = 0; j < stTmp; j++) {
			puTmp[j].a = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
			puTmp[j].c = _cc_HelpText[i][j];
		}

		m_cdt_Draw.ModifyList(mListTmp, puTmp);

		if (puTmp != NULL) {
			::HeapFree(m_hHeapTmp, 0, puTmp);
			puTmp = NULL;
		}
		_y++;
	}

	hRet = ::StringCchLength(_cc_ReturnText, 40, &stTmp);
	_x = (m_MapSize.dwX - stTmp) / 2 - 1;
	_y += 2;
	mListTmp.uiLeft = _x;
	mListTmp.uiUp = _y;
	mListTmp.uiNum = stTmp;

	mrReturnPos.mpRightDown.dwX = mrReturnPos.mpLeftUp.dwX = m_cdt_Draw.GetOffsetX() + _x;
	mrReturnPos.mpRightDown.dwY = mrReturnPos.mpLeftUp.dwY = m_cdt_Draw.GetOffsetY() + _y;
	mrReturnPos.mpRightDown.dwX += stTmp;

	puTmp = (PUnit)::HeapAlloc(m_hHeapTmp, HEAP_ZERO_MEMORY, stTmp * sizeof(Unit));
	for (j = 0; j < stTmp; j++) {
		puTmp[j].a = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;;
		puTmp[j].c = _cc_ReturnText[j];
	}
	m_cdt_Draw.ModifyList(mListTmp, puTmp);
	if (puTmp != NULL) {
		::HeapFree(m_hHeapTmp, 0, puTmp);
		puTmp = NULL;
	}
	

	while (!bReturn) {
		bRet = ::ReadConsoleInput(m_hConsole_input, input_record, 5, &dwTmp);
		if (bRet) {
			for (i = 0; i < dwTmp; i++) {
				if (input_record[i].EventType == KEY_EVENT) {
					key_record = input_record[i].Event.KeyEvent;
					if (key_record.bKeyDown && key_record.wVirtualKeyCode == VK_RETURN)
						bReturn = TRUE;
				}
				else if (input_record[i].EventType == MOUSE_EVENT) {
					mouse_record = input_record[i].Event.MouseEvent;
					if (mouse_record.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED) {
						crdTmp = mouse_record.dwMousePosition;
						if (mrReturnPos.mpLeftUp.dwX <= crdTmp.X && \
							crdTmp.X <= mrReturnPos.mpRightDown.dwX && \
							mrReturnPos.mpLeftUp.dwY <= crdTmp.Y && \
							crdTmp.Y <= mrReturnPos.mpRightDown.dwY)
							bReturn = TRUE;
					}
				}
			}
		}

	}

	this->ClearMap();

	return 0;
}

int SnakeGame::GUIExit()
{
	exit(0);
	return 0;
}

int SnakeGame::ClearMap()
{
	MRect mrTmp;
	Unit uTmp;

	mrTmp.uiLeft = 0;
	mrTmp.uiUp = 0;
	mrTmp.uiColumn = m_MapSize.dwX;
	mrTmp.uiRow = m_MapSize.dwY;

	uTmp.c = _T(' ');
	
	return m_cdt_Draw.ClearRect(mrTmp);

}

#endif
