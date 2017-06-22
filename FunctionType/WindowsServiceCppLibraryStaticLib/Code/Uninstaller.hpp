#ifndef __UNINSTALLLER_HPP__
#define __UNINSTALLLER_HPP__
#include <string>
#include <tchar.h>

namespace WinSvcLib {
	namespace Uninstaller {
		void Uninstall(const std::basic_string<TCHAR> ServiceName);
	}
}
#endif
