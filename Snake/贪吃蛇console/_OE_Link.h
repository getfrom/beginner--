#pragma once
#include "stdafx.h"
#ifdef _DEBUG_
#include <iostream>
using std::cout;
using std::endl;
#endif

#ifndef _KERNEL32_LIB_
#pragma comment(lib,"Kernel32.lib")
#endif

#ifndef _WINDOWS_H_
#define _WINDOWS_H_
#include <Windows.h>
#endif

template<class T>
class _OE_Link
{
public:
	_OE_Link();
	~_OE_Link();

	int new_link();

	int appendAtBack(const T&);
	int insertAtFront(const T&);

	int removeLast();
	int removeIndex(const int&);
	int destroy_link();

	int output_links(T* links, int *iNumber);

#ifdef _DEBUG_
	void print_link();
#endif

private:
	HANDLE m_hHeap;

	struct _Link_Node {
		T tData;
		_Link_Node *pNext;
	};
	typedef _Link_Node* _PLink_Node;

	_PLink_Node m_ptFirst;

	unsigned int m_uiSize;

};

template<class T>
_OE_Link<T>::_OE_Link()
{
	m_hHeap = ::HeapCreate(0, 0, 0);
	if (m_hHeap == NULL)
		throw;
	m_uiSize = 0;
}

template<class T>
_OE_Link<T>::~_OE_Link()
{
	if (m_hHeap != NULL)
		::HeapDestroy(m_hHeap);

}

template<class T>
int _OE_Link<T>::new_link()
{
	if (m_hHeap != NULL)
	{
		::HeapDestroy(m_hHeap);
		m_hHeap = ::HeapCreate(0, 0, 0);
	}

	m_uiSize = 0;
	
	return 1;
}

template<class T>
int _OE_Link<T>::appendAtBack(const T& t)
{
	_Link_Node *pLkndCur, *pLkndBefore = NULL, *lknd_tmp;
	int i;

	lknd_tmp = (_Link_Node*)::HeapAlloc(m_hHeap, HEAP_ZERO_MEMORY, sizeof(_Link_Node));
	lknd_tmp->tData = t;
	if (m_uiSize > 0) {
		lknd_tmp->pNext = m_ptFirst;

		for (pLkndCur = m_ptFirst, i = 0; i < m_uiSize; i++) {
			pLkndBefore = pLkndCur;
			pLkndCur = pLkndCur->pNext;
		}
		if (pLkndBefore != NULL)
			pLkndBefore->pNext = lknd_tmp;
	}
	else {
		lknd_tmp->pNext = lknd_tmp;
		m_ptFirst = lknd_tmp;
	}

	m_uiSize++;
	return 1;
}

template<class T>

int _OE_Link<T>::insertAtFront(const T& t)
{
	this->appendAtBack(t);

	_Link_Node *pLkndCur, *pLkndBefore = NULL;
	int i;
	for (pLkndCur = m_ptFirst, i = 0; i < m_uiSize; i++) {
		pLkndBefore = pLkndCur;
		pLkndCur = pLkndCur->pNext;
	}

	m_ptFirst = pLkndBefore;

	return 1;
}

template<class T>
int _OE_Link<T>::removeLast()
{
	_Link_Node *pLkndCur, *pLkndBefore = NULL;
	int i;
	for (i = 0, pLkndCur = m_ptFirst; i < m_uiSize; i++) {
		pLkndBefore = pLkndCur;
		pLkndCur = pLkndCur->pNext;
	}
	if (pLkndBefore != NULL)
		pLkndBefore->pNext = m_ptFirst;
	
	m_uiSize--;

	return 1;
}

template<class T>
int _OE_Link<T>::removeIndex(const int& index)
{
	int i;
	_PLink_Node pCur, pBefore = NULL;
	for (i = 0, pCur = m_ptFirst; i < m_uiSize && i < index; i++) {
		pBefore = pCur;
		pCur = pCur->pNext
	}

	if (pBefore == NULL)
		return 0;

	pBefore->pNext = pBefore -> pNext -> pNext;
	::HeapFree(m_hHeap, 0, pBefore->pNext);

	m_uiSize--;

	return 1;
}

template<class T>
int _OE_Link<T>::destroy_link()
{
	if (m_hHeap != NULL ) {
		::HeapDestroy(m_hHeap);
		m_hHeap = ::HeapCreate(0, 0, 0);
	}
	m_uiSize = 0;

	return 1;
}

template<class T>
int _OE_Link<T>::output_links(T *links, int *iNumber)
{
	if (links == NULL || *iNumber == 0) {
		*iNumber = m_uiSize;
		return 0;
	}
	else {
		_PLink_Node pCur;
		int i;
		for (i = 0, pCur = m_ptFirst; i < *iNumber && i < m_uiSize; pCur = pCur->pNext) {
			links[i++] = pCur->tData;
		}
		return 1;
	}
}

#ifdef _DEBUG_
template<class T>
void _OE_Link<T>::print_link()
{
	_PLink_Node pCur;
	int i;
	for (pCur = m_ptFirst, i=0; i<m_uiSize; pCur = pCur->pNext, i++)
		std::cout << "(" << pCur->tData << ")->";
	std::cout << std::endl;
}
#endif

