// 贪吃蛇console.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#ifndef _DEBUG_
#define _DEBUG_

#include "SnakeGame.h"

int _main()
{
	int a[] = { 1,2,3,4,5,6,7,8 };
	_OE_Link<int> links;
	links.new_link();
	for (int i = 0; i < sizeof(a) / sizeof(int); i++)
	{
		links.insertAtFront(a[i]);
		links.appendAtBack(a[i]);
	}
	links.print_link();

	links.destroy_link();
	links.new_link();
	int b[] = { 20,10,23,234 };
	for (int i = 0; i < sizeof(b) / sizeof(int); i++)
		links.appendAtBack(b[i]);
	for (int i = 0; i < sizeof(a) / sizeof(int); i++)
		links.insertAtFront(a[i]);
	links.print_link();

	int iNum = 0;
	int *p = NULL;
	links.output_links(NULL, &iNum);
	p = (int*)::malloc(sizeof(int)*iNum);
	links.output_links(p, &iNum);
	for (int i = 0; i < iNum; i++)
		printf("%d ", p[i]);
	printf("\n");
	system("pause");
	for (int i = 0; i < 3; i++)
		links.removeLast();
	links.print_link();

    return 0;
}

int main()
{
	SnakeGame _game;
	_game.initGame();
	_game.GUIMain();

	return 0;
}


#endif	// #define _DEBUG_

