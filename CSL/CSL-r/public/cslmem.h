/*
** Xin YUAN, 2021, BSD(2)
*/

////////////////////////////////////////////////////////////////////////////////
#ifndef __CSL_MEM_H__
#define __CSL_MEM_H__
////////////////////////////////////////////////////////////////////////////////

#ifndef __CSL_BASE_H__
	#error cslmem.h requires cslbase.h to be included first.
#endif

////////////////////////////////////////////////////////////////////////////////
namespace CSL {
////////////////////////////////////////////////////////////////////////////////

// fixed_element_memory_pool

class fixed_element_memory_pool
{
public:
	fixed_element_memory_pool() noexcept : m_pHead(NULL)
	{
	}
	fixed_element_memory_pool(const fixed_element_memory_pool& src) = delete;
	fixed_element_memory_pool& operator=(const fixed_element_memory_pool& src) = delete;
	~fixed_element_memory_pool() noexcept
	{
		FreeDataChain();
	}

//methods

	// free data links
	void FreeDataChain() noexcept
	{
		void* pPlex = m_pHead;
		while( pPlex != NULL ) {
			void* pNext = get_next_block(pPlex);
			::free(pPlex);
			pPlex = pNext;
		}
		m_pHead = NULL;
	}

	// create block
	void* CreateBlock(uintptr_t uMinElements, uintptr_t uMaxElements, uintptr_t uElementSize, uintptr_t& uActElements)  //may throw
	{
		assert( uMinElements > 0 && uMaxElements > 0 && uMinElements <= uMaxElements && uElementSize > 0 );

		void* pPlex = NULL;
		uintptr_t uBytes = 0;
		uintptr_t uElements = uMaxElements;
		uintptr_t uLinkSize = sizeof(void*);

		uActElements = uElements;
		while( uElements >= uMinElements ) {
			//try
			if( !SafeOperators::Multiply<uintptr_t>(uElements, uElementSize, uBytes)
				|| !SafeOperators::Add<uintptr_t>(uBytes, uLinkSize, uBytes) ) {
				uElements --;
				continue;
			}
			break;
		}
		if( uElements < uMinElements )
			throw std::overflow_error("");

		while( uElements >= uMinElements ) {
			//no overflow
			uBytes = uElements * uElementSize + sizeof(void*);
			pPlex = ::malloc(uBytes);
			if( pPlex == NULL ) {
				uElements --;
				continue;
			}
			break;
		}
		if( pPlex == NULL )
			throw std::bad_alloc("");
		uActElements = uElements;

		get_next_block(pPlex) = m_pHead;
		m_pHead = pPlex;

		return get_data(pPlex);
	}

private:
	static void*& get_next_block(void* pBlock) noexcept
	{
		assert( pBlock != NULL );
		return *((void**)pBlock);
	}
	static void* get_data(void* pBlock) noexcept
	{
		return (void*)(((void**)pBlock) + 1);
	}

private:
	void* m_pHead;
};

// free_list<TNode>
//  TNode : has a member named m_pNext (TNode*)

template <class TNode>
class free_list
{
public:
	explicit free_list(uintptr_t uMinElements = 10, uintptr_t uMaxElements = 10) noexcept
					: m_pFree(NULL), m_uMinBlockElements(uMinElements), m_uMaxBlockElements(uMaxElements)
	{
		assert( uMinElements > 0 && uMaxElements > 0 && uMinElements <= uMaxElements );
	}
	free_list(const free_list&) = delete;
	free_list& operator=(const free_list&) = delete;
	~free_list() noexcept
	{
	}

//methods

	void Clear() noexcept
	{
		m_pFree = NULL;
		m_pool.FreeDataChain();
	}
	//get a free node
	TNode* GetFreeNode()  //may throw
	{
		if( m_pFree == NULL ) {
			uintptr uActElements;
			//may throw
			TNode* pNode = (TNode*)m_pool.CreateBlock(m_uMinBlockElements, m_uMaxBlockElements, sizeof(TNode), uActElements);
			pNode += (uActElements - 1);
			for( uintptr_t uBlock = uActElements; uBlock > 0; uBlock -- ) {
				pNode->m_pNext = m_pFree;
				m_pFree = pNode;
				pNode --;
			}
		}
		assert( m_pFree != NULL );
		return m_pFree;
	}
	//pick free node
	void PickFreeNode() noexcept
	{
		if( m_pFree != NULL )
			m_pFree = m_pFree->m_pNext;
	}
	void PutFreeNode(TNode* pNode) noexcept
	{
		pNode->m_pNext = m_pFree;
		m_pFree = pNode;
	}

private:
	//pool
	fixed_element_memory_pool  m_pool;
	TNode*  m_pFree;  //free list
	uintptr_t m_uMinBlockElements, m_uMaxBlockElements;
};

// fixed_size_memory_pool<t_size>

template <uintptr_t t_size>
class fixed_size_memory_pool
{
private:
	struct _Node
	{
		_Node* m_pNext;
		char   m_data[t_size];
	};

public:
	explicit fixed_size_memory_pool(uintptr_t uMinElements = 10, uintptr_t uMaxElements = 10) noexcept
									: m_uElements(0), m_freelist(uMinElements, uMaxElements)
	{
	}
	fixed_size_memory_pool(const fixed_size_memory_pool&) = delete;
	fixed_size_memory_pool& operator=(const fixed_size_memory_pool&) = delete;
	~fixed_size_memory_pool() noexcept
	{
		//check memory leakage
		assert( m_uElements == 0 );
	}

//methods

	void* Allocate() //may throw
	{
		_Node* pNode = m_freelist.GetFreeNode();  //may throw
		m_freelist.PickFreeNode();

		m_uElements ++;
		assert( m_uElements > 0 );

		return (void*)(pNode->m_data);  //data
	}
	void Free(void* p) noexcept
	{
		//back to node
		_Node* pNode = (_Node*)((unsigned char*)p - (intptr_t)((unsigned char*)(((_Node*)(0x128))->m_data) - (unsigned char*)(0x128)));
		m_freelist.PutFreeNode(pNode);

		assert( m_uElements > 0 );
		m_uElements --;
		if( m_uElements == 0 )
			m_freelist.Clear();
	}

	void Clear() noexcept
	{
		m_freelist.Clear();
		m_uElements = 0;
	}

private:
	uintptr_t m_uElements;

	free_list<_Node>  m_freelist;
};

////////////////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////////////////////////////////
