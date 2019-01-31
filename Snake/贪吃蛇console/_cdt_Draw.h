#include "stdafx.h"

/**********-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*
*
*
*
***********-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/


#ifndef _WINDOWS_H_
#include<Windows.h>
#define _WINDOWS_H_
#endif


typedef struct _tag_Area{
	unsigned int uiRow;
	unsigned int uiColumn;
}Area, *PArea;

typedef struct _tag_Unit {
	TCHAR c;
	WORD a;
}Unit, *PUnit, **AUnit;


typedef struct _tag_MOne {
	unsigned int uiLeft;
	unsigned int uiUp;
}MOne, *PMOne;

typedef struct _tag_MList {
	unsigned int uiLeft;
	unsigned int uiUp;
	unsigned int uiNum;
}MList, *PMList;

typedef struct _tag_MRect {
	unsigned int uiLeft;
	unsigned int uiUp;
	unsigned int uiRow;
	unsigned int uiColumn;
}MRect, *PMRect;


class _CDT_DRAW {
public:
	_CDT_DRAW();
	~_CDT_DRAW();
	int SetHandle(HANDLE);
	int SetArea(unsigned int, unsigned int);
	DWORD GetOffsetX() { return m_dwOffset_X; }
	DWORD GetOffsetY() { return m_dwOffset_Y; }
	int SetDefaultClear(Unit _u);
	
	int ModifyOne(MOne _m, Unit _u);
	int ModifyList(MList _m, PUnit _pu);
	int ModifyRect(MRect _m, AUnit _au);
	
	int ClearOne(MOne _m, Unit _u = {0,0});
	int ClearList(MList _m, Unit _u = { 0,0 });
	int ClearRect(MRect _m, Unit _u = { 0,0 });
	
private:
	HANDLE m_hHeap;
	HANDLE m_hConsole;
	Area m_Aarea;
	Unit m_Default_Unit;

	DWORD m_dwOffset_X;
	DWORD m_dwOffset_Y;

	int ClearDraw();
};

_CDT_DRAW::_CDT_DRAW()
{
	m_hConsole = INVALID_HANDLE_VALUE;
	m_Aarea = { 0,0 };
	m_hHeap = ::HeapCreate(0, 0, 0);
}


_CDT_DRAW::~_CDT_DRAW()
{
	if (m_hHeap != NULL) {
		::HeapDestroy(m_hHeap);
		m_hHeap = NULL;
	}
}

int _CDT_DRAW::SetHandle(HANDLE hConsole)
{
	m_hConsole = hConsole;
	return 1;
}

int _CDT_DRAW::SetArea(unsigned int uiRow, unsigned int uiColumn)
{
	const TCHAR *tcLeftUp = _T("©°");
	const TCHAR *tcLeftDown = _T("©¸");
	const TCHAR *tcRightUp = _T("©´");
	const TCHAR *tcRightDown = _T("©¼");

	Unit _max_u;
	CONSOLE_SCREEN_BUFFER_INFO _csb_info;
	SMALL_RECT _s_rect;
	SHORT _sCur_Row, _sCur_Column;

	COORD _ord;
	DWORD dwTmp;

	BOOL bState = ::GetConsoleScreenBufferInfo(m_hConsole, &_csb_info);
	_s_rect = _csb_info.srWindow;
	_sCur_Row = _s_rect.Bottom - _s_rect.Top + 1;
	_sCur_Column = _s_rect.Right - _s_rect.Left + 1;

	if (uiRow > _sCur_Row || uiColumn > _sCur_Column)
		return 0;

	m_Aarea.uiRow = uiRow;
	m_Aarea.uiColumn = uiColumn;
	m_dwOffset_X = (_sCur_Column - uiColumn) / 2 - 1;
	m_dwOffset_Y = (_sCur_Row - uiRow) / 2 - 1;

	_ord.X = m_dwOffset_X - 2;	// do not know why dec two ?
	_ord.Y = m_dwOffset_Y - 1;
	bState = ::WriteConsoleOutputCharacter(m_hConsole, (LPTSTR)tcLeftUp, 1, _ord, &dwTmp);
	_ord.X = m_dwOffset_X + uiColumn;
	bState = ::WriteConsoleOutputCharacter(m_hConsole, (LPTSTR)tcRightUp, 1, _ord, &dwTmp);
	_ord.X = m_dwOffset_X - 2;
	_ord.Y = m_dwOffset_Y + uiRow;
	bState = ::WriteConsoleOutputCharacter(m_hConsole, (LPTSTR)tcLeftDown, 1, _ord, &dwTmp);
	_ord.X = m_dwOffset_X + uiColumn;
	bState = ::WriteConsoleOutputCharacter(m_hConsole, (LPTSTR)tcRightDown, 1, _ord, &dwTmp);

	return 1;
}

int _CDT_DRAW::SetDefaultClear(Unit _u)
{
	m_Default_Unit = _u;
	return 1;
}

int _CDT_DRAW::ModifyOne(MOne _m, Unit _u)
{
	COORD _ord;
	BOOL bState_c, bState_a;
	DWORD dwWritten_c = 0, dwWritten_a = 0;
	HANDLE hProHeap = ::GetProcessHeap();
	LPTSTR _pStrChar;
	PWORD _pStrAttr;

	if (_m.uiLeft >= m_Aarea.uiColumn)
		return 0;

	_pStrChar = (LPTSTR)::HeapAlloc(hProHeap, HEAP_ZERO_MEMORY, sizeof(TCHAR) * 2);
	_pStrAttr = (PWORD)::HeapAlloc(hProHeap, HEAP_ZERO_MEMORY, sizeof(TCHAR) * 2);

	_ord.X = m_dwOffset_X + _m.uiLeft;
	_ord.Y = m_dwOffset_Y + _m.uiUp;

	_pStrChar[0] = _u.c;
	_pStrChar[1] = _T('\0');
	_pStrAttr[0] = _u.a;
	_pStrAttr[1] = WORD('\0');

	bState_c = ::WriteConsoleOutputCharacter(m_hConsole, _pStrChar, 1, _ord, &dwWritten_c);
	bState_a = ::WriteConsoleOutputAttribute(m_hConsole, _pStrAttr, 1, _ord, &dwWritten_a);

	if (bState_c && bState_a && dwWritten_c == 1 && dwWritten_a == 1)
		return 1;
	return 0;
}

int _CDT_DRAW::ModifyList(MList _m, PUnit _pu)
{
	COORD _ord;
	BOOL bState_c, bState_a;
	DWORD dwWritten_c, dwWritten_a;
	HANDLE hProHeap = ::GetProcessHeap();

	if (_m.uiLeft + _m.uiNum >= m_Aarea.uiColumn)
		return 0;

	_ord.X = m_dwOffset_X+_m.uiLeft;
	_ord.Y = m_dwOffset_Y+_m.uiUp;

	LPTSTR pChars = (LPTSTR)::HeapAlloc(hProHeap, HEAP_ZERO_MEMORY, sizeof(TCHAR)*(_m.uiNum + 1));
	for (unsigned i = 0; i < _m.uiNum; i++)
		pChars[i] = _pu[i].c;
	pChars[_m.uiNum] = _T('\0');
	bState_c = ::WriteConsoleOutputCharacter(m_hConsole, pChars, _m.uiNum, _ord, &dwWritten_c);
	::HeapFree(hProHeap, 0, pChars);

	WORD *pAttrs = (WORD*)::HeapAlloc(hProHeap, HEAP_ZERO_MEMORY, sizeof(WORD)*(_m.uiNum + 1));
	for (unsigned i = 0; i < _m.uiNum; i++)
		pAttrs[i] = _pu[i].a;
	pAttrs[_m.uiNum] = 0;
	bState_a = ::WriteConsoleOutputAttribute(m_hConsole, pAttrs, _m.uiNum, _ord, &dwWritten_a);
	::HeapFree(hProHeap, 0, pAttrs);

	if (bState_a && bState_c && dwWritten_a == _m.uiNum && dwWritten_c == _m.uiNum)
		return 1;

	return 0;
}

int _CDT_DRAW::ModifyRect(MRect _m, AUnit _au)
{
	COORD _ord;
	BOOL bState_c, bState_a, bCur_State_c, bCur_State_a;
	DWORD dwWritten_c, dwWritten_a, dwCur_Written_c, dwCur_Written_a, dwThis_Need_Written;
	LPTSTR pChars;
	WORD *pAttrs;

	if (_m.uiLeft + _m.uiColumn > m_Aarea.uiColumn)
		return 0;

	bState_c = bState_a = TRUE;
	dwWritten_c = dwWritten_a = 0;
	_ord.X = m_dwOffset_X+_m.uiLeft;
	pChars = (LPTSTR)::HeapAlloc(m_hHeap, HEAP_ZERO_MEMORY, sizeof(TCHAR)*(_m.uiColumn + 1));
	pAttrs = (WORD*)::HeapAlloc(m_hHeap, HEAP_ZERO_MEMORY, sizeof(WORD)*(_m.uiColumn + 1));
	for (unsigned j = 0; j < _m.uiRow; j++)
	{
		_ord.Y = m_dwOffset_Y+_m.uiUp + j;
		

		for (unsigned i = 0; i < _m.uiColumn; i++)
			pChars[i] = _au[j][i].c;
		pChars[_m.uiColumn] = _T('\0');
		bCur_State_c = ::WriteConsoleOutputCharacter(m_hConsole, pChars, _m.uiColumn, _ord, &dwCur_Written_c);
		bState_c &= bCur_State_c;
		dwWritten_c += dwCur_Written_c;

		for (unsigned i = 0; i < _m.uiColumn; i++)
			pAttrs[i] = _au[j][i].a;
		pAttrs[_m.uiColumn] = WORD(0);
		bCur_State_a = ::WriteConsoleOutputAttribute(m_hConsole, pAttrs, _m.uiColumn, _ord, &dwCur_Written_a);
		bState_a &= bCur_State_a;
		dwWritten_a += dwCur_Written_a;

	}

	dwThis_Need_Written = _m.uiColumn * _m.uiRow;

	::HeapFree(m_hHeap, 0, pChars);
	::HeapFree(m_hHeap, 0, pAttrs);

	if (dwWritten_a == dwThis_Need_Written && dwWritten_c == dwThis_Need_Written)
		return 1;

	return 0;
}

int _CDT_DRAW::ClearOne(MOne _m, Unit _u)
{
	Unit _u_i;
	if (_u.a == 0 && _u.c == 0)
		_u_i = m_Default_Unit;
	else
		_u_i = _u;
	
	return this->ModifyOne(_m, _u_i);
}

int _CDT_DRAW::ClearList(MList _m, Unit _u)
{
	HANDLE hProHeap = ::GetProcessHeap();
	PUnit _pu_i = (PUnit)::HeapAlloc(hProHeap, HEAP_ZERO_MEMORY, sizeof(Unit)*(_m.uiNum));
	int iRet;

	if (_u.a == 0 && _u.c == 0)
		for (unsigned i = 0; i < _m.uiNum; i++)
			_pu_i[i] = m_Default_Unit;
	else
		for (unsigned i = 0; i < _m.uiNum; i++)
			_pu_i[i] = _u;

	iRet = this->ModifyList(_m, _pu_i);
	::HeapFree(hProHeap, 0, _pu_i);
	
	return iRet;
}

int _CDT_DRAW::ClearRect(MRect _m, Unit _u)
{
	HANDLE hProHeap = ::GetProcessHeap();
	unsigned uiAll = _m.uiColumn*_m.uiRow;
	int iRet;
	AUnit _au_i = (AUnit)::HeapAlloc(hProHeap, HEAP_ZERO_MEMORY, sizeof(PUnit)*_m.uiRow);
	for (unsigned i = 0; i < _m.uiRow; i++)
		_au_i[i] = (PUnit)::HeapAlloc(hProHeap, HEAP_ZERO_MEMORY, sizeof(Unit)*_m.uiColumn);

	if (_u.a == 0 && _u.c == 0)
		for (unsigned i = 0; i < _m.uiRow; i++)
			for (unsigned j = 0; j < _m.uiColumn; j++)
				_au_i[i][j] = m_Default_Unit;
	else
		for (unsigned i = 0; i < _m.uiRow; i++)
			for (unsigned j = 0; j < _m.uiColumn; j++)
				_au_i[i][j] = _u;

	iRet = this->ModifyRect(_m, _au_i);


	for (unsigned i = 0; i < _m.uiRow; i++)
		::HeapFree(hProHeap, 0, _au_i[i]);
	::HeapFree(hProHeap, 0, _au_i);

	return iRet;
}

