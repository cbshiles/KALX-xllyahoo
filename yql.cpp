// yql.cpp - YQL routines
#define EXCEL12
#include "xllyahoo.h"
#include "json.h"

using namespace xll;

#define YQL_SERVER L"query.yahooapis.com"
#define YQL_PREFIX _T("/v1/public/yql?")
#define YQL_FORMAT _T("&format=json")

static AddInX xai_yql_show_tables(
	FunctionX(XLL_LPOPERX, _T("?xll_yql_show_tables"), _T("YQL.SHOW.TABLES"))
	.Category(CATEGORY)
	.FunctionHelp(_T("Show availabel YQL tables."))
	.Documentation()
);
LPOPERX WINAPI
xll_yql_show_tables(void)
{
#pragma XLLEXPORT
	static OPERX xResult;

	try {
		xstring query(YQL_PREFIX);
		query.append(_T("q=SHOW TABLES"));
		query.append(YQL_FORMAT);
		std::string gd = GetData(YQL_SERVER, query);
#ifdef EXCEL12
		query = to_wcs(gd);
		xcstr str = query.c_str();
		xResult = JSON::parse_object<XLOPER12>(&str);
#else
		xcstr str = gd.c_str();
		xResult = JSON::parse_object<XLOPER>(&str);
#endif
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());

		return 0;
	}

	return &xResult;
}

static AddInX xai_yql_desc(
	FunctionX(XLL_LPOPERX, _T("?xll_yql_desc"), _T("YQL.DESC"))
	.Arg(XLL_CSTRINGX, _T("Table"), _T("is the table to be described."))
	.Category(CATEGORY)
	.FunctionHelp(_T("Describe a YQL table."))
	.Documentation()
);
LPOPERX WINAPI
xll_yql_desc(xcstr table)
{
#pragma XLLEXPORT
	static OPERX xResult;

	try {
		xstring query(YQL_PREFIX);
		query.append(_T("q=DESC "));
		query.append(table);
		query.append(YQL_FORMAT);
#ifdef EXCEL12
		xstring gd = to_wcs(GetData(YQL_SERVER, query));
		xcstr str = gd.c_str();
		xResult = JSON::parse_object<XLOPER12>(&str);
#else
#endif
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());

		return 0;
	}

	return &xResult;
}

static AddInX xai_yql_query(
	FunctionX(XLL_LPOPERX, _T("?xll_yql_query"), _T("YQL.QUERY"))
	.Arg(XLL_LPOPERX, _T("Select"), _T("is a range of fields to select."))
	.Arg(XLL_LPOPERX, _T("From"), _T("is the table to use for the query."))
	.Arg(XLL_LPOPERX, _T("Where"), _T("is the filter to apply to the query. "))
	.Arg(XLL_CSTRINGX, _T("Env?"), _T("is the environment to use for the query. "))
	.Category(CATEGORY)
	.FunctionHelp(_T("Execute a YQL query."))
	.Documentation(
		_T("If <codeInline>From</codeInline> and <codeInline>Where</codeInline> are missing ")
		_T("then the first argument is assumed to be a raw YQL query ")
	)
);
LPOPERX WINAPI
xll_yql_query(LPOPERX pselect, LPOPERX pfrom, LPOPERX pwhere, xcstr env)
{
#pragma XLLEXPORT
	static OPERX xResult;

	try {
		xstring query(YQL_PREFIX);

		if (pfrom->xltype == xltypeMissing && pwhere->xltype == xltypeMissing) {
			// raw query
			query.append(to_string<XLOPERX>(*pselect, _T(" "), _T(" ")));
		}
		else {
			query.append(_T("q=SELECT "));
			query.append(to_string<XLOPERX>(*pselect, _T(","), _T(",")));

			query.append(_T(" FROM "));
			query.append(to_string<XLOPERX>(*pfrom, _T(","), _T(",")));

			if (pwhere->xltype != xltypeMissing) {
				query.append(_T(" WHERE "));
				query.append(to_string<XLOPERX>(*pwhere, _T(" "), _T(" ")));
			}
		}

		query.append(YQL_FORMAT);

		if (env || *env) 
			query.append(_T("&env=http://")).append(env).append(_T(".env")); 

#ifdef EXCEL12
		std::string gd = GetData(YQL_SERVER, query);
		xstring Gd = to_wcs(gd);
		xcstr str = Gd.c_str();
		xResult = JSON::parse_object<XLOPERX>(&str);
#else
#endif
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());

		return 0;
	}

	return &xResult;
}