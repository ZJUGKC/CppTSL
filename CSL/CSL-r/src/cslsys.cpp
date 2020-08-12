/*
** Xin YUAN, 2019, BSD (2)
*/

////////////////////////////////////////////////////////////////////////////////

#include "precomp.h"

#ifdef WIN32
#include <windows.h>
#endif

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
namespace CSL {
////////////////////////////////////////////////////////////////////////////////

// Execute a command

bool ForegroundProcessHelper::ExecuteCommand(const char* szCmd, int& iRet) noexcept
{
#ifdef WIN32
	iRet = -1;
	size_t len = ::strlen(szCmd);
	char* szBuf = (char*)::malloc(len + 1);
	if( szBuf == nullptr )
		return false;
	::memcpy(szBuf, szCmd, len);
	szBuf[len] = '\0';
	STARTUPINFOA si;
	::memset(&si, 0, sizeof(STARTUPINFOA));
	si.cb = sizeof(STARTUPINFOA);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	PROCESS_INFORMATION pi;
	::memset(&pi, 0, sizeof(PROCESS_INFORMATION));
	if( !::CreateProcessA(NULL, szBuf, NULL, NULL, FALSE, CREATE_NO_WINDOW,
						NULL, NULL, &si, &pi) ) {
		::free(szBuf);
		return false;
	}
	::WaitForSingleObject(pi.hProcess, INFINITE);
	DWORD dwCode;
	if( ::GetExitCodeProcess(pi.hProcess, &dwCode) )
		iRet = (int)dwCode;
	::CloseHandle(pi.hProcess);
	::CloseHandle(pi.hThread);
	::free(szBuf);
	return true;
#else
	iRet = ::system(szCmd);
	return iRet != -1;
#endif
}

////////////////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////////////////
