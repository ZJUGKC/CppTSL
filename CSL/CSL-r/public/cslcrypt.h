﻿/*
** Shijie Zhang, 2019, BSD(2)
*/

////////////////////////////////////////////////////////////////////////////////
#ifndef __CSL_CRYPT_H__
#define __CSL_CRYPT_H__
////////////////////////////////////////////////////////////////////////////////

#ifndef __CSL_BASE_H__
	#error cslcrypt.h requires cslbase.h to be included first.
#endif

////////////////////////////////////////////////////////////////////////////////
namespace CSL {
////////////////////////////////////////////////////////////////////////////////

// Md5_Value

struct Md5_Value
{
	uint32_t a, b, c, d;  //128 bits
};

// Md5Calc

class Md5Calc
{
public:
	Md5Calc() noexcept;
	Md5Calc(const Md5Calc&) = delete;
	Md5Calc& operator=(const Md5Calc&) = delete;
	~Md5Calc() noexcept;

	void Initialize() noexcept;
	bool Input(const uint8_t* pBuffer, uint64_t uSize) noexcept;
	bool End() noexcept;

	const Md5_Value& GetValue() const noexcept;

private:
	Md5_Value  m_value;
	uint8_t    m_buffer[64];    //512 bits
	uint32_t   m_size;          //actual size
	uint64_t   m_uTotalSize;    //total size
};

// Md5Helper

class Md5Helper
{
public:
	static void GenerateSalt(std::string& str);
	static void CalcHash(const char* szSrc, const char* szSalt, std::string& str);
};

////////////////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////////////////////////////////
