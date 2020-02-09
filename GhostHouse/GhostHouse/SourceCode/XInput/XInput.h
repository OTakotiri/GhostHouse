#ifndef CXINPUT_H_
#define CXINPUT_H_

#include <Windows.h>
#include <Xinput.h>

#include "..\SingletonBase\SingletonBase.h"

#pragma comment( lib, "xinput.lib" )

#define INPUT_TRIGGER_MIN	(0)
#define INPUT_TRIGGER_MAX	(255)
#define INPUT_THUMB_MIN		(-10000)
#define INPUT_THUMB_MAX		(10000)
#define INPUT_VIBRATION_MIN	(0)
#define INPUT_VIBRATION_MAX	(65535)

class CXInput : public std::CSingletonBase<CXInput>
{
public:

	CXInput()
		: m_padId()
		, m_state()
		, m_oldstate()
		, m_keystroke()
		, m_vibration()
		, m_bIsPressing( false )
		, m_bActiveController( true )
	{
	}

	~CXInput()
	{
	}

	static bool UpdateStatus()
	{
		if( GetInstance()->m_bActiveController == true ){
			GetInstance()->m_oldstate = GetInstance()->m_state;
			if( ERROR_SUCCESS == XInputGetState(
				GetInstance()->m_padId,
				&GetInstance()->m_state ) ){
				return true;
			}
			return false;
		}
		return false;
	}

	bool UpdateKeyStatus()
	{
		if( m_bActiveController == true ){
			if( ERROR_SUCCESS == XInputGetKeystroke(
				m_padId,
				0,
				&m_keystroke ) ){
				return true;
			}
			return false;
		}
		return false;
	}

	static bool IsPress( const WORD& _padKey )
	{
		if( ( GetInstance()->m_state.Gamepad.wButtons & _padKey ) &&
			!( GetInstance()->m_oldstate.Gamepad.wButtons & _padKey ) ){

			// 押した瞬間.
			if( GetInstance()->m_bIsPressing == false ){
				GetInstance()->m_bIsPressing = true;
				return true;
			}
		}
		GetInstance()->m_bIsPressing = false;
		return false;
	}

	bool SetVibration( const WORD& LMotorSpd, const WORD& RMotorSpd )
	{
		m_vibration.wLeftMotorSpeed = LMotorSpd;
		m_vibration.wRightMotorSpeed = RMotorSpd;

		if( ERROR_SUCCESS == XInputSetState(
			m_padId, &m_vibration ) ){
			return true;
		}
		return false;
	}

	void EndProc()
	{
		SetVibration( 0, 0 );
	}

	static void InitController()
	{
		GetInstance()->m_state.Gamepad.sThumbLX = 0;
		GetInstance()->m_state.Gamepad.sThumbLY = 0;
		GetInstance()->m_state.Gamepad.sThumbRX = 0;
		GetInstance()->m_state.Gamepad.sThumbRY = 0;
	}

	static BYTE GetLTrigger()
	{
		return GetInstance()->m_state.Gamepad.bLeftTrigger;
	}
	static BYTE GetRTrigger()
	{
		return GetInstance()->m_state.Gamepad.bRightTrigger;
	}
	static float GetLThumbX()
	{
		return GetInstance()->m_state.Gamepad.sThumbLX;
	}
	static float GetLThumbY()
	{
		return GetInstance()->m_state.Gamepad.sThumbLY;
	}
	static float GetRThumbX()
	{
		return GetInstance()->m_state.Gamepad.sThumbRX;
	}
	static float GetRThumbY()
	{
		return GetInstance()->m_state.Gamepad.sThumbRY;
	}

	static void SetActiveFlag( bool ControlleriscActive )
	{
		GetInstance()->m_bActiveController = ControlleriscActive;
	}

private:
	DWORD				m_padId;
	XINPUT_STATE		m_state;
	XINPUT_STATE		m_oldstate;
	XINPUT_KEYSTROKE	m_keystroke;
	XINPUT_VIBRATION	m_vibration;
	bool				m_bIsPressing;
	bool				m_bActiveController;

	// コピー・ムーブコンストラクタ, 代入演算子の削除.
	CXInput( const CXInput & ) = delete;
	CXInput& operator = ( const CXInput & ) = delete;
	CXInput( CXInput && ) = delete;
	CXInput& operator = ( CXInput && ) = delete;

};



#endif//#ifndef CXINPUT_H