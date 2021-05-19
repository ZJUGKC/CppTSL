/*
** Xin YUAN, 2021, BSD (2)
*/

////////////////////////////////////////////////////////////////////////////////
#ifndef __CSL_WON_H__
#define __CSL_WON_H__
////////////////////////////////////////////////////////////////////////////////

#ifndef __CSL_RDP_H__
	#error cslwon.h requires cslrdp.h to be included first.
#endif

////////////////////////////////////////////////////////////////////////////////
namespace CSL {
////////////////////////////////////////////////////////////////////////////////

// Utility

class WonUtility
{
public:
	WonUtility() noexcept;
	WonUtility(const WonUtility&) = delete;
	WonUtility& operator=(const WonUtility&) = delete;
	~WonUtility() noexcept;

	bool Initialize();

private:
	//table
	std::shared_ptr<RdaTable>  m_spTable;

private:
	friend class WonParser;
};

// WonTokenTypes

enum WonTokenTypes
{
	Error = -1,
	Unknown = 0,
	Unexpected,
	ValueString,
	ValueNumberInteger,
	ValueNumberFloat,
	ValueNumberHexadecimal,
	ValueBooleanTrue,
	ValueBooleanFalse,
	ValueNull,
	BeginObject,
	EndObject,
	Key,
	EndPair,
	BeginArray,
	EndArray,
	Max = EndArray,
	ValueMin = ValueString,
	ValueMax = ValueNull
};

// WonTokenInfo

struct WonTokenInfo
{
	int iType;  //WonTokenTypes::*
	std::string strValue;
};

// Parser

class WonParser
{
public:
	WonParser() noexcept;
	WonParser(const WonParser&) = delete;
	WonParser& operator=(const WonParser&) = delete;
	~WonParser() noexcept;

	void Initialize(const WonUtility& wu);
	void SetInput(const std::shared_ptr<std::istream>& spStream) noexcept;
	void Start();
	//return value: < 0 (error), = 0 (end), > 0 (succeeded)
	int32_t GetNextToken(WonTokenInfo& info);

private:
	//scanner
	std::shared_ptr<RdScanner> m_spScanner;
	//parser
	RdParser m_parser;

	//info
	WonTokenInfo m_info;
};

#define WON_ROOT_SYMBOL  "won"

/*
Meta data:

| Symbol  | data               |
|:--------|:-------------------|
| "won"   | AST start position |

| AST node type          | data              | child    |
|:-----------------------|:------------------|:---------|
| ValueString            | Symbol            |          |
| ValueNumberInteger     | Symbol            |          |
| ValueNumberFloat       | Symbol            |          |
| ValueNumberHexadecimal | Symbol            |          |
| ValueBooleanTrue       | Symbol            |          |
| ValueBooleanFalse      | Symbol            |          |
| ValueNull              | Symbol            |          |
| BeginObject            | Symbol            | Key      |
| Key                    | Symbol            | Value*** |
| BeginArray             | Symbol            | Value***, Begin*** |
*/

class WonHelper
{
public:
	static bool Parse(WonParser& wp, RdMetaData& data);
};

////////////////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////////////////////////////////
