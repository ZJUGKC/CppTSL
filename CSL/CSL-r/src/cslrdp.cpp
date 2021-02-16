/*
** Xin YUAN, 2019, BSD (2)
*/

////////////////////////////////////////////////////////////////////////////////

#include "precomp.h"

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
namespace CSL {
////////////////////////////////////////////////////////////////////////////////

// RdScanner

RdScanner::RdScanner() throw() : m_uStartAction(0)
{
}
RdScanner::~RdScanner() throw()
{
}

void RdScanner::SetStream(const std::shared_ptr<std::istream>& spStream) throw()
{
	assert( spStream.get() != NULL );
	m_spStream = spStream;
}

void RdScanner::ClearActions() throw()
{
	m_map.clear();
}

void RdScanner::AddAction(uint32_t uActionID, RdScannerAction&& rsa)
{
	m_map.insert(std::pair<uint32_t, RdScannerAction>(uActionID, std::move(rsa)));
}

void RdScanner::SetStartAction(uint32_t uStartAction) throw()
{
	m_uStartAction = uStartAction;
}

void RdScanner::Start(RdToken& token) throw()
{
	assert( m_spStream.get() != NULL );
	//clear
	token.infoStart.uRow = token.infoStart.uCol = 0;
	token.infoEnd.uRow = token.infoEnd.uCol = 0;
}

bool RdScanner::GetToken(RdToken& token)
{
	assert( m_spStream.get() != NULL );
	token.uID = TK_NULL;
	token.strToken.clear();
	token.infoStart = token.infoEnd;
	uint32_t uNextAction = m_uStartAction;
	//loop
	while( uNextAction != LA_NULL ) {
		uint32_t uActionID = uNextAction;
		uNextAction = LA_NULL;
		auto it(m_map.find(uActionID));
		assert( it != m_map.end() );
		bool bRet = it->second(*(m_spStream.get()), uNextAction, token);
		if( !bRet )
			return false;
	}
	return true;
}

// RdaTable

RdaTable::RdaTable() throw() : m_uStartNT(TK_NULL), m_uMaxTerminalID(TK_START)
{
}

RdaTable::~RdaTable() throw()
{
}

//internal
bool RdaTable::index_to_action(uintptr_t index, bool bPop, int32_t& iAct) throw()
{
	uintptr_t index2 = bPop ? index + 2 : index + 1;
	if( index2 < index )
		return false;
	if( bPop ) {
		if( index2 > (uintptr_t)((std::numeric_limits<int32_t>::max)()) + 1 )
			return false;
		iAct = -((int32_t)index2);
		return true;
	}
	if( index2 > (uintptr_t)((std::numeric_limits<int32_t>::max)()) )
		return false;
	iAct = (int32_t)index2;
	return true;
}

bool RdaTable::generate_first_set(uint32_t uMaxTerminalID)
{
	std::vector<uintptr_t> vecIndex;

	//check eps & terminal
	uintptr_t index = 0;
	auto iter(m_rules.begin());
	for( ; iter != m_rules.end(); ++ iter, ++ index ) {
		if( iter->pRule[0].uToken == TK_EPSILON || iter->pRule[0].uToken <= uMaxTerminalID )
			return false;
		bool bEps = (iter->pRule[1].uToken == TK_EPSILON);
		if( bEps && iter->uNum != 2 )
			return false;
		auto iterF(m_map.find(iter->pRule[0].uToken));
		if( iterF == m_map.end() ) {
			std::shared_ptr<_TableItem> spItem(std::make_shared<_TableItem>());
			spItem->iEpsilon = 0;
			if( bEps ) {
				if( !index_to_action(index, true, spItem->iEpsilon) )
					return false;
			}
			else {
				if( iter->pRule[1].uToken <= uMaxTerminalID ) {
					int32_t iAct;
					if( !index_to_action(index, false, iAct) )
						return false;
					spItem->mapTerminal.insert(std::pair<uint32_t, int32_t>(iter->pRule[1].uToken, iAct));
				}
				else {
					vecIndex.push_back(index);
				}
			}
			m_map.insert(std::pair<uint32_t, std::shared_ptr<_TableItem>>(iter->pRule[0].uToken, spItem));
		}
		else {
			if( bEps ) {
				//two rules are the same.
				if( iterF->second->iEpsilon < 0 )
					return false;
				if( !index_to_action(index, true, iterF->second->iEpsilon) )
					return false;
			}
			else {
				if( iter->pRule[1].uToken <= uMaxTerminalID ) {
					auto iterP(iterF->second->mapTerminal.find(iter->pRule[1].uToken));
					//two rules have the same terminal firstly.
					if( iterP != iterF->second->mapTerminal.end() )
						return false;
					int32_t iAct;
					if( !index_to_action(index, false, iAct) )
						return false;
					iterF->second->mapTerminal.insert(std::pair<uint32_t, int32_t>(iter->pRule[1].uToken, iAct));
				}
				else {
					vecIndex.push_back(index);
				}
			}
		}
	}
	//closure
	uintptr_t uLastSize = vecIndex.size();
	uintptr_t uVecIdx = 0;
	while( uVecIdx < vecIndex.size() ) {
		index = vecIndex[uVecIdx];
		uint32_t uNT = m_rules[index].pRule[1].uToken;
		//find
		bool bFound = false;
		for( auto iter(vecIndex.begin()); iter != vecIndex.end(); ++ iter ) {
			if( uNT == m_rules[*iter].pRule[0].uToken ) {
				bFound = true;
				break;
			}
		}
		if( bFound ) {
			//do next
			uVecIdx ++;
			if( uVecIdx == vecIndex.size() ) {
				//dead loop
				if( uLastSize == vecIndex.size() )
					return false;
				uLastSize = vecIndex.size();
				uVecIdx = 0;
			}
			continue;
		}
		auto iterF(m_map.find(uNT));
		//no rule
		if( iterF == m_map.end() )
			return false;
		//the first nonterminal of right part cannot derive epsilon
		if( iterF->second->iEpsilon < 0 )
			return false;
		auto iterP(m_map.find(m_rules[index].pRule[0].uToken));
		assert( iterP != m_map.end() );
		//propagation
		for( auto iterF2 = iterF->second->mapTerminal.begin();
			iterF2 != iterF->second->mapTerminal.end();
			++ iterF2 ) {
			//two rules have the same terminal firstly.
			auto iterP2(iterP->second->mapTerminal.find(iterF2->first));
			if( iterP2 != iterP->second->mapTerminal.end() )
				return false;
			int32_t iAct;
			if( !index_to_action(index, false, iAct) )
				return false;
			iterP->second->mapTerminal.insert(std::pair<uint32_t, int32_t>(iterF2->first, iAct));
		}
		//remove
		vecIndex.erase(vecIndex.begin() + uVecIdx);
		if( uVecIdx == vecIndex.size() ) {
			uLastSize = vecIndex.size();
			uVecIdx = 0;
		}
	} //end while

	//check null rules
	for( auto iterF(m_map.begin()); iterF != m_map.end(); ++ iterF ) {
		if( iterF->second->mapTerminal.size() == 0 )
			return false;
	}

	return true;
}

//internal class
typedef std::map<uint32_t, std::set<uint32_t>>  PropMap;
typedef std::pair<uint32_t, std::set<uint32_t>>  PropPair;

bool RdaTable::add_follow_set(uint32_t uMaxTerminalID)
{
	//follow
	auto iter(m_rules.begin());
	for( ; iter != m_rules.end(); ++ iter ) {
		if( iter->uNum <= 2 )
			continue;
		for( uintptr_t i = 1; i < iter->uNum - 1; i ++ ) {
			uint32_t uToken = iter->pRule[i].uToken;
			if( uToken == TK_EPSILON || uToken <= uMaxTerminalID )
				continue;
			//nonterminal
			auto iterF(m_map.find(uToken));
			//no rule
			if( iterF == m_map.end() )
				return false;
			//next
			uintptr_t uNext = i + 1;
			while( uNext < iter->uNum ) {
				uint32_t uF = iter->pRule[uNext].uToken;
				if( uF == TK_EPSILON )
					return false;
				if( uF <= uMaxTerminalID ) {
					if( iterF->second->mapTerminal.find(uF) == iterF->second->mapTerminal.end() ) {
						if( iterF->second->sFollow.find(uF) == iterF->second->sFollow.end() )
							iterF->second->sFollow.insert(uF);
					}
					break;
				}
				//NT
				auto iterP(m_map.find(uF));
				if( iterP == m_map.end() )
					return false;
				for( auto iterF2 = iterP->second->mapTerminal.begin();
					iterF2 != iterP->second->mapTerminal.end();
					++ iterF2 ) {
					if( iterF->second->mapTerminal.find(iterF2->first) == iterF->second->mapTerminal.end() ) {
						if( iterF->second->sFollow.find(iterF2->first) == iterF->second->sFollow.end() )
							iterF->second->sFollow.insert(iterF2->first);
					}
				}
				if( !(iterP->second->iEpsilon < 0) )
					break;
				uNext ++;
			}
		}
	} //end for

	//EOE
	auto iterF(m_map.find(m_uStartNT));
	assert( iterF != m_map.end() );
	iterF->second->iEpsilon = -1;  //special
	iterF->second->sFollow.insert(TK_END_OF_EVENT);

	//propagation
	PropMap mapProp;
	std::vector<uint32_t> vecToDo;
	iter = m_rules.begin();
	for( ; iter != m_rules.end(); ++ iter ) {
		if( iter->uNum <= 1 )
			continue;
		auto iterP(mapProp.find(iter->pRule[0].uToken));
		for( uintptr_t i = iter->uNum - 1; i > 0; i -- ) {
			uint32_t uToken = iter->pRule[i].uToken;
			if( uToken == TK_EPSILON || uToken <= uMaxTerminalID )
				break;
			iterF = m_map.find(uToken);
			assert( iterF != m_map.end() );
			if( uToken != iter->pRule[0].uToken ) {
				if( iterP == mapProp.end() ) {
					auto ip = mapProp.insert(PropPair(iter->pRule[0].uToken, std::set<uint32_t>()));
					iterP = ip.first;
					vecToDo.push_back(iter->pRule[0].uToken);
				}
				if( iterP->second.find(uToken) == iterP->second.end() )
					iterP->second.insert(uToken);
			}
			if( !(iterF->second->iEpsilon < 0) )
				break;
		}
	}
	//loop
	while( !vecToDo.empty() ) {
		uint32_t uToken = vecToDo[0];
		vecToDo.erase(vecToDo.begin());
		auto iterP(mapProp.find(uToken));
		if( iterP == mapProp.end() )
			continue;
		iterF = m_map.find(uToken);
		assert( iterF != m_map.end() );
		for( auto iterP2 = iterP->second.begin();
			iterP2 != iterP->second.end();
			++ iterP2 ) {
			uint32_t uNT = *iterP2;
			auto iterF2(m_map.find(uNT));
			assert( iterF2 != m_map.end() );
			bool bChanged = false;
			for( auto iterF3 = iterF->second->sFollow.begin();
				iterF3 != iterF->second->sFollow.end();
				++ iterF3 ) {
				if( iterF2->second->mapTerminal.find(*iterF3) == iterF2->second->mapTerminal.end() ) {
					if( iterF2->second->sFollow.find(*iterF3) == iterF2->second->sFollow.end() ) {
						iterF2->second->sFollow.insert(*iterF3);
						bChanged = true;
					}
				}
			}
			if( bChanged ) {
				bool bFound = false;
				for( auto iterV = vecToDo.begin(); iterV != vecToDo.end(); ++ iterV ) {
					if( uNT == *iterV ) {
						bFound = true;
						break;
					}
				}
				if( !bFound )
					vecToDo.push_back(uNT);
			}
		}
	} //end while

	return true;
}

bool RdaTable::Generate(const RULEELEMENT* pRules, uint32_t uMaxTerminalID)
{
	assert( pRules != NULL );
	m_map.clear();
	m_rules.clear();

	//rules
	const RULEELEMENT* p = pRules;
	while( p->uToken != TK_NULL ) {
		RULEITEM item;
		item.pRule = p;
		item.uNum  = 0;
		while( p->uToken != TK_NULL ) {
			(item.uNum) ++;
			p ++;
		}
		if( item.uNum == 1 )
			return false;
		//left part
		if( item.pRule->uToken <= uMaxTerminalID )
			return false;
		//left recursion
		if( item.pRule->uToken == item.pRule[1].uToken )
			return false;
		//add
		m_rules.push_back(item);
		p ++;
	}

	if( m_rules.empty() )
		return false;

	m_uStartNT = m_rules[0].pRule[0].uToken;

	//table
	if( !generate_first_set(uMaxTerminalID) )
		return false;
	if( !add_follow_set(uMaxTerminalID) )
		return false;

	m_uMaxTerminalID = uMaxTerminalID;

	return true;
}

uint32_t RdaTable::GetStartNT() const throw()
{
	return m_uStartNT;
}

uint32_t RdaTable::GetMaxTerminalID() const throw()
{
	return m_uMaxTerminalID;
}

int32_t RdaTable::Input(uint32_t uNonterminal, uint32_t uTerminal) const throw()
{
	auto iterN(m_map.find(uNonterminal));
	if( iterN == m_map.end() )
		return 0;
	auto iterT(iterN->second->mapTerminal.find(uTerminal));
	if( iterT != iterN->second->mapTerminal.end() )
		return iterT->second;
	if( iterN->second->iEpsilon < 0 ) {
		auto iterF(iterN->second->sFollow.find(uTerminal));
		if( iterF != iterN->second->sFollow.end() )
			return iterN->second->iEpsilon;
	}
	return 0;
}

void RdaTable::GetRule(uintptr_t index, RULEITEM& item) const throw()
{
	assert( index < m_rules.size() );
	item = m_rules[index];
}

// RdParser

#define TK_NO_EVENT      ((uint32_t)-3)

RdParser::RdParser() throw() : m_uMaxTerminalID(TK_NULL)
{
}

RdParser::~RdParser() throw()
{
}

void RdParser::SetScanner(const std::shared_ptr<RdScanner>& sp) throw()
{
	assert( sp.get() != nullptr );
	m_spScanner = sp;
}

void RdParser::SetTable(const std::shared_ptr<RdaTable>& sp) throw()
{
	assert( sp.get() != nullptr );
	m_spTable = sp;
}

void RdParser::ClearActions() throw()
{
	m_map.clear();
}

void RdParser::AddAction(uint32_t uActionID, RdParserAction&& rpa)
{
	m_map.insert(std::pair<uint32_t, RdParserAction>(uActionID, std::move(rpa)));
}

void RdParser::SetAcceptedAction(RdParserAction&& rpa) throw()
{
	m_rpaAccepted = std::move(rpa);
}

void RdParser::Start()
{
	m_uMaxTerminalID = m_spTable->GetMaxTerminalID();
	m_spScanner->Start(m_token);
	m_vecError.clear();
	while( !m_stack.empty() )
		m_stack.pop();
	RULEELEMENT elem;
	elem.uToken = m_spTable->GetStartNT();
	elem.uAction = LA_NULL;
	m_stack.push(elem);
	m_uCurrentEvent = TK_NO_EVENT;
	m_bEmpty = true;
}

//internal
void RdParser::append_unexpected_error()
{
	//format
	char tmp[1024];
	::snprintf(tmp, sizeof(tmp), "Error [%u, %u] [%s] :",
			m_token.infoStart.uRow + 1, m_token.infoStart.uCol,
			m_token.strToken.c_str());
	std::string strTmp(tmp);
	strTmp += " unexpected.";
	m_vecError.push_back(std::move(strTmp));
}
bool RdParser::do_action(uint32_t uActionID)
{
	if( uActionID != LA_NULL ) {
		auto iterA(m_map.find(uActionID));
		if( iterA != m_map.end() ) {
			if( iterA->second != nullptr ) {
				if( !iterA->second(m_token.strToken, m_vecError) )
					return false;
			}
		}
	}
	return true;
}

int32_t RdParser::Parse()
{
	//loop
	while( true ) {
		//input event
		if( m_uCurrentEvent == TK_NO_EVENT ) {
			bool bRet = m_spScanner->GetToken(m_token);
			if( !bRet ) {
				append_unexpected_error();
				return -2;
			}
			if( m_token.uID == TK_ERROR ) {
				append_unexpected_error();
				return -2;
			}
			if( m_token.uID == TK_NULL )
				continue;
			m_uCurrentEvent = m_token.uID;
		}

		//end
		if( m_stack.empty() ) {
			if( m_uCurrentEvent != TK_END_OF_EVENT ) {
				append_unexpected_error();
				return -2;
			}
			//accepted
			if( m_rpaAccepted != nullptr ) {
				if( !m_rpaAccepted(m_token.strToken, m_vecError) )
					return -2;
			}
			return 0;
		}

		//top
		RULEELEMENT elem = m_stack.top();

		// terminal
		if( elem.uToken <= m_uMaxTerminalID ) {
			if( elem.uToken != m_uCurrentEvent ) {
				append_unexpected_error();
				return -1;
			}
			m_stack.pop();
			//action
			if( !do_action(elem.uAction) )
				return -2;
			//reset
			m_uCurrentEvent = TK_NO_EVENT;
			break;
		}

		// nonterminal
		int32_t iAct = m_spTable->Input(elem.uToken, m_uCurrentEvent);
		if( iAct == 0 ) {
			append_unexpected_error();
			return -1;
		}
		RULEITEM item;
		if( iAct > 0 ) {
			//rule
			m_spTable->GetRule(iAct - 1, item);
			//action
			if( !do_action(item.pRule[0].uAction) )
				return -2;
			//pop
			m_stack.pop();
			//push
			for( uintptr_t i = item.uNum - 1; i > 0; i -- )
				m_stack.push(item.pRule[i]);
			//flag
			if( m_bEmpty )
				m_bEmpty = false;
		}
		else {
			//action
			if( iAct < -1 ) {
				m_spTable->GetRule(-(iAct + 2), item);
				if( !do_action(item.pRule[0].uAction) )
					return -2;
			}
			//pop
			m_stack.pop();
		}
	} //end while

	return 1;
}

bool RdParser::Revert()
{
	if( m_stack.empty() )
		return false;
	RdPushDownStack skTemp;
	//stack
	RULEELEMENT elem = m_stack.top();
	while( true ) {
		m_stack.pop();
		skTemp.push(elem);
		if( m_stack.empty() )
			break;
		elem = m_stack.top();
		//check
		if( elem.uToken <= m_uMaxTerminalID ) {
			if( m_uCurrentEvent == elem.uToken )
				return true;
		}
		else {
			int32_t iAct = m_spTable->Input(elem.uToken, m_uCurrentEvent);
			if( iAct != 0 )
				return true;
		}
	}
	while( !skTemp.empty() ) {
		m_stack.push(skTemp.top());
		skTemp.pop();
	}
	//input
	elem = m_stack.top();
	while( true ) {
		bool bRet = m_spScanner->GetToken(m_token);
		if( !bRet )
			return false;
		if( m_token.uID == TK_ERROR )
			return false;
		if( m_token.uID == TK_NULL )
			continue;
		if( m_token.uID == TK_END_OF_EVENT )
			return false;
		m_uCurrentEvent = m_token.uID;
		//check
		if( elem.uToken <= m_uMaxTerminalID ) {
			if( m_uCurrentEvent == elem.uToken )
				break;
		}
		else {
			int32_t iAct = m_spTable->Input(elem.uToken, m_uCurrentEvent);
			if( iAct != 0 )
				break;
		}
	}
	return true;
}

bool RdParser::IsEmpty() const throw()
{
	return m_bEmpty;
}

const std::vector<std::string>& RdParser::GetErrorArray() const throw()
{
	return m_vecError;
}

// RdAllocator

RdAllocator::RdAllocator() throw()
{
}

RdAllocator::~RdAllocator() throw()
{
}

uint32_t RdAllocator::Allocate(uint32_t uBytes)
{
	assert( uBytes != 0 );
	uint32_t uOld = (uint32_t)m_vec.size();
	if( uOld == 0 )
		uOld = sizeof(uint32_t);
	//round
	uint32_t uAct = SafeOperators::AddThrow(uBytes, (uint32_t)sizeof(uint32_t) - 1);
	uAct = uAct / sizeof(uint32_t) * sizeof(uint32_t);
	//resize
	uint32_t uNew = SafeOperators::AddThrow(uOld, uAct);
	m_vec.resize(uNew);
	*((uint32_t*)(&m_vec[0])) = uNew - sizeof(uint32_t);
	return uOld;
}

void* RdAllocator::ToPointer(uint32_t uAddress) throw()
{
	if( m_vec.empty() || uAddress == 0 )
		return nullptr;
	if( uAddress >= (uint32_t)m_vec.size() )
		return nullptr;
	return &m_vec[uAddress];
}
const void* RdAllocator::ToPointer(uint32_t uAddress) const throw()
{
	if( m_vec.empty() || uAddress == 0 )
		return nullptr;
	if( uAddress >= (uint32_t)m_vec.size() )
		return nullptr;
	return &m_vec[uAddress];
}

// RdMetaData

RdMetaData::RdMetaData() throw() : m_uSymbolStart(0), m_uBins(103), m_uZeroLevelHead(0),
								m_uAstStart(0)
{
}

RdMetaData::~RdMetaData() throw()
{
}

uint32_t RdMetaData::CalcHash(const char* szSymbol)
{
	return (uint32_t)((std::hash<std::string>()(std::string(szSymbol))) % m_uBins);
}

//internal
RdMetaDataPosition RdMetaData::find_next(const char* szSymbol, uint32_t uHashCode, RdMetaDataPosition pos) const throw()
{
	while( pos.uAddress != 0 ) {
		const _SymbolNode* p = (const _SymbolNode*)m_raPool.ToPointer(pos.uAddress);
		if( p->uHashCode == uHashCode && ::strcmp(szSymbol, (const char*)m_raString.ToPointer(p->uKey)) == 0 )
			break;
		pos.uAddress = p->uHashNext;
	}
	return pos;
}

RdMetaDataPosition RdMetaData::Find(const char* szSymbol, uint32_t uHashCode) const throw()
{
	assert( szSymbol != nullptr );
	RdMetaDataPosition pos;
	pos.uAddress = 0;
	if( m_uSymbolStart == 0 )
		return pos;
	const uint32_t* p = (const uint32_t*)m_raPool.ToPointer(m_uSymbolStart + sizeof(uint32_t) + uHashCode * sizeof(uint32_t));
	if( p == nullptr )
		return pos;
	if( *p == 0 )
		return pos;
	pos.uAddress = *p;
	pos = find_next(szSymbol, uHashCode, pos);
	return pos;
}	

RdMetaDataPosition RdMetaData::FindNext(RdMetaDataPosition pos) const throw()
{
	if( pos.uAddress == 0 )
		return pos;
	const _SymbolNode* p = (const _SymbolNode*)m_raPool.ToPointer(pos.uAddress);
	pos.uAddress = p->uHashNext;
	pos = find_next((const char*)m_raString.ToPointer(p->uKey), p->uHashCode, pos);
	return pos;
}

RdMetaDataPosition RdMetaData::GetZeroLevelHead() const throw()
{
	RdMetaDataPosition pos;
	pos.uAddress = m_uZeroLevelHead;
	return pos;
}

RdMetaDataPosition RdMetaData::GetLevelNext(RdMetaDataPosition pos) const throw()
{
	if( pos.uAddress == 0 )
		return pos;
	const _SymbolNode* p = (const _SymbolNode*)m_raPool.ToPointer(pos.uAddress);
	pos.uAddress = p->uLevelNext;
	return pos;
}

void RdMetaData::GetInfo(RdMetaDataPosition pos, RdMetaDataInfo& info, bool& bAnalysis) const throw()
{
	assert( pos.uAddress != 0 );
	const _SymbolNode* p = (const _SymbolNode*)m_raPool.ToPointer(pos.uAddress);
	info.szKey = (const char*)m_raString.ToPointer(p->uKey);
	info.uType = p->uFlags;
	info.uLevel = p->uLevel & MASK_LEVEL;
	bAnalysis = (p->uLevel & MASK_ANALYSIS) != 0;
	info.posData.uAddress = p->uData;
}

void RdMetaData::SetType(RdMetaDataPosition pos, uint32_t uType) throw()
{
	assert( pos.uAddress != 0 );
	_SymbolNode* p = (_SymbolNode*)m_raPool.ToPointer(pos.uAddress);
	p->uFlags = uType;
}

void RdMetaData::SetLevel(RdMetaDataPosition pos, uint32_t uLevel) throw()
{
	assert( pos.uAddress != 0 );
	_SymbolNode* p = (_SymbolNode*)m_raPool.ToPointer(pos.uAddress);
	p->uLevel = (p->uLevel & MASK_ANALYSIS) | (uLevel & MASK_LEVEL);
}

void RdMetaData::SetData(RdMetaDataPosition pos, RdMetaDataPosition posData) throw()
{
	assert( pos.uAddress != 0 );
	_SymbolNode* p = (_SymbolNode*)m_raPool.ToPointer(pos.uAddress);
	p->uData = posData.uAddress;
}

void RdMetaData::ClearAnalysisFlag(RdMetaDataPosition pos) throw()
{
	assert( pos.uAddress != 0 );
	_SymbolNode* p = (_SymbolNode*)m_raPool.ToPointer(pos.uAddress);
	p->uLevel &= (~MASK_ANALYSIS);
}

RdMetaDataPosition RdMetaData::InsertSymbol(const char* szSymbol, uint32_t uType, bool bLevelLink)
{
	assert( szSymbol != nullptr );
	uint32_t uHashCode = CalcHash(szSymbol);
	//allocate
	if( m_uSymbolStart == 0 ) {
		uint32_t uBytes = sizeof(uint32_t) + m_uBins * sizeof(uint32_t);
		m_uSymbolStart = m_raPool.Allocate(uBytes);
		::memset(m_raPool.ToPointer(m_uSymbolStart), 0, uBytes);
	}
	if( m_stkLevel.empty() )
		m_stkLevel.push(0);
	size_t uSize = ::strlen(szSymbol);
	if( uSize > (size_t)((std::numeric_limits<uint32_t>::max)()) - 1 )
		throw std::overflow_error("");
	uint32_t uKey = m_raString.Allocate((uint32_t)uSize + 1);
	char* szKey = (char*)m_raString.ToPointer(uKey);
	::memcpy(szKey, szSymbol, uSize);
	szKey[uSize] = '\0';
	RdMetaDataPosition pos;
	pos.uAddress = m_raPool.Allocate(sizeof(_SymbolNode));
	//fill
	_SymbolNode* pNode = (_SymbolNode*)m_raPool.ToPointer(pos.uAddress);
	pNode->uFlags = uType;
	pNode->uLevel = (uint32_t)m_stkLevel.size() - 1;
	pNode->uKey = uKey;
	pNode->uHashCode = uHashCode;
	pNode->uHashNext = 0;
	pNode->uLevelNext = 0;
	if( bLevelLink ) {
		uint32_t& uHead = m_stkLevel.top();
		pNode->uLevelNext = uHead;
		uHead = pos.uAddress;
	}
	pNode->uData = 0;
	//link
	uint32_t* pStart = (uint32_t*)m_raPool.ToPointer(m_uSymbolStart);
	uint32_t uNewCount = SafeOperators::AddThrow<uint32_t>(pStart[0], 1);
	pStart[0] = uNewCount;
	pNode->uHashNext = pStart[1 + uHashCode];
	pStart[1 + uHashCode] = pos.uAddress;
	return pos;
}

//internal
uint32_t RdMetaData::reverse_level_link(uint32_t uHead) throw()
{
	uint32_t uNode = uHead;
	uHead = 0;
	while( uNode != 0 ) {
		_SymbolNode* pNode = (_SymbolNode*)m_raPool.ToPointer(uNode);
		uint32_t uNext = pNode->uLevelNext;
		pNode->uLevelNext = uHead;
		uHead = uNode;
		uNode = uNext;
	}
	return uHead;
}

void RdMetaData::EnterLevel()
{
	if( m_stkLevel.empty() )
		m_stkLevel.push(0);
	if( m_stkLevel.size() > MAX_LEVEL )
		throw std::overflow_error("");
	m_stkLevel.push(0);
}
RdMetaDataPosition RdMetaData::LeaveLevel(bool bReverseLevelLink) throw()
{
	RdMetaDataPosition pos;
	pos.uAddress = 0;
	if( m_stkLevel.empty() )
		return pos;
	//pop
	uint32_t uHead = m_stkLevel.top();
	m_stkLevel.pop();
	//reverse
	if( bReverseLevelLink )
		uHead = reverse_level_link(uHead);
	//clear analysis flags
	uint32_t uNode = uHead;
	while( uNode != 0 ) {
		_SymbolNode* pNode = (_SymbolNode*)m_raPool.ToPointer(uNode);
		pNode->uLevel &= (~MASK_ANALYSIS);
		uNode = pNode->uLevelNext;
	}
	pos.uAddress = uHead;
	return pos;
}
uint32_t RdMetaData::GetCurrentLevel() const throw()
{
	return m_stkLevel.empty() ? 0 : (uint32_t)m_stkLevel.size() - 1;
}
void RdMetaData::FinishZeroLevel(bool bReverseLevelLink) throw()
{
	RdMetaDataPosition pos(LeaveLevel(bReverseLevelLink));
	assert( m_stkLevel.empty() );
	m_uZeroLevelHead = pos.uAddress;
}
RdMetaDataPosition RdMetaData::ReverseLevelLink(RdMetaDataPosition pos) throw()
{
	pos.uAddress = reverse_level_link(pos.uAddress);
	return pos;
}

RdMetaDataPosition RdMetaData::InsertData(uint32_t uSize)
{
	RdMetaDataPosition pos;
	pos.uAddress = m_raData.Allocate(uSize);
	return pos;
}
void* RdMetaData::GetData(RdMetaDataPosition posData) throw()
{
	assert( posData.uAddress != 0 );
	return m_raData.ToPointer(posData.uAddress);
}
const void* RdMetaData::GetData(RdMetaDataPosition posData) const throw()
{
	assert( posData.uAddress != 0 );
	return m_raData.ToPointer(posData.uAddress);
}

RdMetaDataPosition RdMetaData::InsertAstNode(uint32_t uType)
{
	//allocate
	if( m_uAstStart == 0 ) {
		m_uAstStart = m_raAst.Allocate(sizeof(uint32_t) + sizeof(uint32_t));
		uint32_t* pStart = (uint32_t*)m_raAst.ToPointer(m_uAstStart);
		pStart[0] = 0;
		pStart[1] = 0;
	}
	//root
	uint32_t* pStart = (uint32_t*)m_raAst.ToPointer(m_uAstStart);
	if( pStart[1] == 0 ) {
		uint32_t uRoot = m_raAst.Allocate(sizeof(_AstNode));
		::memset(m_raAst.ToPointer(uRoot), 0, sizeof(_AstNode));
		pStart = (uint32_t*)m_raAst.ToPointer(m_uAstStart);
		pStart[1] = uRoot;
	}
	//node
	RdMetaDataPosition pos;
	pos.uAddress = m_raAst.Allocate(sizeof(_AstNode));
	_AstNode* pNode = (_AstNode*)m_raAst.ToPointer(pos.uAddress);
	pNode->uType   = uType;
	pNode->uParent = 0;
	pNode->uChild  = 0;
	pNode->uNext   = 0;
	pNode->uData   = 0;
	pStart = (uint32_t*)m_raAst.ToPointer(m_uAstStart);
	uint32_t uNewCount = SafeOperators::AddThrow<uint32_t>(pStart[0], 1);
	pStart[0] = uNewCount;
	return pos;
}
void RdMetaData::SetAstParent(RdMetaDataPosition pos, RdMetaDataPosition posParent) throw()
{
	assert( pos.uAddress != 0 );
	_AstNode* pNode = (_AstNode*)m_raAst.ToPointer(pos.uAddress);
	pNode->uParent = posParent.uAddress;
}
void RdMetaData::SetAstChild(RdMetaDataPosition pos, RdMetaDataPosition posChild) throw()
{
	assert( pos.uAddress != 0 );
	_AstNode* pNode = (_AstNode*)m_raAst.ToPointer(pos.uAddress);
	pNode->uChild = posChild.uAddress;
}
void RdMetaData::SetAstNext(RdMetaDataPosition pos, RdMetaDataPosition posNext) throw()
{
	assert( pos.uAddress != 0 );
	_AstNode* pNode = (_AstNode*)m_raAst.ToPointer(pos.uAddress);
	pNode->uNext = posNext.uAddress;
}
void RdMetaData::SetAstData(RdMetaDataPosition pos, RdMetaDataPosition posData) throw()
{
	assert( pos.uAddress != 0 );
	_AstNode* pNode = (_AstNode*)m_raAst.ToPointer(pos.uAddress);
	pNode->uData = posData.uAddress;
}
void RdMetaData::GetAstNodeInfo(RdMetaDataPosition pos, RdMetaAstNodeInfo& info) const throw()
{
	assert( pos.uAddress != 0 );
	const _AstNode* pNode = (const _AstNode*)m_raAst.ToPointer(pos.uAddress);
	info.uType = pNode->uType;
	info.posParent.uAddress = pNode->uParent;
	info.posChild.uAddress  = pNode->uChild;
	info.posNext.uAddress   = pNode->uNext;
	info.posData.uAddress   = pNode->uData;
}
void RdMetaData::SetAstLinkParent(RdMetaDataPosition posHead, RdMetaDataPosition posParent) throw()
{
	uint32_t uNode = posHead.uAddress;
	while( uNode != 0 ) {
		_AstNode* pNode = (_AstNode*)m_raAst.ToPointer(uNode);
		pNode->uParent = posParent.uAddress;
		uNode = pNode->uNext;
	}
}
RdMetaDataPosition RdMetaData::ReverseAstLink(RdMetaDataPosition posHead) throw()
{
	uint32_t uNode = posHead.uAddress;
	uint32_t uHead = 0;
	while( uNode != 0 ) {
		_AstNode* pNode = (_AstNode*)m_raAst.ToPointer(uNode);
		uint32_t uNext = pNode->uNext;
		pNode->uNext = uHead;
		uHead = uNode;
		uNode = uNext;
	}
	posHead.uAddress = uHead;
	return posHead;
}
RdMetaDataPosition RdMetaData::GetAstStart() const throw()
{
	RdMetaDataPosition pos;
	pos.uAddress = m_uAstStart;
	return pos;
}
void RdMetaData::ResetAst() throw()
{
	m_uAstStart = 0;
}
RdMetaDataPosition RdMetaData::GetAstRoot(RdMetaDataPosition posStart) const throw()
{
	assert( posStart.uAddress != 0 );
	uint32_t* pStart = (uint32_t*)m_raAst.ToPointer(posStart.uAddress);
	RdMetaDataPosition posRet;
	posRet.uAddress = pStart[1];
	return posRet;
}

////////////////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////////////////
