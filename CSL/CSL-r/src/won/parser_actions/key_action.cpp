﻿/*
** Xin YUAN, 2021, BSD (2)
*/

////////////////////////////////////////////////////////////////////////////////

#include "precomp.h"

#include "../WonParser.h"

#include "../base/WonDef.h"

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
namespace CSL {
////////////////////////////////////////////////////////////////////////////////

// KeyAction

RdParserAction WonParserHelper::get_KeyAction(WonTokenInfo& info)
{
	return [&info](const std::string& strToken, std::vector<std::string>& vecError)->bool
			{
				info.iType = WonTokenTypes::Key;
				info.strValue = strToken;
				return true;
			};
}

////////////////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////////////////
