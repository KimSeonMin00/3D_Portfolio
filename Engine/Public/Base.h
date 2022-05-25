#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

/* 객체의 레퍼런스 카운트를 관리한다. */

class ENGINE_DLL CBase abstract
{	
protected:
	CBase() = default;
	virtual ~CBase() = default;

public:
	unsigned long AddRef(); /* 레퍼런슼카운트를 증가시키낟. */
	unsigned long Release();/* 레퍼런슼카운트를 감소시킨다. or 삭제한다. */

private:
	unsigned long			m_dwRefCnt = 0;

public:
	virtual void Free() = 0;

};

END


