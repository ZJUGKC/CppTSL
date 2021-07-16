/*
** Xin YUAN, 2021, BSD (2)
*/

////////////////////////////////////////////////////////////////////////////////

#include "precomp.h"

#include "ProcessFile.h"

////////////////////////////////////////////////////////////////////////////////

//help
static
void _print_help()
{
	const char* ppsz[] = {
		"file2code (c) File to C/C++ code, version: 1.0.0",
		"  Usage : file2code <-c|-cpp> <source file> <destination file>",
		"    destination file : file name consists of digits, letters and underscores",
		"    e.g., file2code -c pic.png pic_png",
		"          file2code -c res/pic.png res_pic_png",
		NULL
	};
	const char** pp = ppsz;
	while( *pp != NULL ) {
		std::cout << *pp << std::endl;
		pp ++;
	}
}

//process
static
int _cmd_process_file(const char* szSrc, const char* szDest, bool bCpp)
{
	size_t uPos = CSL::FsPathHelper::FindFilePart(szDest);
	//check length of file part
	if ( uPos == ::strlen(szDest) ) {
		std::cout << "Error : The destination file name should be a C variable name!" << std::endl;
		return 1;
	}
	//check name
	const char* sz = szDest + uPos;
	while ( *sz != '\0' ) {
		char ch = *sz;
		if ( ::islower(ch) == 0 && ::isupper(ch) == 0 && ::isdigit(ch) == 0 && ch != '_' ) {
			std::cout << "Error : The destination file name should be a C variable name!" << std::endl;
			return 1;
		}
		sz ++;
	}
	//process
	return CSL::ProcessFile(szSrc, szDest, szDest + uPos, bCpp) ? 0 : 1;
}

int main(int argc, char* argv[])
{
	CSL::ConsoleEncoding ce;

	if( argc != 4 ) {
		_print_help();
		return 1;
	}

	//parameters
	bool bCpp;
	if( ::strcmp(argv[1], "-c") == 0 ) {
		bCpp = false;
	}
	else if( ::strcmp(argv[1], "-cpp") == 0 ) {
		bCpp = true;
	}
	else {
		_print_help();
		return 1;
	}

	//process
	return _cmd_process_file(argv[2], argv[3], bCpp);
}

////////////////////////////////////////////////////////////////////////////////
