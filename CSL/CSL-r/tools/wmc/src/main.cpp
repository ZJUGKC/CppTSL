/*
** Xin YUAN, 2019, BSD (2)
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
		"wmc (c) Wmark Compiler, version: 1.0.0",
		"  Usage : wmc -c <source file> <destination file>",
		NULL
	};
	const char** pp = ppsz;
	while( *pp != NULL ) {
		std::cout << *pp << std::endl;
		pp ++;
	}
}

int main(int argc, char* argv[])
{
	if( argc < 4 ) {
		_print_help();
		return 1;
	}

	//parameters
	if( ::strcmp(argv[1], "-c") == 0 ) {
		if( ::strcmp(argv[2], argv[3]) == 0 ) {
			std::cout << "Error: The source file name and the destination file name cannot be the same!" << std::endl;
			return 1;
		}

		//process
		if( !CSL::ProcessOneFile(argv[2], argv[3]) )
			return 1;
	}
	else {
		_print_help();
		return 1;
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
