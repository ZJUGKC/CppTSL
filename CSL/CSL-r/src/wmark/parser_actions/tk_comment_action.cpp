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

// TkCommentAction

RdParserAction WmarkParserHelper::get_TkCommentAction(RdParserActionMetaData& data)
{
	return [&data](const std::string& strToken, std::vector<std::string>& vecError)->bool
			{
				//comment
				assert( data.posParent.uAddress != 0 );
				RdMetaDataPosition pos = data.spMeta->InsertAstNode(WMARK_NODETYPE_COMMENT);
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
