#include "stdafx.h"
#include "Installer.hpp"
#include <ShlObj.h>
#include <string>

static std::string GetErrorMessageA() {
	char* lpMessageBuffer = nullptr;
	const DWORD code = GetLastError();
	const DWORD length = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, code, LANG_USER_DEFAULT, (LPSTR)&lpMessageBuffer, 0, NULL);
	if (length == 0) throw std::runtime_error("エラーメッセージ取得時にエラーが発生しました。\nエラーコード : " + std::to_string(GetLastError()));
	DWORD i = length - 3;
	for (; '\r' != lpMessageBuffer[i] && '\n' != lpMessageBuffer[i] && '\0' != lpMessageBuffer[i]; i++);//改行文字削除
	lpMessageBuffer[i] = '\0';
	std::string s = "エラーコード : " + std::to_string(code) + "　" + lpMessageBuffer;
	LocalFree(lpMessageBuffer);
	return s;
}


namespace WinSvcLib {
	namespace Installer {
		struct ServiceInfo {
			std::basic_string<TCHAR> ServiceName; // サービスの名前
			std::basic_string<TCHAR> DisplayName; // サービスの表示名
			LPTSTR Description; // サービスの説明。SERVICE_DESCRIPTION構造体のlpDescriptionの型がLPCTSTRでないためstd::basic_string使用不可
			std::basic_string<TCHAR> ExecuteFilePath; // 実行ファイルのパス
			ServiceType Type; // サービスの種類
			ServiceStartType StartType; // サービスの開始の種類
			ServiceErrorControlType ErrorControl; // エラー発生時の深刻さ
			std::basic_string<TCHAR> LoadOrderGroup; // サービスの所属しているロード順序決定グループ名
			LPCTSTR Dependencies; // 複数の依存名からなる配列
			std::basic_string<TCHAR> ExecuteUser; // 実行ユーザー名
			std::basic_string<TCHAR> ExecuteUserPassword; // 実行ユーザーのパスワード
			bool DeviceDriver;
		} inf;
		static inline std::basic_string<TCHAR> ChangeUserEnumToString(const ExecuteUser User) {
			switch (User) {
				case ExecuteUser::LocalService: return TEXT("NT AUTHORITY\\LocalService");
				case ExecuteUser::NetworkService: return TEXT("NT AUTHORITY\\NetworkService");
				default: return TEXT("NT AUTHORITY\\SYSTEM");
			}
		}
		static inline bool IsAuthorityUser() {
			return inf.ExecuteUser == ChangeUserEnumToString(ExecuteUser::System)
				|| inf.ExecuteUser == ChangeUserEnumToString(ExecuteUser::LocalService)
				|| inf.ExecuteUser == ChangeUserEnumToString(ExecuteUser::NetworkService);
		}
		void InstallerLibInit(const std::basic_string<TCHAR> ServiceName, const std::basic_string<TCHAR> ExecuteFilePath, bool IsDeviceDriver) {
			inf.DeviceDriver = IsDeviceDriver;
			inf.ServiceName = inf.DisplayName = ServiceName;
			inf.Type = ServiceType::Win32OwnProcess;
			inf.ErrorControl = ServiceErrorControlType::Normal;
			inf.Description = NULL;
			if (ExecuteFilePath.empty()) {
				LPTSTR Buffer = new TCHAR[MAX_PATH];
				GetModuleFileName(NULL, Buffer, sizeof(Buffer) / sizeof(Buffer[0]));
				inf.ExecuteFilePath = Buffer;
				delete[] Buffer;
			}
			else inf.ExecuteFilePath = ExecuteFilePath;
			inf.StartType = ServiceStartType::AutoStart;
			inf.ExecuteUser = ChangeUserEnumToString(ExecuteUser::System);
			inf.ExecuteUserPassword = TEXT("");
		}
		void SetServiceDescription(LPTSTR Description) { inf.Description = Description; }
		void ChangeServiceType(const ServiceType Type) { inf.Type = Type; }
		void ChangeStartType(const ServiceStartType StartType) {
			if (!inf.DeviceDriver && (StartType == ServiceStartType::BootStart || StartType == ServiceStartType::SystemStart)) return;
			inf.StartType = StartType;
		}
		void ChangeDisplayName(const std::basic_string<TCHAR> DisplayName) { inf.DisplayName = DisplayName; }
		void SetLoadOrderGroup(const std::basic_string<TCHAR> LoadOrderGroup) { inf.LoadOrderGroup = LoadOrderGroup; }
		void SetDependencies(LPCTSTR Dependencies) { inf.Dependencies = Dependencies; }
		void ChangeExecuteUser(const std::basic_string<TCHAR> UserName) { inf.ExecuteUser = UserName; }
		void ChangeExecuteUser(const ExecuteUser User) { inf.ExecuteUser = ChangeUserEnumToString(User); }
		void SetUserPassword(const std::basic_string<TCHAR> Password) {
			if (IsAuthorityUser()) return;
			inf.ExecuteUserPassword = Password;
		}
		static void CloseServiceHandles(SC_HANDLE &Service, SC_HANDLE &scm) {
			CloseServiceHandle(Service);
			CloseServiceHandle(scm);
		}
		void InstallerLibEnd(LPDWORD lpdwTagId) {
			if (FALSE == IsUserAnAdmin()) throw std::runtime_error("管理者権限での起動が必要です。");
			SC_HANDLE Service = NULL, scm = NULL;
			SERVICE_DESCRIPTION ServiceDescription;
			if ((scm = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE)) == NULL) {
				CloseServiceHandles(Service, scm);
				throw std::runtime_error(
					"Failed In OpenSCManager Function\n"
					+ GetErrorMessageA()
				);
			}
			if ((Service = CreateService(
				scm,
				inf.ServiceName.c_str(),
				inf.DisplayName.c_str(),
				SERVICE_CHANGE_CONFIG,
				static_cast<DWORD>(inf.Type),
				static_cast<DWORD>(inf.StartType == ServiceStartType::DelayStart ? ServiceStartType::AutoStart : inf.StartType),
				static_cast<DWORD>(inf.ErrorControl),
				inf.ExecuteFilePath.c_str(),
				inf.LoadOrderGroup.empty() ? NULL : inf.LoadOrderGroup.c_str(),
				lpdwTagId,
				inf.Dependencies,
				inf.ExecuteUser.c_str(),
				inf.ExecuteUserPassword.c_str()
			)) == NULL) {
				CloseServiceHandles(Service, scm);
				throw std::runtime_error(
					"Failed In CreateService Function\n"
					+ GetErrorMessageA()
				);
			}
			ServiceDescription.lpDescription = inf.Description;
			if (inf.StartType == ServiceStartType::DelayStart) {
				SERVICE_DELAYED_AUTO_START_INFO info = { TRUE };
				if (ChangeServiceConfig2(Service, SERVICE_CONFIG_DELAYED_AUTO_START_INFO, &info)) {
					CloseServiceHandles(Service, scm);
					throw std::runtime_error(
						"Failed Set Service Delay Start\n"
						+ GetErrorMessageA()
					);
				}
			}
			if (0 == ChangeServiceConfig2(Service, SERVICE_CONFIG_DESCRIPTION, &ServiceDescription)) {
				CloseServiceHandles(Service, scm);
				throw std::runtime_error(
					"Failed Set Service Description\n"
					+ GetErrorMessageA()
				);
			}
			CloseServiceHandles(Service, scm);
		}
	}
}
