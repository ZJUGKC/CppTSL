/*
** Xin YUAN, 2019, BSD (2)
*/

////////////////////////////////////////////////////////////////////////////////

#include "precomp.h"

#include "../WmarkParser.h"

#include "../base/WmarkDef.h"

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
namespace CSL {
////////////////////////////////////////////////////////////////////////////////

// TkTextAction

RdParserAction WmarkParserHelper::get_TkTextAction(RdParserActionMetaData& data)
{
	return [&data](const std::string& strToken, std::vector<std::string>& vecError)->bool
			{
				//text
				assert( data.posParent.uAddress != 0 );
				//string
				size_t uSize = strToken.length();
				if( uSize >= (size_t)((std::numeric_limits<uint32_t>::max)()) )
					return false;
				//node
				RdMetaDataPosition pos = data.spMeta->InsertAstNode(WMARK_NODETYPE_TEXT);
				//token
				RdMetaDataPosition posData = data.spMeta->InsertData((uint32_t)uSize + 1);
				char* szData = (char*)data.spMeta->GetData(posData);
				::memcpy(szData, strToken.c_str(), uSize);
				szData[uSize] = '\0';
				//data
				data.spMeta->SetAstData(pos, posData);
				//link
				data.spMeta->SetAstParent(pos, data.posParent);
				if( data.posCurrent.uAddress == 0 )
					data.spMeta->SetAstChild(data.posParent, pos);
				else
					data.spMeta->SetAstNext(data.posCurrent, pos);
				data.posCurrent = pos;
				return true;
			};
}

////////////////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////////////////
