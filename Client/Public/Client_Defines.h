#pragma once

#include <process.h>

namespace Client 
{

	const unsigned int g_iWinCX = 1280;
	const unsigned int g_iWinCY = 720;

	/* LEVEL_STATIC : 할당되기위한 레벨은 아니다. 각종 인게임내에서 사용되는 오브젝트들의 매니져들에서 사용. */ 
	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_END };
}

extern HWND g_hWnd;
extern HINSTANCE g_hInst;
using namespace Client;
