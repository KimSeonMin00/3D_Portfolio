#pragma once

#include "Base.h"

BEGIN(Engine)

class CInput_Device final : public CBase
{
	DECLARE_SINGLETON(CInput_Device)

public:
	enum DIMM { DIMM_X, DIMM_Y, DIMM_WHEEL, DIMM_END };
	enum DIMB { DIMB_LBUTTON, DIMB_RBUTTON, DIMB_WHEEL, DIMB_END };


private:
	CInput_Device();
	virtual ~CInput_Device() = default;


public:
	_byte Get_DIKeyState(_ubyte byKeyID) {
		return m_KeyState[byKeyID];
	}

	_long Get_DIMMoveState(DIMM eMouseMoveID) {		
		return *((_long*)&m_MouseState + eMouseMoveID);
	}

	_byte Get_DIMButtonState(DIMB eMouseButtonID) {
		return m_MouseState.rgbButtons[eMouseButtonID];
	}




public:
	HRESULT Initialize_Input_Device(HINSTANCE hInst, HWND hWnd);
	void Update_InputDeviceState();

//	0000 0000 0000 0000 
//&	1000 0000 0000 0000
//    0000 0000 0000 0000
//	
//
//	GetKeyState(VK_RETURN) & 0x8001

private:
	LPDIRECTINPUT8				m_pInput = nullptr;
	LPDIRECTINPUTDEVICE8		m_pKeyBoard = nullptr;
	LPDIRECTINPUTDEVICE8		m_pMouse = nullptr;

private:
	_byte						m_KeyState[256];
	DIMOUSESTATE				m_MouseState;


public:
	virtual void Free() override;
};

END