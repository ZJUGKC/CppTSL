/*
** Xin YUAN, 2019, BSD (2)
*/

////////////////////////////////////////////////////////////////////////////////

#include "precomp.h"

#include "ProcessFile.h"

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
namespace CSL {
////////////////////////////////////////////////////////////////////////////////

bool ProcessOneFile(const char* szSrc, const char* szDest)
{
	//input
	std::shared_ptr<std::ifstream> spSrc(std::make_shared<std::ifstream>(szSrc, std::ifstream::in | std::ifstream::binary));
	//UTF8 BOM
	StreamHelper::CheckBOM_UTF8(*spSrc);

	//output
	std::shared_ptr<RdMetaData> spMeta(std::make_shared<RdMetaData>());

	//parser
	std::shared_ptr<WmarkUtility> spU(std::make_shared<WmarkUtility>());
	if( !spU->Initialize() ) {
		std::cout << "Error: Initialization failed!" << std::endl;
		return false;
	}
	WmarkParser parser;
	parser.Initialize(100, spU);
	parser.SetInput(std::static_pointer_cast<std::istream, std::ifstream>(spSrc));
	parser.SetOutput(spMeta);
	parser.Start();
	//loop
	bool bFailed = false;
	while( true ) {
		int32_t iRet = parser.Parse();
		if( iRet < 0 ) {
			std::cout << "Error: Parsing failed!" << std::endl;
			bFailed = true;
			break;
		}
		if( iRet == 0 )
			break;
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	//errors
	const std::vector<std::string>& arr(parser.GetErrorArray());
	if( arr.size() > 0 ) {
		for( auto& str : arr )
			std::cout << str << std::endl;
		return false;
	}
	if( bFailed )
		return false;

	//output
	std::shared_ptr<std::ofstream> spDest(std::make_shared<std::ofstream>(szDest, std::ofstream::out | std::ofstream::binary | std::ofstream::trunc));
	WmarkHtmlGenerator gen;
	gen.Initialize();
	if( !gen.Generate(*spMeta, *spDest) ) {
		std::cout << "Error: Output failed!" << std::endl;
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////////////////
