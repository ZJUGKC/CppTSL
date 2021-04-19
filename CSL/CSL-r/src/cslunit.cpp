/*
** Xin YUAN, 2021, BSD (2)
*/

////////////////////////////////////////////////////////////////////////////////

#include "precomp.h"

////////////////////////////////////////////////////////////////////////////////
namespace CSL {
////////////////////////////////////////////////////////////////////////////////

// TestSuite

TestSuite::TestSuite() noexcept
{
}
TestSuite::~TestSuite() noexcept
{
}

void TestSuite::AddTest(const char* szName, std::function<bool()>&& pn)
{
	auto ret(m_map.insert(std::pair<std::string, std::function<bool()>>(std::string(szName), std::move(pn))));
	if ( !ret.second )
		throw std::runtime_error("insert error");
	m_vec.push_back(ret.first);
}

TestSuite::const_iterator TestSuite::GetBegin() const noexcept
{
	return m_vec.begin();
}
TestSuite::iterator TestSuite::GetBegin() noexcept
{
	return m_vec.begin();
}
TestSuite::const_iterator TestSuite::GetEnd() const noexcept
{
	return m_vec.end();
}
TestSuite::iterator TestSuite::GetEnd() noexcept
{
	return m_vec.end();
}

const std::string& TestSuite::GetName(const const_iterator& iter) const noexcept
{
	return (*iter)->first;
}
std::function<bool()> TestSuite::GetFunction(const const_iterator& iter) const noexcept
{
	return (*iter)->second;
}

std::function<bool()> TestSuite::Find(const char* sz) const
{
	std::string str(sz);
	auto iter(m_map.find(str));
	if ( iter == m_map.end() )
		return std::function<bool()>();
	return iter->second;
}

// TestMainHelper

int TestMainHelper::Process(const TestSuite& ts, int argc, char* argv[])
{
	const char* c_sep_b = "=====================================";
	const char* c_sep_e = "*************************************";

	size_t uTotal = 0;
	size_t uFailed = 0;

	if ( argc <= 1 ) {
		auto iter(ts.GetBegin());
		for ( ; iter != ts.GetEnd(); ++ iter ) {
			std::cout << c_sep_b << std::endl;
			std::cout << "Test Name : " << ts.GetName(iter) << std::endl;
			uTotal ++;
			std::function<bool()> fn(ts.GetFunction(iter));
			if ( fn == nullptr || !fn() ) {
				uFailed ++;
				std::cout << "Failed." << std::endl;
			}
			std::cout << c_sep_e << std::endl;
		}
	}
	else {
		for ( int i = 1; i < argc; i ++ ) {
			std::cout << c_sep_b << std::endl;
			std::cout << "Test Name : " << argv[i] << std::endl;
			uTotal ++;
			std::function<bool()> fn(ts.Find(argv[i]));
			if ( fn == nullptr || !fn() ) {
				uFailed ++;
				std::cout << "Failed." << std::endl;
			}
			std::cout << c_sep_e << std::endl;
		}
	}

	std::cout << "Total : " << uTotal << " Failed : " << uFailed << std::endl;

	return uFailed > 0 ? 1 : 0;
}

////////////////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////////////////
