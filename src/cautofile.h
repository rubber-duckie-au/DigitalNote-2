#ifndef CAUTOFILE_H
#define CAUTOFILE_H

#include <cstdio>

/** RAII wrapper for FILE*.
 *
 * Will automatically close the file when it goes out of scope if not null.
 * If you're returning the file pointer, return file.release().
 * If you need to close the file early, use file.fclose() instead of fclose(file).
 */
class CAutoFile
{
protected:
	FILE* file;
	short state;
	short exceptmask;

public:
	int nType;
	int nVersion;

	CAutoFile(FILE* filenew, int nTypeIn, int nVersionIn);
	~CAutoFile();
	
	void fclose();
	FILE* release();
	operator FILE*();
	FILE* operator->();
	FILE& operator*();
	FILE** operator&();
	FILE* operator=(FILE* pnew);
	bool operator!();
	
	//
	// Stream subset
	//
	void setstate(short bits, const char* psz);
	bool fail() const;
	bool good() const;
	void clear(short n = 0);
	short exceptions();
	short exceptions(short mask);
	void SetType(int n);
	int GetType();
	void SetVersion(int n);
	int GetVersion();
	void ReadVersion();
	void WriteVersion();
	CAutoFile& read(char* pch, size_t nSize);
	CAutoFile& write(const char* pch, size_t nSize);

	template<typename T>
	unsigned int GetSerializeSize(const T& obj);
	
	template<typename T>
	CAutoFile& operator<<(const T& obj);
	
	template<typename T>
	CAutoFile& operator>>(T& obj);
};

#endif // CAUTOFILE_H
