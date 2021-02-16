#
# Xin YUAN, 2021, BSD (2)
#

# target

INCLUDE_DIRECTORIES(
	"${CMAKE_CURRENT_BINARY_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}"
	"${PROJECT_SOURCE_DIR}/public"
	"${PROJECT_SOURCE_DIR}/src"
	"${PROJECT_SOURCE_DIR}/auxiliary/file2code/src"
	)

SET(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/aux-bin")
SET(SRC_PCH_LIST "precomp.cpp")
SET(SRC_LIST "src/main.cpp" "src/ProcessFile.cpp"
	)
ADD_EXECUTABLE(file2code ${SRC_PCH_LIST} ${SRC_LIST})
SET_PCH(file2code "exe" "precomp.h" ${SRC_PCH_LIST} ${SRC_LIST})
SET_TARGET_PROPERTIES(file2code PROPERTIES
	OUTPUT_NAME "file2code"
	LINK_FLAGS "${STRING_LINK_FLAGS_TARGET_C}")
TARGET_LINK_LIBRARIES(file2code CSL-Utility ${STRING_LINK_EXTRA_LIBS})

# end
