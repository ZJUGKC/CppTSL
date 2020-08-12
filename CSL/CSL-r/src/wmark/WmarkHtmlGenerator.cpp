/*
** Xin YUAN, 2019, BSD (2)
*/

////////////////////////////////////////////////////////////////////////////////

#include "precomp.h"

#include "WmarkHtmlGenerator.h"

#include "base/WmarkDef.h"

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
namespace CSL {
////////////////////////////////////////////////////////////////////////////////

// WmarkHtmlGeneratorHelper

void WmarkHtmlGeneratorHelper::InitActions(std::map<uint32_t, WmarkMetaDataTraversalAction>& map)
{
	map.insert(std::pair<uint32_t, WmarkMetaDataTraversalAction>(WMARK_NODETYPE_COMMENT, get_CommentGenerator()));
	map.insert(std::pair<uint32_t, WmarkMetaDataTraversalAction>(WMARK_NODETYPE_PARAGRAPH, get_ParagraphGenerator()));
	map.insert(std::pair<uint32_t, WmarkMetaDataTraversalAction>(WMARK_NODETYPE_TEXT, get_TextGenerator()));
	map.insert(std::pair<uint32_t, WmarkMetaDataTraversalAction>(WMARK_NODETYPE_INDENT, get_IndentGenerator()));
}

////////////////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////////////////
