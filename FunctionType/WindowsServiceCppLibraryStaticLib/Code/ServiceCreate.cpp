#include "stdafx.h"
#include "Common.hpp"
#include "ServiceCreate.hpp"
#include <strsafe.h>
#include <process.h>
#include <string>
#include <vector>
#include <stdexcept>
static SERVICE_STATUS SvcStatus;
static SERVICE_STATUS_HANDLE SvcStatusHandle;
struct WinServiceInfo {
	SVCTHREAD ServiceThread;
	std::basic_string<TCHAR> ServiceName;
	WinSvcLib::ServiceState State;
	WinSvcLib::ServiceControl CurrentControl;
} inf;

struct CommandLine {
	CommandLine() = default;
	CommandLine(DWORD dwArgc, LPTSTR lpszArgv[]) : argc(dwArgc) {
		if (lpszArgv == NULL) this->argv = std::vector<std::basic_string<TCHAR>>();
		else for (DWORD i = 0; i < dwArgc; i++) this->argv.emplace_back(lpszArgv[i]);
	}
	DWORD argc;
	std::vector<std::basic_string<TCHAR>> argv;
} cmd;

DWORD WINAPI HandlerEx(DWORD dwControl, DWORD, PVOID, PVOID) {
	inf.CurrentControl = static_cast<WinSvcLib::ServiceControl>(dwControl);
	switch (dwControl) {
	case SERVICE_CONTROL_STOP:
		SvcStatus.dwCurrentState = static_cast<DWORD>(WinSvcLib::ServiceState::Stopped);
		inf.State = WinSvcLib::ServiceState::Stopped;
		break;
	case SERVICE_CONTROL_PAUSE:
		SvcStatus.dwCurrentState = static_cast<DWORD>(WinSvcLib::ServiceState::Paused);
		inf.State = WinSvcLib::ServiceState::Paused;
		break;
	case SERVICE_CONTROL_CONTINUE:
		SvcStatus.dwCurrentState = static_cast<DWORD>(WinSvcLib::ServiceState::Running);
		inf.State = WinSvcLib::ServiceState::Running;
		break;
	case SERVICE_CONTROL_SHUTDOWN:
		SvcStatus.dwCurrentState = static_cast<DWORD>(WinSvcLib::ServiceState::Stopped);
		inf.State = WinSvcLib::ServiceState::Stopped;
		break;
	default:
		break;
	}
	SetServiceStatus(SvcStatusHandle, &SvcStatus);
	return NO_ERROR;
}

