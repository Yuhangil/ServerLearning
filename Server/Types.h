#pragma once

#include <cmath>



typedef struct _tagVECTOR_INT
{
	int x, y, z;
	_tagVECTOR_INT() :
		x(0), y(0), z(0)
	{
	}
	_tagVECTOR_INT(int _x, int _y, int _z) :
		x(_x), y(_y), z(_z)
	{
	}
	_tagVECTOR_INT(int _x, int _z) :
		x(_x), y(0), z(_z)
	{
	}

	static _tagVECTOR_INT Cross(_tagVECTOR_INT v1, _tagVECTOR_INT v2)
	{
		return _tagVECTOR_INT(
			v1.y * v2.z - v1.z * v2.y,
			v1.z * v2.x - v1.x * v2.z,
			v1.x * v2.y - v1.y * v2.x);
	}
	void operator = (const _tagVECTOR_INT& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
	}

	bool operator==(const _tagVECTOR_INT& v)
	{
		if (x == v.x && y == v.y && z == v.z)
			return true;
		return false;

	}
	bool operator !=(const _tagVECTOR_INT& v)
	{
		if (x == v.x && y == v.y && z == v.z)
			return false;
		return true;
	}

	_tagVECTOR_INT operator%(const int i) const
	{
		return _tagVECTOR_INT(x % i, y % i, z % i);
	}

	_tagVECTOR_INT operator +(const _tagVECTOR_INT& v) const
	{
		_tagVECTOR_INT tV;
		tV.x = x + v.x;
		tV.y = y + v.y;
		tV.z = z + v.z;
		return tV;
	}
	void operator +=(const _tagVECTOR_INT& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
	}
	_tagVECTOR_INT operator *(float f) const
	{
		_tagVECTOR_INT tV;
		tV.x = x * f;
		tV.y = y * f;
		tV.z = z * f;
		return tV;
	}
}VECTOR_INT, _SIZE;

typedef struct _tagVECTOR
{
	float x, y, z;
	_tagVECTOR() :
		x(0), y(0), z(0)
	{
	}
	_tagVECTOR(float _x, float _y, float _z) :
		x(_x), y(_y), z(_z)
	{
	}
	_tagVECTOR(float _x, float _z) :
		x(_x), y(0), z(_z)
	{
	}

	static _tagVECTOR Cross(_tagVECTOR v1, _tagVECTOR v2)
	{
		return _tagVECTOR(
			v1.y * v2.z - v1.z * v2.y,
			v1.z * v2.x - v1.x * v2.z,
			v1.x * v2.y - v1.y * v2.x);
	}
	float Magnitude()
	{
		return sqrtf(x * x + y * y + z * z);
	}

	bool operator==(const _tagVECTOR& v)
	{
		if (x == v.x && y == v.y && z == v.z)
			return true;
		return false;

	}
	bool operator !=(const _tagVECTOR& v)
	{
		if (x == v.x && y == v.y && z == v.z)
			return false;
		return true;
	}

	void operator = (const _tagVECTOR& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
	}

	_tagVECTOR_INT operator%(const int i) const
	{
		return _tagVECTOR_INT((int)x % i, (int)y % i, (int)z % i);
	}

	_tagVECTOR operator +(const _tagVECTOR& v) const
	{
		_tagVECTOR tV;
		tV.x = x + v.x;
		tV.y = y + v.y;
		tV.z = z + v.z;
		return tV;
	}
	void operator +=(const _tagVECTOR& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
	}
	_tagVECTOR operator *(float f) const
	{
		_tagVECTOR tV;
		tV.x = x * f;
		tV.y = y * f;
		tV.z = z * f;
		return tV;
	}
	_tagVECTOR operator /(float f) const
	{
		_tagVECTOR tV;
		tV.x = x / f;
		tV.y = y / f;
		tV.z = z / f;
		return tV;
	}
}VECTOR;

typedef struct SOCKET_DATA
{
	int header; // 0~7 bit checksum , 8~23bit data length, 24~31 bit data type
	char Message[512];
}DATA;

typedef struct STRUCTURE_DATA
{
	unsigned int structure_id;
	VECTOR_INT pos;
	_SIZE size;
}STRUCTURE_DATA;

typedef struct SOCKET_INFO
{
	SOCKET socket;
	DATA message;
	int receiveBytes;
	int sendBytes;
	clock_t last_send;
} SOCKET_INFO;

typedef struct TERRAIN_INFO
{
	float main;
	float* otherBiomes;
	int structure;
}TERRAIN_INFO;

typedef struct ITEM_DATA
{
	unsigned int id;
	unsigned int amount;
}ITEM_DATA;


