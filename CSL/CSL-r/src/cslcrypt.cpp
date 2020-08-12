/*
** Shijie Zhang, 2019, BSD(2)
*/

////////////////////////////////////////////////////////////////////////////////

#include "precomp.h"

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
namespace CSL {
////////////////////////////////////////////////////////////////////////////////

#define shift(x, n)   (((x) << (n)) | ((x) >> (32 - (n))))  //right shift, add 0
#define F(x, y, z)    (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z)    (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z)    ((x) ^ (y) ^ (z))
#define I(x, y, z)    ((y) ^ ((x) | (~z)))
#define A             (0x67452301)
#define B             (0xefcdab89)
#define C             (0x98badcfe)
#define D             (0x10325476)

//ti unsigned int(abs(sin(i+1))*(2^32))
const uint32_t k[64] = {
	0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
	0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501, 0x698098d8,
	0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193,
	0xa679438e, 0x49b40821, 0xf61e2562, 0xc040b340, 0x265e5a51,
	0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
	0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905,
	0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a, 0xfffa3942, 0x8771f681,
	0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60,
	0xbebfbc70, 0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
	0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665, 0xf4292244,
	0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92,
	0xffeff47d, 0x85845dd1, 0x6fa87e4f, 0xfe2ce6e0, 0xa3014314,
	0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};
//numebr of left shift
const uint32_t s[64] = {7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7,
						12, 17, 22, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
						4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 6, 10,
						15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21
};

//process 512 bits
//  M[] : 16 elements
void process_512_group(const uint32_t M[],
					uint32_t& atemp, uint32_t& btemp, uint32_t& ctemp, uint32_t& dtemp) noexcept
{
	uint32_t f, g;
	uint32_t a = atemp;
	uint32_t b = btemp;
	uint32_t c = ctemp;
	uint32_t d = dtemp;
	for ( uint32_t i = 0; i < 64; i ++ ) {
		if ( i < 16 ) {
			f = F(b, c, d);
			g = i;
		}
		else if ( i < 32 ) {
			f = G(b, c, d);
			g = (5 * i + 1) % 16;
		}
		else if ( i < 48 ) {
			f = H(b, c, d);
			g = (3 * i + 5) % 16;
		}
		else {
			f = I(b, c, d);
			g = (7 * i) % 16;
		}
		uint32_t tmp = d;
		d = c;
		c = b;
		b = b + shift((a + f + k[i] + M[g]), s[i]);
		a = tmp;
	}
	atemp = a + atemp;
	btemp = b + btemp;
	ctemp = c + ctemp;
	dtemp = d + dtemp;
}

//convert data to integer array
//  data[] : 64 elements
//  M[] : 16 elements
void convert_to_uint_array(const uint8_t data[], uint32_t M[]) noexcept
{
	for ( uint32_t i = 0; i < 16; i ++ )
		M[i] = 0;
	for ( uint32_t i = 0; i < 64; i ++ )
		M[i >> 2] |= (((uint32_t)(data[i])) << ((i % 4) * 8));
}

const char str16[17] = "0123456789abcdef";

void change_hex(uint32_t a, char h[9]) noexcept
{
	h[8] = 0;
	char* p = &h[7];
	for ( int i = 0; i < 4; i ++ ) {
		//reverse order
		uint8_t b = (uint8_t)a;
		*p -- = str16[b & 0x0F];
		*p -- = str16[b >> 4];
		a >>= 8;
	}
}

////////////////////////////////////////////////////////////////////////////////

// Md5Calc

Md5Calc::Md5Calc() noexcept
{
	Initialize();
}
Md5Calc::~Md5Calc() noexcept
{
}

void Md5Calc::Initialize() noexcept
{
	m_value.a = A;
	m_value.b = B;
	m_value.c = C;
	m_value.d = D;
	m_size    = 0;
	m_uTotalSize = 0;
}

bool Md5Calc::Input(const uint8_t* pBuffer, uint64_t uSize) noexcept
{
	if ( uSize == 0 )
		return true;
	//overflow
	uint64_t tmp;
	if ( !SafeOperators::Add(m_uTotalSize, uSize, tmp) )
		return false;
	m_uTotalSize = tmp;
	//512 bits
	uint32_t grp[16];
	//rest
	const uint8_t* pRest = pBuffer;
	tmp = uSize;
	if ( m_size != 0 ) {
		uint32_t uAdd = 64 - m_size;
		if ( tmp <= uAdd )
			uAdd = (uint32_t)tmp;
		::memcpy(m_buffer + m_size, pRest, uAdd);
		pRest += uAdd;
		tmp -= uAdd;
		m_size += uAdd;
		//update
		if ( m_size == 64 ) {
			convert_to_uint_array(m_buffer, grp);
			process_512_group(grp, m_value.a, m_value.b, m_value.c, m_value.d);
			m_size = 0;
		}
		if ( tmp == 0 )
			return true;
	}
	assert( m_size == 0 );
	//loop
	while ( tmp >= 64 ) {
		//update
		convert_to_uint_array(pRest, grp);
		process_512_group(grp, m_value.a, m_value.b, m_value.c, m_value.d);
		pRest += 64;
		tmp -= 64;
	}
	//rest
	if ( tmp != 0 ) {
		::memcpy(m_buffer, pRest, (size_t)tmp);
		m_size = (uint32_t)tmp;
	}
	return true;
}

bool Md5Calc::End() noexcept
{
	//overflow
	uint64_t tmp;  //bits
	if ( !SafeOperators::Multiply(m_uTotalSize, (uint64_t)8, tmp) )
		return false;
	//512 bits
	uint32_t grp[16];
	//rest
	assert( m_size < 64 );
	//fill
	//  total bits % 512 -> 448
	uint32_t index = m_size;
	m_buffer[index] = 0x80;  //bit 1 0 0 0 0 ...
	for ( ++ index; index < 64; index ++ )
		m_buffer[index] = 0;
	convert_to_uint_array(m_buffer, grp);
	if ( m_size < 64 - 8 ) {
		grp[16 - 2] = (uint32_t)tmp;
		grp[16 - 1] = (uint32_t)(tmp >> 32);
	}
	process_512_group(grp, m_value.a, m_value.b, m_value.c, m_value.d);
	//addition
	if ( m_size >= 64 - 8 ) {
		for ( index = 0; index < 16; index ++ )
			grp[index] = 0;
		grp[16 - 2] = (uint32_t)tmp;
		grp[16 - 1] = (uint32_t)(tmp >> 32);
		process_512_group(grp, m_value.a, m_value.b, m_value.c, m_value.d);
	}
	return true;
}

const Md5_Value& Md5Calc::GetValue() const noexcept
{
	return m_value;
}

// Md5Helper

void Md5Helper::GenerateSalt(std::string& str)
{
	str.clear();
	::srand((unsigned int)::time(NULL));
	for ( int i = 0; i < 16; ++ i ) {
		switch ( ::rand() % 3 ) {
		case 1:
			str += 'A' + ::rand() % 26;
			break;
		case 2:
			str += 'a' + ::rand() % 26;
			break;
		default:
			str += '0' + ::rand() % 10;
			break;
		}
	}
}

void Md5Helper::CalcHash(const char* szSrc, const char* szSalt, std::string& str)
{
	Md5Calc mc;
	mc.Initialize();
	if ( !mc.Input((const uint8_t*)szSrc, ::strlen(szSrc)) )
		throw std::overflow_error("");
	if ( !mc.Input((const uint8_t*)szSalt, ::strlen(szSalt)) )
		throw std::overflow_error("");
	if ( !mc.End() )
		throw std::overflow_error("");
	str.clear();
	const Md5_Value& v = mc.GetValue();
	char h[9];
	change_hex(v.a, h);
	str += h;
	change_hex(v.b, h);
	str += h;
	change_hex(v.c, h);
	str += h;
	change_hex(v.d, h);
	str += h;
	//other ways
	str += szSalt;
}

////////////////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////////////////
