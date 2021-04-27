/*
** Gao Chengxiang, 2021, BSD(2)
*/

////////////////////////////////////////////////////////////////////////////////
#ifndef __CSL_DIFF_H__
#define __CSL_DIFF_H__
////////////////////////////////////////////////////////////////////////////////

#ifndef __CSL_BASE_H__
	#error csldiff.h requires cslbase.h to be included first.
#endif

////////////////////////////////////////////////////////////////////////////////
namespace CSL {
////////////////////////////////////////////////////////////////////////////////

// myers algorithm

typedef std::pair<int, int> myers_point;

//find middle point from start to end in edit graph (a, b)
template <class TIterator>
inline void myers_find_middle(const typename TIterator::value_type* a, const typename TIterator::value_type* b,
							const myers_point& start, const myers_point& end,
							std::vector<int>& vecForward, std::vector<int>& vecBackward,
							myers_point& pt) noexcept
{
	/*
	start 1 2 ... n
	1
	2
	...
	m             end
	*/
	int n = end.first - start.first;    //x
	int m = end.second - start.second;  //y

	//initialize
	for(int i = 0; i <= n + m; ++ i)
		vecForward[i] = -1;
	vecForward[m + 1] = 0;
	for(int i = 0; i <= n + m; ++ i)
		vecBackward[i] = -1;
	vecBackward[n - 1] = n;

	for(int d = 0; d <= (n + m + 1) / 2; d++) {
		//forward
		for(int k = -m + ((d^m)&1), kInv = k + m; k <= n; k += 2, kInv = k + m) {
			int x, y;
			if((kInv == 0 || vecForward[kInv - 1] < 0) && (kInv == m+n || vecForward[kInv + 1] < 0)) {
				continue;
			}
			else if(kInv == 0 || vecForward[kInv - 1] < 0 || (kInv != n + m && vecForward[kInv + 1] >= 0 && vecForward[kInv - 1] < vecForward[kInv + 1])) {
				//now: move from top (k+1)
				x = vecForward[kInv + 1];
				y = x - k;
			}
			else {
				//now: move from left (k-1)
				x = vecForward[kInv - 1] + 1;
				y = x - k;
			}
			if(x > n || y > m || x < 0 || y < 0)
				continue;
			int endx = x, endy = y;
			while(endx < n && endy < m && a[endx + start.first] == b[endy + start.second])
				endx++, endy++;
			vecForward[kInv] = endx;
			if(vecForward[kInv] >= 0 && vecBackward[kInv] >= 0 && vecForward[kInv] >= vecBackward[kInv]) {
				pt.first = x + start.first;
				pt.second = y + start.second;
				return ;
			}
		}

		//backward
		for(int k = -m + ((d^n)&1), kInv = k + m; k <= n; k += 2, kInv = k + m) {
			int x, y;
			if((kInv == 0 || vecBackward[kInv - 1] < 0) && (kInv == m+n || vecBackward[kInv + 1] < 0)) {
				continue;
			}
			else if(kInv == m + n || vecBackward[kInv + 1] < 0 || (kInv != 0 && vecBackward[kInv - 1] >= 0 && vecBackward[kInv - 1] < vecBackward[kInv + 1])) {
				//move from bottom
				x = vecBackward[kInv - 1];
				y = x - k;
			}
			else {
				//move from right
				x = vecBackward[kInv + 1] - 1;
				y = x - k;
			}
			if(x > n || y > m || x < 0 || y < 0)
				continue;
			while(x > 0 && y > 0 && a[x - 1 + start.first] == b[y - 1 + start.second])
				x--, y--;
			vecBackward[kInv] = x;
			if(vecForward[kInv] >= 0 && vecBackward[kInv] >= 0 && vecForward[kInv] >= vecBackward[kInv]) {
				pt.first = x + start.first;
				pt.second = y + start.second;
				return ;
			}
		}
	}
	pt = end;
}

//Longest common sequence
struct LcsItem
{
	int iStart1;  //based 0
	int iEnd1;
	int iStart2;  //based 0
	int iEnd2;
};

//myers algorithm

template <typename TIterator>
inline void MyersDiff(const TIterator& a_begin, const TIterator& a_end,
					const TIterator& b_begin, const TIterator& b_end,
					std::vector<LcsItem>& vecLCS)
{
	//length
    auto a_size = std::distance(a_begin, a_end);
    auto b_size = std::distance(b_begin, b_end);
	assert( a_size >= 0 && b_size >= 0 );
	if( a_size > (std::numeric_limits<int>::max)()
		|| b_size > (std::numeric_limits<int>::max)() )
		throw std::overflow_error("");
	int n = (int)a_size;
	int m = (int)b_size;

	//V[k]: forward & backward
	std::vector<int> vecForward;
	std::vector<int> vecBackward;
	{
		//n+m+2
		int count = SafeOperators::AddThrow(n, m);
		count = SafeOperators::AddThrow(count, (int)2);
		vecForward.resize(count);
		vecBackward.resize(count);
	}

	//result
	vecLCS.clear();

	//add all or delete all
	if( n == 0 || m == 0 )
		return ;

	//division stack
	std::stack<myers_point> vecSearchStack;
	//edit path
	std::vector<myers_point> vecEditPath;

	//initialize
	myers_point start = std::make_pair(0, 0);
	myers_point end = std::make_pair(n, m);
	myers_point middle;

	//loop
	do {
		//same sequence at beginning
		while(start.first != end.first && start.second != end.second && *(&(*a_begin)+start.first) == *(&(*b_begin)+start.second)) {
			start.first++;
			start.second++;
			vecEditPath.push_back(start); //based 1
		}
		//current n < current m
		while(start.first == end.first && start.second != end.second) {
			start.second++;
			vecEditPath.push_back(start);
		}
		//current n > current m
		while(start.first != end.first && start.second == end.second) {
			start.first++;
			vecEditPath.push_back(start);
		}

		//search
		if(start != end) {
			myers_find_middle<TIterator>(&(*a_begin), &(*b_begin), start, end, vecForward, vecBackward, middle);
			vecSearchStack.push(end);
			end = middle;
		}
		else {
			if(!vecSearchStack.empty()) {
				end = vecSearchStack.top();
				vecSearchStack.pop();
			}
		}
	} while(start != end || !vecSearchStack.empty());

	//to LCS
	start = std::make_pair(0, 0);
	for( auto& it : vecEditPath ) {
		//the same
        if(it.first == start.first + 1 && it.second == start.second + 1) {
			if(vecLCS.size() != 0) {
				LcsItem& item(vecLCS[vecLCS.size() - 1]);
				if(item.iEnd1 + 1 == start.first && item.iEnd2 + 1 == start.second) {
					item.iEnd1 = start.first;
					item.iEnd2 = start.second;
					start = it;
					continue;
				}
			}
			LcsItem item;
			item.iStart1 = item.iEnd1 = start.first;
			item.iStart2 = item.iEnd2 = start.second;
			vecLCS.push_back(item);
		}
		else {
			assert( (it.first == start.first && it.second == start.second + 1) // add start.second
					|| (it.first == start.first + 1 && it.second == start.second) // delete start.first
				);
		}
		start = it;
	}
}

//Edit types
enum {
	Diff_Add = 0,
	Diff_Delete,
	Diff_Revise
};

//Diff item
struct DiffItem
{
	int iType;
	int iStart1;  //based 0
	int iEnd1;
	int iStart2;  //based 0
	int iEnd2;
};

inline void LcsToDiff(const std::vector<LcsItem>& vecLCS, int n, int m, std::vector<DiffItem>& vecDiff)
{
	vecDiff.clear();
	if( n == 0 && m == 0 )
		return ;

	DiffItem ditem;
	ditem.iStart1 = 0;
	ditem.iStart2 = 0;
	if( n == 0 ) {
		ditem.iType = Diff_Add;
		ditem.iEnd1 = 0;
		ditem.iEnd2 = m - 1;
		vecDiff.push_back(ditem);
		return ;
	}
	if( m == 0 ) {
		ditem.iType = Diff_Delete;
		ditem.iEnd1 = n - 1;
		ditem.iEnd2 = 0;
		vecDiff.push_back(ditem);
		return ;
	}
	if( vecLCS.size() == 0 ) {
		ditem.iType = Diff_Revise;
		ditem.iEnd1 = n - 1;
		ditem.iEnd2 = m - 1;
		vecDiff.push_back(ditem);
		return ;
	}

	//loop
	int iLast1 = -1;
	int iLast2 = -1;
	for( size_t i = 0; i < vecLCS.size(); i ++ ) {
		const LcsItem& sitem(vecLCS[i]);
		ditem.iStart1 = iLast1 + 1;
		ditem.iStart2 = iLast2 + 1;
		if( sitem.iStart1 == iLast1 + 1 ) {
			if( (i == 0 && sitem.iStart2 != 0) || i != 0 ) {
				assert( (i == 0) || (sitem.iStart2 != iLast2 + 1) );
				ditem.iType = Diff_Add;
				ditem.iEnd1 = iLast1 + 1;
				ditem.iEnd2 = sitem.iStart2 - 1;
				vecDiff.push_back(ditem);
			}
		}
		else {
			if( sitem.iStart2 == iLast2 + 1 ) {
				ditem.iType = Diff_Delete;
				ditem.iEnd1 = sitem.iStart1 - 1;
				ditem.iEnd2 = iLast2 + 1;
				vecDiff.push_back(ditem);
			}
			else {
				ditem.iType = Diff_Revise;
				ditem.iEnd1 = sitem.iStart1 - 1;
				ditem.iEnd2 = sitem.iStart2 - 1;
				vecDiff.push_back(ditem);
			}
		}
		iLast1 = sitem.iEnd1;
		iLast2 = sitem.iEnd2;
	}
	//tail
	ditem.iStart1 = iLast1 + 1;
	ditem.iStart2 = iLast2 + 1;
	if( iLast1 == n - 1 ) {
		if( iLast2 < m - 1 ) {
			ditem.iType = Diff_Add;
			ditem.iEnd1 = iLast1 + 1;
			ditem.iEnd2 = m - 1;
			vecDiff.push_back(ditem);
		}
	}
	else {
		if( iLast2 == m - 1 ) {
			ditem.iType = Diff_Delete;
			ditem.iEnd1 = n - 1;
			ditem.iEnd2 = iLast2 + 1;
			vecDiff.push_back(ditem);
		}
		else {
			ditem.iType = Diff_Revise;
			ditem.iEnd1 = n - 1;
			ditem.iEnd2 = m - 1;
			vecDiff.push_back(ditem);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////////////////////////////////