namespace WinSvcLib {
	ServiceControl operator | (const ServiceControl A, const ServiceControl B) {
		return static_cast<ServiceControl>(static_cast<DWORD>(A) | static_cast<DWORD>(B));
	}
	ServiceType operator | (const ServiceType A, const ServiceType B) {
		return static_cast<ServiceType>(static_cast<DWORD>(A) | static_cast<DWORD>(B));
	}
	ServiceControlsAccepted operator | (const ServiceControlsAccepted A, const ServiceControlsAccepted B) {
		return static_cast<ServiceControlsAccepted>(static_cast<DWORD>(A) | static_cast<DWORD>(B));
	}
	ServiceAccessType operator | (const ServiceAccessType A, const ServiceAccessType B) {
		return static_cast<ServiceAccessType>(static_cast<DWORD>(A) | static_cast<DWORD>(B));
	}
	namespace ServiceCreate {
		void EntryServiceMainToWindows(SVCMAIN ServiceMain) {
#ifndef _DEBUG
			TCHAR buf[256];
#ifdef UNICODE
			wcscpy_s(buf, inf.ServiceName.c_str());
#else
			strcpy_s(buf, inf.ServiceName.c_str());
#endif
			SERVICE_TABLE_ENTRY ServiceTableEntry[] = {
				{ buf, ServiceMain },
				{ NULL, NULL }
			};
			StartServiceCtrlDispatcher(ServiceTableEntry);
#else
			SvcStatus.dwCurrentState = static_cast<DWORD>(ServiceState::Running);
			ServiceMain(0, NULL);
#endif
		}
		void SetDebugCommandLine(LPTSTR lpszArgv[]) {
#ifdef _DEBUG
			cmd = CommandLine(sizeof(lpszArgv) / sizeof(lpszArgv[0]), lpszArgv);
#else
			UNREFERENCED_PARAMETER(lpszArgv);
#endif
		}
		namespace CallInServiceMain {
			void WinSvcLibInit(const std::basic_string<TCHAR> ServiceName, SVCTHREAD ServiceThread, DWORD dwArgc, LPTSTR lpszArgv[]) {
#ifdef _DEBUG
				UNREFERENCED_PARAMETER(dwArgc);
				UNREFERENCED_PARAMETER(lpszArgv);
				ServiceThread(NULL);

#else
				cmd = CommandLine(dwArgc, lpszArgv);
				inf.ServiceThread = ServiceThread;
				SvcStatusHandle = RegisterServiceCtrlHandlerEx(ServiceName.c_str(), HandlerEx, NULL);
				memset(&SvcStatus, 0, sizeof(SvcStatus));
				inf.ServiceName = ServiceName;
				SvcStatus.dwServiceType = static_cast<DWORD>(ServiceType::Win32OwnProcess);
				SvcStatus.dwCurrentState = static_cast<DWORD>(ServiceState::Running);
				SvcStatus.dwControlsAccepted = static_cast<DWORD>(ServiceControlsAccepted::Stop | ServiceControlsAccepted::PauseContinue);
				SvcStatus.dwWin32ExitCode = NO_ERROR;
				SvcStatus.dwServiceSpecificExitCode = 0;
				SvcStatus.dwCheckPoint = 0;
				SvcStatus.dwWaitHint = 2000;
#endif
			}
			void WinSvcLibInit(const std::basic_string<TCHAR> ServiceName, SVCTHREAD ServiceThread) {
				WinSvcLibInit(ServiceName, ServiceThread, 1, NULL);
			}
			void SetServiceType(const ServiceType SvcType) {
				SvcStatus.dwServiceType = static_cast<DWORD>(SvcType);
			}
			void SetCurrentState(const ServiceState SvcState) {
				SvcStatus.dwCurrentState = static_cast<DWORD>(SvcState);
			}
			void SetControlsAccepted(const ServiceControlsAccepted SvcControlAccepted) {
				SvcStatus.dwControlsAccepted = static_cast<DWORD>(SvcControlAccepted);
			}
			void SetWin32ExitCode(const DWORD Win32ExitCode) {
				SvcStatus.dwWin32ExitCode = Win32ExitCode;
			}
			void SetServiceSpecificExitCode(const DWORD ServiceSpecificExitCode) {
				SvcStatus.dwServiceSpecificExitCode = ServiceSpecificExitCode;
			}
			void SetCheckPoint(const DWORD CheckPoint) {
				SvcStatus.dwCheckPoint = CheckPoint;
			}
			void SetWaitHint(const DWORD WaitHint) {
				SvcStatus.dwWaitHint = WaitHint;
			}
			void WinSvcLibEnd() {
#ifndef _DEBUG
				UINT uiThreadID;
				_beginthreadex(
					NULL,
					0,
					inf.ServiceThread,
					NULL,
					0,
					&uiThreadID
				);
				SetServiceStatus(SvcStatusHandle, &SvcStatus);
#endif
			}
		}
		namespace CallInServiceThread {
			ServiceControl GetCurrentControl() {
				return inf.CurrentControl;
			}
			ServiceState GetServiceState() {
				return inf.State;
			}
			DWORD GetArgc() {
				return cmd.argc;
			}
			std::basic_string<TCHAR> GetArgv(const size_t Number) {
				return cmd.argv[Number];
			}
			std::vector<std::basic_string<TCHAR>> GetArgv() {
				return cmd.argv;
			}
		}
	}
}

