#pragma once

#include "Client_Defines.h"
#include "Base.h"

/* 다음 할당을 위한 레벨의 자원들을 생성한다. */

BEGIN(Client)

class CLoader final : public CBase
{
private:
	explicit CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLoader() = default;

public:
	CRITICAL_SECTION Get_CriticalSection() const {
		return m_CriticalSection;
	}

	LEVEL Get_NextLevelID() const {
		return m_eNextLevelID;
	}

	const _tchar* Get_String() const {
		return m_szLoadingText;
	}

	_bool Get_isFinished() const {
		return m_isFinished;
	}

public:
	HRESULT NativeConstruct(LEVEL eNextLevelID);
	_uint Loading_ForLogo();
	_uint Loading_ForGamePlay();
	_uint Load_MapObject(LEVEL eLevel);

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;
	HANDLE					m_hThread;
	CRITICAL_SECTION		m_CriticalSection;
	LEVEL					m_eNextLevelID = LEVEL_END;


	_tchar				m_szLoadingText[MAX_PATH] = TEXT("");
	_bool				m_isFinished = false;




public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, LEVEL eNextLevelID);
	virtual void Free() override;
};

END