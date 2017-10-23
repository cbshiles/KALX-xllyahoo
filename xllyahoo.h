// xllyahoo.h - Yahoo! finance quotes.
// Must define this for WinHttp functions.
//#define EXCEL12
#include <regex>
#include "xll/xll.h"
#include "xll/utility/winhttp.h"

#ifndef CATEGORY
#define CATEGORY _T("Yahoo!")
#endif

#define YAHOO_CLIENT L"XllYahoo/1.0"

typedef xll::traits<XLOPERX>::xchar xchar;
typedef xll::traits<XLOPERX>::xcstr xcstr;
typedef xll::traits<XLOPERX>::xfp xfp;
typedef xll::traits<XLOPERX>::xstring xstring;
typedef xll::traits<XLOPERX>::xword xword;


inline std::string
to_mbs(const std::basic_string<WCHAR>& xs)
{
	std::string s;

	s.resize(xs.size() + 1);
	wcstombs_s(0, &s[0], s.size(), &xs[0], xs.size());

	return s;
}
inline std::basic_string<WCHAR>
to_wcs(const std::string& s)
{
	std::basic_string<WCHAR> xs;

	xs.resize(s.size() + 1);
	mbstowcs_s(0, &xs[0], xs.size(), &s[0], s.size()); 

	return xs;
}

// convert Yahoo data to OPERX
inline OPERX
Eval(const xstring& s, const std::basic_regex<xchar>& RS)
{
	xstring data(s);
	// remove repeated CRLF
	static std::basic_regex<xchar> NA(_T("N/A"));

	data.insert(data.begin(), _T('{'));
	data = std::regex_replace(data, RS, xstring(_T(";")));
	data = std::regex_replace(data, NA, xstring(_T("#N/A")));
	data.at(data.find_last_of(_T(';'))) = _T('}');

	return ExcelX(xlfEvaluate, OPERX(&data[0]));
}

inline std::string
GetData(LPCWSTR server, const std::basic_string<WCHAR>& query)
{
		WinHttp::Open session(YAHOO_CLIENT);
		WinHttp::Connect connect(session, server);
		WinHttp::OpenRequest request(connect, L"GET", query.c_str());
		ensure (request.SendRequest());
		ensure (request.ReceiveResponse());
		request.QueryHeaders(); // toss these
		
		return request.ReadData();
}
