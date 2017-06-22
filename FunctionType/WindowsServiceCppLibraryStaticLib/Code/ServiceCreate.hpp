#ifndef __SERVICECREATE_HPP__
#define __SERVICECREATE_HPP__
#include "Common.hpp"
#include <string>
#include <vector>
#include <Windows.h>
typedef UINT (WINAPI* SVCTHREAD)(PVOID);
typedef void (WINAPI* SVCMAIN)(DWORD, LPTSTR[]);

namespace WinSvcLib {
	enum class ServiceControl : DWORD {
		Stop					= 0x00000001, // SERVICE_CONTROL_STOP
		Pause					= 0x00000002, // SERVICE_CONTROL_PAUSE
		Continue				= 0x00000003, // SERVICE_CONTROL_CONTINUE
		InterRogate				= 0x00000004, // SERVICE_CONTROL_INTERROGATE
		Shutdown				= 0x00000005, // SERVICE_CONTROL_SHUTDOWN
		ParamChange				= 0x00000006, // SERVICE_CONTROL_PARAMCHANGE
		NetBindAdd				= 0x00000007, // SERVICE_CONTROL_NETBINDADD
		NetBindRemove			= 0x00000008, // SERVICE_CONTROL_NETBINDREMOVE
		NetBindEnable			= 0x00000009, // SERVICE_CONTROL_NETBINDENABLE
		NetBindDisable			= 0x0000000A, // SERVICE_CONTROL_NETBINDDISABLE
		DeviceEvent				= 0x0000000B, // SERVICE_CONTROL_DEVICEEVENT
		HardwareProfileChange	= 0x0000000C, // SERVICE_CONTROL_HARDWAREPROFILECHANGE
		PowerEvent				= 0x0000000D, // SERVICE_CONTROL_POWEREVENT
		SessionChange			= 0x0000000E, // SERVICE_CONTROL_SESSIONCHANGE
		PreShutdown				= 0x0000000F, // SERVICE_CONTROL_PRESHUTDOWN
		TimeChange				= 0x00000010, // SERVICE_CONTROL_TIMECHANGE
		TriggerEvent			= 0x00000020  // SERVICE_CONTROL_TRIGGEREVENT
	};
	enum class ServiceState : DWORD {
		Stopped					= 0x00000001, // SERVICE_STOPPED
		StartPending			= 0x00000002, // SERVICE_START_PENDING
		StopPending				= 0x00000003, // SERVICE_STOP_PENDING
		Running					= 0x00000004, // SERVICE_RUNNING
		ContinuePending			= 0x00000005, // SERVICE_CONTINUE_PENDING
		PausePending			= 0x00000006, // SERVICE_PAUSE_PENDING
		Paused					= 0x00000007  // SERVICE_PAUSED
	};
	enum class ServiceControlsAccepted : DWORD {
		Stop= 0x00000001, // SERVICE_ACCEPT_STOP
		PauseContinue			= 0x00000002, // SERVICE_ACCEPT_PAUSE_CONTINUE
		Shutdown				= 0x00000004, // SERVICE_ACCEPT_SHUTDOWN
		ParamChange				= 0x00000008, // SERVICE_ACCEPT_PARAMCHANGE
		NetBindChange			= 0x00000010, // SERVICE_ACCEPT_NETBINDCHANGE
		HardwareProfileChange	= 0x00000020, // SERVICE_ACCEPT_HARDWAREPROFILECHANGE
		PowerEvent				= 0x00000040, // SERVICE_ACCEPT_POWEREVENT
		SessionChange			= 0x00000080, // SERVICE_ACCEPT_SESSIONCHANGE
		PreShutdown				= 0x00000100, // SERVICE_ACCEPT_PRESHUTDOWN
		TimeChange				= 0x00000200, // SERVICE_ACCEPT_TIMECHANGE
		TriggerEvent			= 0x00000400  // SERVICE_ACCEPT_TRIGGEREVENT
	};
	ServiceControl operator | (const ServiceControl A, const ServiceControl B);
	ServiceControlsAccepted operator | (const ServiceControlsAccepted A, const ServiceControlsAccepted B);
	namespace ServiceCreate {
		void  EntryServiceMainToWindows(SVCMAIN ServiceMain, SVCTHREAD ServiceThread);
		void  SetDebugCommandLine(LPTSTR lpszArgv[]);
		namespace CallInServiceMain {
			void  WinSvcLibInit(const std::basic_string<TCHAR> ServiceName, SVCTHREAD ServiceThread);
			void  WinSvcLibInit(const std::basic_string<TCHAR> ServiceName, SVCTHREAD ServiceThread, DWORD dwArgc, LPTSTR lpszArgv[]);
			void  SetServiceType(const ServiceType SvcType);
			void  SetCurrentState(const ServiceState SvcState);
			void  SetControlsAccepted(const ServiceControlsAccepted SvcControlAccepted);
			void  SetWin32ExitCode(const DWORD Win32ExitCode);
			void  SetServiceSpecificExitCode(const DWORD ServiceSCallpecificExitCode);
			void  SetCheckPoint(const DWORD CheckPoint);
			void  SetWaitHint(const DWORD WaitHint);
			void  WinSvcLibEnd();
		}
		namespace CallInServiceThread {
			ServiceControl  GetCurrentControl();
			ServiceState  GetServiceState();
			DWORD  GetArgc();
			std::basic_string<TCHAR>  GetArgv(const size_t Number);
			std::vector<std::basic_string<TCHAR>>  GetArgv();
		}
	}
}
#endif
