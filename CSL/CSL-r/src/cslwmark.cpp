/*
** Xin YUAN, 2019, BSD (2)
*/

////////////////////////////////////////////////////////////////////////////////

#include "precomp.h"

#include "wmark/base/WmarkDef.h"

#include "wmark/WmarkScanner.h"
#include "wmark/WmarkParser.h"
#include "wmark/WmarkHtmlGenerator.h"

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
namespace CSL {
////////////////////////////////////////////////////////////////////////////////

// Utility

WmarkUtility::WmarkUtility() throw()
{
}

WmarkUtility::~WmarkUtility() throw()
{
}

bool WmarkUtility::Initialize()
{
	//table
	return WmarkParserHelper::CreateTable(m_spTable);
}

// Parser

WmarkParser::WmarkParser() throw() : m_uMaxErrorNumber(0)
{
}

WmarkParser::~WmarkParser() throw()
{
}

void WmarkParser::Initialize(uint32_t uMaxErrorNumber, const WmarkUtility& wu)
{
	//scanner
	m_spScanner = std::make_shared<RdScanner>();
	WmarkScannerHelper::InitActions(*m_spScanner);
	m_spScanner->SetStartAction(WMARK_SCANNER_TK_ACTION);
	//parser
	m_parser.SetScanner(m_spScanner);
	m_parser.SetTable(wu.m_spTable);
	//data
	m_data.posParent.uAddress = 0;
	m_data.posCurrent.uAddress = 0;
	//actions
	WmarkParserHelper::InitActions(m_parser, m_data);
	//error number
	m_uMaxErrorNumber = uMaxErrorNumber;
}

void WmarkParser::SetInput(const std::shared_ptr<std::istream>& spStream) throw()
{
	assert( m_spScanner.get() != nullptr );
	m_spScanner->SetStream(spStream);
}

void WmarkParser::SetOutput(const std::shared_ptr<RdMetaData>& spMeta) throw()
{
	assert( spMeta.get() != nullptr );
	m_data.spMeta = spMeta;
}

void WmarkParser::Start()
{
	m_parser.Start();
	m_data.posParent.uAddress = 0;
	m_data.posCurrent.uAddress = 0;
}

int32_t WmarkParser::Parse()
{
	int32_t iRet = m_parser.Parse();
	if( iRet == -1 ) {
		if( !m_parser.Revert() )
			return -1;
		return 1;
	}
	if( iRet > 0 ) {
		if( m_uMaxErrorNumber != 0 && m_parser.GetErrorArray().size() >= (size_t)m_uMaxErrorNumber )
			return 0;
	}
	return iRet;
}

const std::vector<std::string>& WmarkParser::GetErrorArray() const throw()
{
	return m_parser.GetErrorArray();
}

// Generator

WmarkHtmlGenerator::WmarkHtmlGenerator() noexcept
{
}
WmarkHtmlGenerator::~WmarkHtmlGenerator() noexcept
{
}

void WmarkHtmlGenerator::Initialize()
{
	assert( m_map.size() == 0 );
	WmarkHtmlGeneratorHelper::InitActions(m_map);
}

bool WmarkHtmlGenerator::Generate(RdMetaData& data, std::ostream& stm)
{
	//symbol
	uint32_t uHash = data.CalcHash(WMARK_ROOT_SYMBOL);
	RdMetaDataPosition pos = data.Find(WMARK_ROOT_SYMBOL, uHash);
	//empty
	if( pos.uAddress == 0 )
		return true;
	RdMetaDataPosition posRoot;
	{
		RdMetaDataInfo info;
		bool bAnalysis;
		data.GetInfo(pos, info, bAnalysis);
		//empty
		if( info.posData.uAddress == 0 )
			return true;
		posRoot = *((RdMetaDataPosition*)data.GetData(info.posData));
		//empty
		if( posRoot.uAddress == 0 )
			return true;
		posRoot = data.GetAstRoot(posRoot);
		//empty
		if( posRoot.uAddress == 0 )
			return true;
	} //end block
	//ast
	RdMetaAstNodeInfo info;
	data.GetAstNodeInfo(posRoot, info);
	//empty
	if( info.posChild.uAddress == 0 )
		return true;
	pos = info.posChild;
	while( pos.uAddress != 0 ) {
		data.GetAstNodeInfo(pos, info);
		auto iter = m_map.find(info.uType);
		if( iter != m_map.end() ) {
			if( !iter->second(true, data, info.posData, stm) )
				return false;
		}
		//child
		if( info.posChild.uAddress != 0 ) {
			pos = info.posChild;
			continue;
		}
		//next
		if( info.posNext.uAddress != 0 ) {
			pos = info.posNext;
			continue;
		}
		//parent
		pos = info.posParent;
		while( pos.uAddress != posRoot.uAddress ) {
			data.GetAstNodeInfo(pos, info);
			iter = m_map.find(info.uType);
			if( iter != m_map.end() ) {
				if( !iter->second(false, data, info.posData, stm) )
					return false;
			}
			if( info.posNext.uAddress != 0 ) {
				pos = info.posNext;
				break;
			}
			pos = info.posParent;
		}
		if( pos.uAddress == posRoot.uAddress )
			pos.uAddress = 0;
	} //end while
	return true;
}

////////////////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////////////////
