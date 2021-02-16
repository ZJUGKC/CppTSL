/*
** Xin YUAN, 2019, BSD (2)
*/

////////////////////////////////////////////////////////////////////////////////

#include "precomp.h"

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
namespace CSL {
////////////////////////////////////////////////////////////////////////////////

//console

#ifdef WIN32
ConsoleEncoding::ConsoleEncoding(UINT uCP) noexcept
{
	m_uInput = ::GetConsoleCP();
	assert( m_uInput != 0 );
	m_uOutput = ::GetConsoleOutputCP();
	assert( m_uOutput != 0 );

	BOOL bRet;
	bRet = ::SetConsoleOutputCP(uCP);
	assert( bRet );
	bRet = ::SetConsoleCP(uCP);
	assert( bRet );
}
ConsoleEncoding::~ConsoleEncoding() noexcept
{
	BOOL bRet;
	bRet = ::SetConsoleOutputCP(m_uOutput);
	assert( bRet );
	bRet = ::SetConsoleCP(m_uInput);
	assert( bRet );
}
#else
ConsoleEncoding::ConsoleEncoding(uint32_t uCP) noexcept
{
}
ConsoleEncoding::~ConsoleEncoding() noexcept
{
}
#endif

//code convert

#ifdef WIN32

// Ansi -> wstring
bool AnsiToWide(UINT uCP, const char* sz, int len, std::wstring& str)
{
	assert( len >= 0 );
	if ( len == 0 ) {
		str.clear();
		return true;
	}
	int ret = ::MultiByteToWideChar(uCP, 0, sz, len, NULL, 0);
	if ( ret <= 0 )
		return false;
	str.resize(ret);
	ret = ::MultiByteToWideChar(uCP, 0, sz, len, &str.front(), ret);
	assert( ret > 0 );
	return true;
}
// wstring -> Ansi
bool WideToAnsi(UINT uCP, const wchar_t* wsz, int len, std::string& str)
{
	assert( len >= 0 );
	if ( len == 0 ) {
		str.clear();
		return true;
	}
	int ret = ::WideCharToMultiByte(uCP, 0, wsz, len, NULL, 0, NULL, NULL);
	if ( ret <= 0 )
		return false;
	str.resize(ret);
	ret = ::WideCharToMultiByte(uCP, 0, wsz, len, &str.front(), ret, NULL, NULL);
	assert( ret > 0 );
	return true;
}

#else

class conv_base
{
public:
	conv_base() noexcept : m_c((iconv_t)-1)
	{
	}
	conv_base(const conv_base&) = delete;
	conv_base& operator=(const conv_base&) = delete;
	~conv_base() noexcept
	{
		Close();
	}

	void Close() noexcept
	{
		if( m_c != (iconv_t)-1 ) {
			int ret = ::iconv_close(m_c);
			(void)ret;
			assert( ret == 0 );
			m_c = (iconv_t)-1;
		}
	}

	bool Open(const char* szFrom, const char* szTo) noexcept
	{
		assert( m_c == (iconv_t)-1 );
		m_c = ::iconv_open(szTo, szFrom);
		return m_c != (iconv_t)-1;
	}

