#ifndef __INSTALLLER_HPP__
#define __INSTALLLER_HPP__
#include "Common.hpp"
#include <string>
#include <Windows.h>

namespace WinSvcLib {
	namespace Installer {
		enum class ExecuteUser : short {
			System			= 0,
			LocalService	= 1,
			NetworkService	= 2
		};
		enum class ServiceStartType : DWORD {
			BootStart		= 0x00000000, // システムローダーが開始するデバイスドライバの指定
			SystemStart		= 0x00000001, // IoInitSystem関数が開始するデバイスドライバを指定
			AutoStart		= 0x00000002, // 自動開始
			ManualStart		= 0x00000003, // 手動開始
			Disabled		= 0x00000004, // 無効
			DelayStart		= 0x00000005  // 遅延開始
		};
		enum class ServiceErrorControlType : DWORD {
			Ignore			= 0x00000000, // SERVICE_ERROR_IGNORE
			Normal			= 0x00000001, // SERVICE_ERROR_NORMAL
			Severe			= 0x00000002, // SERVICE_ERROR_SEVERE
			Critical		= 0x00000003  // SERVICE_ERROR_CRITICAL
		};
		void InstallerLibInit(const std::basic_string<TCHAR> ServiceName, const std::basic_string<TCHAR> ExecuteFilePath, bool IsDeviceDriver);
		void SetServiceDescription(LPTSTR Description);
		void ChangeServiceType(const ServiceType Type);
		void ChangeStartType(const ServiceStartType StartType);
		void ChangeDisplayName(const std::basic_string<TCHAR> DisplayName);
		void SetLoadOrderGroup(const std::basic_string<TCHAR> LoadOrderGroup);
		void SetDependencies(LPCTSTR Dependencies);
		void ChangeExecuteUser(const std::basic_string<TCHAR> UserName);
		void ChangeExecuteUser(const ExecuteUser User);
		void SetUserPassword(const std::basic_string<TCHAR> Passoword);
		void InstallerLibEnd(LPDWORD lpdwTagId);
	}
}
#endif
