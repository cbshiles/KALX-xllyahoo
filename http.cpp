// http.c - raw HTTP commands
//#include "xll/utility/winhttp.h"
#define EXCEL12
#include "xllyahoo.h"

#ifndef CATEGORY
#define CATEGORY _T("HTTP")
#endif

using namespace xll;

typedef traits<XLOPERX>::xcstr xcstr;

static AddInX xai_http_get(
	FunctionX(XLL_LPOPERX, _T("?xll_http_get"), _T("HTTP.GET"))
	.Arg(XLL_CSTRINGX, _T("Server"), _T("is the name of the host."))
	.Arg(XLL_CSTRINGX, _T("Request"), _T("is the HTTP GET request."))
	.Category(CATEGORY)
	.FunctionHelp(_T("Issue an HTTP GET request."))
);
LPOPERX WINAPI
xll_http_get(xcstr server, xcstr request)
{
#pragma XLLEXPORT
	static OPERX xResult;
	static std::basic_regex<xchar> RS(_T("(\r\n)+"));

	try {
#ifdef EXCEL12
		std::string gd = GetData(server, request);
		xResult = Eval(to_wcs(gd), RS);
#else
		std::string gd = GetData(server, to_wcs(query));
		xResult = Eval(gd, RS);
#endif
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());

		xResult = ErrX(xlerrValue);
	}
	
	return &xResult;
}
