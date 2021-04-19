/*
** Xin YUAN, 2021, BSD (2)
*/

////////////////////////////////////////////////////////////////////////////////
#ifndef __CSL_UNIT_H__
#define __CSL_UNIT_H__
////////////////////////////////////////////////////////////////////////////////

#ifndef __CSL_BASE_H__
	#error cslunit.h requires cslbase.h to be included first.
#endif

////////////////////////////////////////////////////////////////////////////////
namespace CSL {
////////////////////////////////////////////////////////////////////////////////

//test

class TestBodyHelper
{
public:
	//T : Callable object
	template <typename T>
	static bool Run(const T& t) throw()
	{
		try {
			t();
		}
		catch(std::logic_error& e) {
			std::cout << e.what() << std::endl;
			return false;
		}
		catch(...) {
			//**********
			std::cout << "Exception error." << std::endl;
			//**********
			return false;
		}
		return true;
	}

	//use __FILE__, __LINE__

	//T : Callable object
	template <typename T>
	static void Block(const T& fn, const char* szFile, int iLineNum)
	{
		bool bException = false;
		try {
			fn();
		}
		catch(std::logic_error& e) {
			std::cout << e.what() << std::endl;
			bException = true;
		}
		catch(...) {
			std::cout << "Exception: (" << szFile << ")(" << iLineNum << ")" << std::endl;
			bException = true;
		}
		Assert(!bException, szFile, iLineNum);
	}
	static void Assert(bool bValue, const char* szFile, int iLineNum)
	{
		if( !bValue ) {
			std::string str("Assert error: (");
			str += szFile;
			str += ")(";
			str += std::to_string(iLineNum);
			str += ")";
			throw std::logic_error(str);
		}
	}
	//T : Callable object
	template <typename T>
	static void Exception(const T& t, const char* szFile, int iLineNum)
	{
		try {
			t();
		}
		catch(...) {
			//**********
			std::cout << "Correct Exception : (" << szFile << ")(" << iLineNum << ")" << std::endl;
			//**********
			return ;
		}
		Assert(false, szFile, iLineNum);
	}
};

class TestSuite
{
private:
	typedef std::unordered_map<std::string, std::function<bool()>> mapClass;

public:
	typedef std::vector<mapClass::iterator>::iterator iterator;
	typedef std::vector<mapClass::iterator>::const_iterator const_iterator;

public:
	TestSuite() noexcept;
	TestSuite(const TestSuite&) = delete;
	TestSuite& operator=(const TestSuite&) = delete;
	~TestSuite() noexcept;

	void AddTest(const char* szName, std::function<bool()>&& pn);

	const_iterator GetBegin() const noexcept;
	iterator GetBegin() noexcept;
	const_iterator GetEnd() const noexcept;
	iterator GetEnd() noexcept;

	const std::string& GetName(const const_iterator& iter) const noexcept;
	std::function<bool()> GetFunction(const const_iterator& iter) const noexcept;

	std::function<bool()> Find(const char* sz) const;

private:
	mapClass m_map;
	std::vector<mapClass::iterator> m_vec;
};

class TestMainHelper
{
public:
	static int Process(const TestSuite& ts, int argc, char* argv[]);
};

////////////////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////////////////////////////////
