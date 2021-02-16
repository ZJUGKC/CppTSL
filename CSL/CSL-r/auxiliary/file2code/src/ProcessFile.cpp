/*
** Xin YUAN, 2021, BSD (2)
*/

////////////////////////////////////////////////////////////////////////////////

#include "precomp.h"

#include "ProcessFile.h"

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
namespace CSL {
////////////////////////////////////////////////////////////////////////////////

static
void write_h_file(std::ofstream& output, const char* szVar)
{
	output << "#ifndef __" << szVar << "_H__" << std::endl;
	output << "#define __" << szVar << "_H__" << std::endl;
	output << "extern const unsigned char c_" << szVar << "[];" << std::endl;
	output << "#endif" << std::endl;
}

static
bool write_source_file(std::ifstream& input, std::ofstream& output, const char* szVar, uintptr_t uFileSize)
{
	output << "#include \"" << szVar << ".h\"" << std::endl;
	output << "const unsigned char c_" << szVar << "[]={" << std::endl;
	//length
	{
		unsigned char* p = (unsigned char*)(&uFileSize);
		for ( uintptr_t i = 0; i < sizeof(uintptr_t); i ++ )
			output << (int)(p[i]) << ",";
		output << std::endl;
	}
	//body
	uintptr_t cur = 0;
	while ( true ) {
		char ch;
		input.get(ch);
		if ( input.eof() )
			break;
		if ( !input.good() )
			return false;
		output << (int)(unsigned char)ch << ",";
		cur ++;
		if ( (cur % 100) == 0 )
			output << std::endl;
	}
	if ( cur != uFileSize )
		return false;
	//tail
	cur = (4 - (cur % 4)) % 4;
	for ( uintptr_t i = 0; i < cur + 3; i ++ )
		output << "0,";
	output << "0};" << std::endl;
	return true;
}

bool ProcessFile(const char* szSrc, const char* szDest, const char* szVar, bool bCpp)
{
	std::ofstream output;
	std::string strDest;

	std::ifstream input;
	input.open(szSrc, std::ios_base::in | std::ios_base::binary);
	if ( !input.is_open() ) {
		std::cout << "Error: [" << szSrc << "] cannot be opened!" << std::endl;
		return false;
	}
	input.seekg(0, std::ios_base::end);
	if ( input.fail() ) {
		std::cout << "Error: [" << szSrc << "] cannot be sought!" << std::endl;
		return false;
	}
	uintptr_t uFileSize;
	{
		std::streampos size = input.tellg();
		if ( size > (unsigned int)((std::numeric_limits<int>::max)()) ) {
			std::cout << "Error: [" << szSrc << "] is too large!" << std::endl;
			return false;
		}
		input.seekg(0, std::ios_base::beg);
		uFileSize = (uintptr_t)(int)size;
	}

	//.h
	strDest = szDest;
	strDest += ".h";
	output.open(strDest, std::ios_base::out);
	if ( !output.is_open() ) {
		std::cout << "Error: [" << strDest << "] cannot be opened!" << std::endl;
		return false;
	}
	write_h_file(output, szVar);
	output.close();

	//.cpp
	strDest = szDest;
	strDest += bCpp ? ".cpp" : ".c";
	output.open(strDest, std::ios_base::out);
	if ( !output.is_open() ) {
		std::cout << "Error: [" << strDest << "] cannot be opened!" << std::endl;
		return false;
	}
	if ( !write_source_file(input, output, szVar, uFileSize) ) {
		std::cout << "Error: cannot write to c/cpp file!" << std::endl;
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////////////////
