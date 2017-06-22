#ifndef __UNINSTALLER_HPP__
#define __UNINSTALLER_HPP__
#ifdef SVCUNINSTALLER_EXPORTS
#define UNINSTALL_LIB __declspec(dllexport)
#else
#define UNINSTALL_LIB __declspec(dllimport)
#endif
#include <string>
#ifdef UNICODE
typedef wchar_t TCHAR;
#else
typedef char TCHAR;
#endif

namespace WinSvcLib {
	namespace Uninstaller {
		void UNINSTALL_LIB Uninstall(const std::basic_string<TCHAR> ServiceName);
	}
}
#endif
