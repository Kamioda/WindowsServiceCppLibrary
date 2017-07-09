#ifndef __COMMON_HPP__
#define __COMMON_HPP__
#include "BuildCoreMacro.hpp"
typedef unsigned long DWORD;

namespace WinSvcLib {
	enum class ServiceAccessType : DWORD {
		QueryConfig			= 0x0001, // SERVICE_QUERY_CONFIG
		ChangeConfig		= 0x0002, // SERVICE_CHANGE_CONFIG
		QueryStatus			= 0x0004, // SERVICE_QUERY_STATUS
		EnumerateDependents	= 0x0008, // SERVICE_ENUMERATE_DEPENDENTS
		Start				= 0x0010, // SERVICE_START
		Stop				= 0x0020, // SERVICE_STOP
		PauseContinue		= 0x0040, // SERVICE_PAUSE_CONTINUE
		Interrogate			= 0x0080, // SERVICE_INTERROGATE
		UserDefinedControl	= 0x0100, // SERVICE_USER_DEFINED_CONTROL
		AllAccess			= 0xF01FF // SERVICE_ALL_ACCESS
	};
	enum class ServiceType : DWORD {
		KernelDriver		= 0x00000001, // SERVICE_KERNEL_DRIVER
		FileSystemDriver	= 0x00000002, // SERVICE_FILE_SYSTEM_DRIVER
		Adapter				= 0x00000004, // SERVICE_ADAPTER
		RecognizerDriver	= 0x00000008, // SERVICE_RECOGNIZER_DRIVER
		Driver				= 0x0000000B, // SERVICE_DRIVER
		Win32OwnProcess		= 0x00000010, // SERVICE_WIN32_OWN_PROCESS
		Win32ShareProcess	= 0x00000020, // SERVICE_WIN32_SHARE_PROCESS
		Win32				= 0x00000030, // SERVICE_WIN32
		InteractiveProcess	= 0x00000100, // SERVICE_INTERACTIVE_PROCESS
		TypeAll				= 0x0000013F  // SERVICE_TYPE_ALL
	};
	WINSVCBUILD ServiceAccessType operator | (const ServiceAccessType A, const ServiceAccessType B);
	WINSVCBUILD ServiceType operator | (const ServiceType A, const ServiceType B);
}
#endif
