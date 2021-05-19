/*
** Xin YUAN, 2021, BSD (2)
*/

////////////////////////////////////////////////////////////////////////////////

#include "precomp.h"

#include "won/base/WonDef.h"

#include "won/WonScanner.h"
#include "won/WonParser.h"

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
namespace CSL {
////////////////////////////////////////////////////////////////////////////////

// Utility

WonUtility::WonUtility() noexcept
{
}

WonUtility::~WonUtility() noexcept
{
}

bool WonUtility::Initialize()
{
	//table
	return WonParserHelper::CreateTable(m_spTable);
}

// Parser

WonParser::WonParser() noexcept
{
}

WonParser::~WonParser() noexcept
{
}

void WonParser::Initialize(const WonUtility& wu)
{
	//scanner
	m_spScanner = std::make_shared<RdScanner>();
	WonScannerHelper::InitActions(*m_spScanner);
	m_spScanner->SetStartAction(WON_SCANNER_TK_ACTION);
	//parser
	m_parser.SetScanner(m_spScanner);
	m_parser.SetTable(wu.m_spTable);
	//info
	m_info.iType = WonTokenTypes::Unknown;
	//actions
	WonParserHelper::InitActions(m_parser, m_info);
}

void WonParser::SetInput(const std::shared_ptr<std::istream>& spStream) noexcept
{
	assert( m_spScanner.get() != nullptr );
	m_spScanner->SetStream(spStream);
}

void WonParser::Start()
{
	m_parser.Start();
	m_info.iType = WonTokenTypes::Unknown;
}

int32_t WonParser::GetNextToken(WonTokenInfo& info)
{
	info.strValue.clear();

	m_info.iType = WonTokenTypes::Unknown;
	do {
		//parse
		int32_t iRet = m_parser.Parse();
		//error
		if( iRet < 0 ) {
			const std::vector<std::string>& arrError(m_parser.GetErrorArray());
			size_t uCount = arrError.size();
			if( uCount > 0 )
				info.strValue = std::move(arrError[uCount - 1]);
			info.iType = WonTokenTypes::Error;
			if( iRet == -1 )
				info.iType = WonTokenTypes::Unexpected;
			return iRet;
		}
		//end
		if( iRet == 0 )
			break;
		if( m_info.iType != WonTokenTypes::Unknown ) {
			//normal
			info.iType = m_info.iType;
			info.strValue = std::move(m_info.strValue);
			return 1;
		}
		//unknown, continue
	} while( true );

	return 0;
}

// Helper

static void _delete_quotes(std::string& str) noexcept
{
	assert( str.size() >= 2 );
	str.erase(str.size() - 1, 1);
	str.erase(0, 1);
}
static RdMetaDataPosition _add_node(RdMetaData& data, const char* szToken, int iType, const RdMetaDataPosition& posCur)
{
	RdMetaDataPosition pos;

	uint32_t uHash = data.CalcHash(szToken);
	RdMetaDataPosition posSymbol(data.Find(szToken, uHash));
	if( posSymbol.uAddress == 0 )
		posSymbol = data.InsertSymbol(szToken, uHash, 0, false);

	pos = data.InsertAstNode((uint32_t)iType);
	data.SetAstData(pos, posSymbol);
	data.SetAstParent(pos, posCur);
	RdMetaDataPosition posParent(posCur);
	RdMetaAstNodeInfo info;
	if( posCur.uAddress == 0 ) {
		data.GetAstNodeInfo(pos, info);
		posParent = info.posParent;
		assert( posParent.uAddress != 0 );
	}
	data.GetAstNodeInfo(posParent, info);
	if( info.uType == WonTokenTypes::BeginObject || info.uType == WonTokenTypes::BeginArray ) {
		if( info.posChild.uAddress == 0 ) {
			data.SetAstChild(posParent, pos);
		}
		else {
			RdMetaDataPosition pos1(info.posChild);
			data.GetAstNodeInfo(pos1, info);
			RdMetaDataPosition pos2(info.posNext);
			while( pos2.uAddress != 0 ) {
				data.GetAstNodeInfo(pos2, info);
				pos1 = pos2;
				pos2 = info.posNext;
			}
			data.SetAstNext(pos1, pos);
		}
	}
	else {
		assert( info.posChild.uAddress == 0 );
		data.SetAstChild(posParent, pos);
	}

	return pos;
}
static void _leave_current_level(RdMetaData& data, RdMetaDataPosition& pos) noexcept
{
	assert( pos.uAddress != 0 );
	RdMetaAstNodeInfo info;
	data.GetAstNodeInfo(pos, info);
	assert( info.posParent.uAddress != 0 );
	pos = info.posParent;
}

bool WonHelper::Parse(WonParser& wp, RdMetaData& data)
{
	//start
	wp.Start();

	//loop
	RdMetaDataPosition posCurrent;
	posCurrent.uAddress = 0;
	RdMetaDataPosition pos;
	WonTokenInfo info;
	while( true ) {
		int32_t iRet = wp.GetNextToken(info);
		if( iRet < 0 )
			return false;
		if( iRet == 0 )
			break;
		//types
		switch( info.iType ) {
		case WonTokenTypes::ValueString:
			_delete_quotes(info.strValue);
		case WonTokenTypes::ValueNumberInteger:
		case WonTokenTypes::ValueNumberFloat:
		case WonTokenTypes::ValueNumberHexadecimal:
		case WonTokenTypes::ValueBooleanTrue:
		case WonTokenTypes::ValueBooleanFalse:
		case WonTokenTypes::ValueNull:
			pos = _add_node(data, info.strValue.c_str(), info.iType, posCurrent);
 			break;
		case WonTokenTypes::Key:
			_delete_quotes(info.strValue);
		case WonTokenTypes::BeginObject:
		case WonTokenTypes::BeginArray:
			pos = _add_node(data, info.strValue.c_str(), info.iType, posCurrent);
			//enter
			posCurrent = pos;
			break;
		case WonTokenTypes::EndObject:
		case WonTokenTypes::EndPair:
		case WonTokenTypes::EndArray:
			_leave_current_level(data, posCurrent);
			break;
		default:
			return false;
			break;
		}
	} //end while

	//won
	uint32_t uHash = data.CalcHash(WON_ROOT_SYMBOL);
	pos = data.Find(WON_ROOT_SYMBOL, uHash);
	if( pos.uAddress == 0 )
		pos = data.InsertSymbol(WON_ROOT_SYMBOL, uHash, 0, false);
	data.SetData(pos, data.GetAstStart());

	return true;
}

////////////////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////////////////
