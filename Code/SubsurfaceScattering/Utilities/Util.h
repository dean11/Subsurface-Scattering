#ifndef BACHELOR_UTIL_H
#define BACHELOR_UTIL_H

#include <string>

namespace Util
{
	static ::std::wstring & StringToWstring(const ::std::string &str, ::std::wstring &wstr)
	{
		const char *orig = str.c_str();

		// Convert to a wchar_t*
		size_t origsize = strlen(orig) + 1;
		const size_t newsize = 255;
		size_t convertedChars = 0;
		wchar_t wcstring[newsize];
		mbstowcs_s(&convertedChars, wcstring, origsize, orig, _TRUNCATE);
		wstr = wcstring;
		//wcscat_s(wcstring, L" (wchar_t *)");
		return wstr;
	}
	static ::std::string & WStringToString(const ::std::wstring &wstr, ::std::string &str)
	{
		const wchar_t* orig = wstr.c_str();
		// Convert to a char*
		size_t origsize = wcslen(orig) + 1;
		const size_t newsize = 255;
		size_t convertedChars = 0;
		char nstring[newsize];
		wcstombs_s(&convertedChars, nstring, origsize, orig, _TRUNCATE);
		str = nstring;
		//strcat_s(nstring, " (char *)");
		return str;
	}
}

#endif // !BACHELOR_UTIL_H