	size_t Convert(char** ppSrc, size_t* pSrcBytes, char** ppDest, size_t* pDestBytes) noexcept
	{
		assert( m_c != (iconv_t)-1 );
		return ::iconv(m_c, ppSrc, pSrcBytes, ppDest, pDestBytes);
	}

private:
	iconv_t  m_c;
};

template <class TString>
inline void _realloc_string(TString& str, size_t& uTotal, size_t& uRest, char*& pNext)
{
	size_t uOffset = uTotal - uRest;
	uTotal = SafeOperators::MultiplyThrow(uTotal, (size_t)2);
	str.resize(uTotal / sizeof(typename TString::value_type));
	uRest = uTotal - uOffset;
	pNext = (char*)&str.front() + uOffset;
}

template <typename TChar, class TString>
inline bool cvt_string(const char* szFrom, const char* szTo, const TChar* sz, int len, TString& str)
{
	assert( len >= 0 );

	size_t uSrcBytes = SafeOperators::MultiplyThrow((size_t)len, sizeof(TChar));
	size_t uDestBytes = SafeOperators::MultiplyThrow((size_t)len, sizeof(typename TString::value_type));

	conv_base cvt;
	if ( !cvt.Open(szFrom, szTo) )
		return false;

	str.resize(uDestBytes / sizeof(typename TString::value_type));
	//reset state (no check)
	cvt.Convert(NULL, NULL, NULL, NULL);
	char* ps = (char*)sz;
	char* pd = (char*)&str.front();
	size_t uRest = uDestBytes;
	while ( true ) {
		size_t res = cvt.Convert(&ps, &uSrcBytes, &pd, &uRest);
		if ( res != (size_t)-1 ) {
			do {
				res = cvt.Convert(NULL, NULL, &pd, &uRest);
				if ( res != (size_t)-1 )
					break;
				if ( errno != E2BIG )
					return false;
				_realloc_string(str, uDestBytes, uRest, pd);
			} while ( true );
			str.resize((uDestBytes - uRest) / sizeof(typename TString::value_type));
			break;
		}
		if ( errno != E2BIG )
			return false;
		_realloc_string(str, uDestBytes, uRest, pd);
	}

	//BOM
	if ( sizeof(typename TString::value_type) > 1 ) {
		if ( str.size() != 0 && str[0] == 0xFEFF )
			str.erase(str.begin());
	}
	return true;
}

#endif

// UTF8 -> wstring
bool CodeConvertHelper::UTF8ToWide(const char* sz, size_t len, std::wstring& str)
{
	if ( len > (size_t)((std::numeric_limits<int>::max)()) )
		return false;
#ifdef WIN32
	return AnsiToWide(CP_UTF8, sz, (int)len, str);
#else
	return cvt_string("UTF-8", "UTF-32", sz, (int)len, str);
#endif
}
bool CodeConvertHelper::UTF8ToWide(const char* sz, std::wstring& str)
{
	return UTF8ToWide(sz, ::strlen(sz), str);
}
bool CodeConvertHelper::UTF8ToWide(const std::string& str1, std::wstring& str2)
{
	return UTF8ToWide(str1.c_str(), str1.size(), str2);
}

// wstring -> UTF8
bool CodeConvertHelper::WideToUTF8(const wchar_t* wsz, size_t len, std::string& str)
{
	if ( len > (size_t)((std::numeric_limits<int>::max)()) )
		return false;
#ifdef WIN32
	return WideToAnsi(CP_UTF8, wsz, (int)len, str);
#else
	return cvt_string("UTF-32", "UTF-8", wsz, (int)len, str);
#endif
}
bool CodeConvertHelper::WideToUTF8(const wchar_t* wsz, std::string& str)
{
	return WideToUTF8(wsz, ::wcslen(wsz), str);
}
bool CodeConvertHelper::WideToUTF8(const std::wstring& str1, std::string& str2)
{
	return WideToUTF8(str1.c_str(), str1.size(), str2);
}

//environment variables

bool EnvironmentVariableHelper::Get(const char* szName, std::string& str)
{
#ifdef WIN32
	size_t ret, count;
	errno_t err = ::getenv_s(&ret, NULL, 0, szName);
	if ( ret == 0 )
		return false;
	count = ret;  //including '\0'
	str.resize(count - 1);
	err = ::getenv_s(&ret, &str.front(), count, szName);
	if ( err != 0 )
		return false;
	return true;
#else
	char* p = ::getenv(szName);
	if( p == NULL )
		return false;
	str = p;
	return true;
#endif
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

bool FsPathHelper::GetHomeDirectory(std::string& str)
{
#ifdef WIN32
	return EnvironmentVariableHelper::Get("HOMEPATH", str);
#else
	return EnvironmentVariableHelper::Get("HOME", str);
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

bool FsPathHelper::FindExtensionStart(const char* szPath, size_t& uPos) noexcept
{
	size_t n = ::strlen(szPath);
	if ( n == 0 )
		return false;
	const char* sz = szPath;
	sz += n;
	do {
		sz --;
		char ch = *sz;
		if ( IsPathSeparator(ch) || IsDriveSeparator(ch) )
			return false;
		if ( ch == '.' ) {
			uPos = sz - szPath;
			return true;
		}
	} while ( sz != szPath );
	return false;
}

//file system

bool FsOpHelper::CheckFileExists(const char* sz) noexcept
{
#ifdef WIN32
	return ::_access(sz, 0) == 0;
#else
	return ::access(sz, F_OK) == 0;
#endif
}

bool FsOpHelper::CheckDirectoryExists(const char* sz) noexcept
{
#ifdef WIN32
	struct _stat st;
	return ::_stat(sz, &st) == 0 && (st.st_mode & _S_IFDIR) != 0;
#else
	struct stat st;
	return ::stat(sz, &st) == 0 && S_ISDIR(st.st_mode) != 0;
#endif
}

bool FsOpHelper::DeleteFile(const char* sz) noexcept
{
#ifdef WIN32
	return ::_unlink(sz) == 0;
#else
	return ::unlink(sz) == 0;
#endif
}

bool FsOpHelper::DeleteDirectory(const char* sz) noexcept
{
#ifdef WIN32
	return ::_rmdir(sz) == 0;
#else
	return ::rmdir(sz) == 0;
#endif
}

bool FsOpHelper::CreateDirectory(const char* sz) noexcept
{
#ifdef WIN32
	return ::_mkdir(sz) == 0;
#else
	return ::mkdir(sz, S_IRWXU | S_IRWXG | S_IRWXO) == 0;
#endif
}

bool FsOpHelper::Rename(const char* szOld, const char* szNew) noexcept
{
	return ::rename(szOld, szNew) == 0;
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
