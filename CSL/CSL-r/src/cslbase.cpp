/*
** Xin YUAN, 2019, BSD (2)
*/

////////////////////////////////////////////////////////////////////////////////

#include "precomp.h"

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
namespace CSL {
////////////////////////////////////////////////////////////////////////////////

//code convert

// Ansi -> wstring

void CodeConvertHelper::AnsiToWide(const char* sz1, std::wstring& str2)
{
	str2.clear();
	std::mbstate_t state = {};
	size_t len = std::mbsrtowcs(nullptr, &sz1, 0, &state);
	if( len != (size_t)-1 ) {
		str2.resize(len + 1);
		len = std::mbsrtowcs(&(str2.at(0)), &sz1, len, &state);
		assert( len != (size_t)-1 );
		str2.resize(len);
	}
}

// wstring -> Ansi

void CodeConvertHelper::WideToAnsi(const wchar_t* sz1, std::string& str2)
{
	str2.clear();
	std::mbstate_t state = {};
	size_t len = std::wcsrtombs(nullptr, &sz1, 0, &state);
	if( len != (size_t)-1 ) {
		str2.resize(len + 1);
		len = std::wcsrtombs(&(str2.at(0)), &sz1, len, &state);
		assert( len != (size_t)-1 );
		str2.resize(len);
	}
}

// UTF8 -> wstring

void CodeConvertHelper::UTF8ToWide(const char* sz1, std::wstring& str2)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	str2 = conv.from_bytes(sz1);
}
void CodeConvertHelper::UTF8ToWide(const std::string& str1, std::wstring& str2)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	str2 = conv.from_bytes(str1);
}

// wstring -> UTF8

void CodeConvertHelper::WideToUTF8(const wchar_t* wsz1, std::string& str2)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	str2 = conv.to_bytes(wsz1);
}
void CodeConvertHelper::WideToUTF8(const std::wstring& str1, std::string& str2)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	str2 = conv.to_bytes(str1);
}

//path

void FsPathHelper::ToPlatform(char* szBuffer) throw()
{
#ifdef WIN32
	char* ps = szBuffer;
	while( *ps != 0 ) {
		if( *ps == '/' )
			*ps = '\\';
		++ ps;
	}
#endif
}

const char* FsPathHelper::GetHomeDirectory() throw()
{
#ifdef WIN32
	return ::getenv("HOMEPATH");
#else
	return ::getenv("HOME");
#endif
}

bool FsPathHelper::GetExePathName(std::string& str)
{
	str.resize(MAX_FULL_PATH + 1);
#ifdef WIN32
	DWORD dwRet = ::GetModuleFileNameA(NULL, &str[0], MAX_FULL_PATH + 1);
	if ( dwRet == 0 ) {
		str.clear();
		return false;
	}
	if ( dwRet == MAX_FULL_PATH + 1 ) {
		str.resize(MAX_FULL_PATH);
		return false;
	}
	str.resize(dwRet);
	return true;
#else
	ssize_t ret = ::readlink("/proc/self/exe", &str[0], MAX_FULL_PATH);
	if ( ret == -1 ) {
		str.clear();
		return false;
	}
	str.resize(ret);
	return true;
#endif
}

bool FsPathHelper::IsDriveSeparator(char ch) noexcept
{
#ifdef WIN32
	return ch == ':';
#else
	return false;
#endif
}
bool FsPathHelper::IsPathSeparator(char ch) noexcept
{
#ifdef WIN32
	return ch == '/' || ch == '\\';
#else
	return ch == '/';
#endif
}

void FsPathHelper::AppendSeparator(std::string& str)
{
	size_t n = str.size();
	if ( n == 0 || !IsPathSeparator(str[n - 1]) )
		str += '/';
}
void FsPathHelper::RemoveSeparator(std::string& str)
{
	size_t n = str.size();
	if ( n != 0 && IsPathSeparator(str[n - 1]) )
		str.erase(n - 1, 1);
}

size_t FsPathHelper::FindFilePart(const char* szPath) noexcept
{
	size_t n = ::strlen(szPath);
	if ( n == 0 )
		return 0;
	const char* sz = szPath;
	sz += n;
	do {
		sz --;
		char ch = *sz;
		if ( IsPathSeparator(ch) || IsDriveSeparator(ch) )
			return (sz + 1) - szPath;
	} while ( sz != szPath );
	return 0;
}

//stream

bool StreamHelper::CheckBOM_UTF8(std::istream& stm)
{
	bool bRet = false;
	uint8_t buf[3];
	stm.read((char*)buf, 3);
	std::streamsize n = stm.gcount();
	int32_t iBack = 0;
	if( n == 3 ) {
		if( buf[0] != 0xEF || buf[1] != 0xBB || buf[2] != 0xBF )
			iBack = 3;
		else
			bRet = true;
	}
	else {
		iBack = (int32_t)n;
	}
	if( iBack != 0 )
		stm.seekg(-iBack, std::ios::cur);
	return bRet;
}

//framework

// Property Trigger

PropertyTrigger::PropertyTrigger() noexcept
{
}
PropertyTrigger::~PropertyTrigger() noexcept
{
}

void PropertyTrigger::ClearNotifications() noexcept
{
	m_vecPN.clear();
}

uintptr_t PropertyTrigger::AddNotification(PropertyNotification&& pn)
{
	uintptr_t index = 0;
	for ( auto& fn : m_vecPN ) {
		if ( fn == nullptr ) {
			fn = std::move(pn);
			return index + 1;
		}
		index ++;
	}
	m_vecPN.push_back(std::move(pn));
	return index + 1;
}

void PropertyTrigger::RemoveNotification(uintptr_t uCookie) noexcept
{
	assert( uCookie > 0 && uCookie <= m_vecPN.size() );
	m_vecPN[uCookie - 1] = nullptr;
}

void PropertyTrigger::Fire(uint32_t uID)
{
	for ( auto& fn : m_vecPN ) {
		if ( fn != nullptr )
			fn(uID);
	}
}

////////////////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////////////////
