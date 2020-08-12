#
# Xin YUAN, 2019, BSD (2)
#

# target

INCLUDE_DIRECTORIES(
	"${CMAKE_CURRENT_BINARY_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}"
	"${PROJECT_SOURCE_DIR}/public"
	"${PROJECT_SOURCE_DIR}/src"
	)

SET(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")
SET(SRC_PCH_LIST "precomp.cpp")
SET(SRC_LIST "cslbase.cpp" "cslsys.cpp" "cslcrypt.cpp" "cslrdp.cpp" "cslwmark.cpp"
	"wmark/scanner_actions/tk_action.cpp" "wmark/scanner_actions/comment_action.cpp" "wmark/scanner_actions/text_action.cpp"
	"wmark/parser_actions/accepted_action.cpp" "wmark/parser_actions/program_action.cpp" "wmark/parser_actions/tk_comment_action.cpp"
	"wmark/parser_actions/block_element_action.cpp" "wmark/parser_actions/berr_tail_action.cpp" "wmark/parser_actions/tk_text_action.cpp"
	"wmark/parser_actions/tk_indent_action.cpp"
	"wmark/generators/CommentGenerator.cpp" "wmark/generators/ParagraphGenerator.cpp" "wmark/generators/TextGenerator.cpp"
	"wmark/generators/IndentGenerator.cpp"
	"wmark/WmarkScanner.cpp" "wmark/WmarkParser.cpp" "wmark/WmarkHtmlGenerator.cpp"
	)
ADD_LIBRARY(CSL-Utility STATIC ${SRC_PCH_LIST} ${SRC_LIST})
SET_PCH(CSL-Utility "exe" "precomp.h" ${SRC_PCH_LIST} ${SRC_LIST})
SET_TARGET_PROPERTIES(CSL-Utility PROPERTIES
	OUTPUT_NAME "CSL-Utility"
	LINK_FLAGS "${STRING_LINK_FLAGS_TARGET}")
TARGET_LINK_LIBRARIES(CSL-Utility ${STRING_LINK_EXTRA_LIBS})

# end
