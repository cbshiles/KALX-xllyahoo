// json.cpp - Parse JSON strings
/*
JSON.MAKE(keys, values) - same size arrays
JSON.ADD(object, keys, values)
JSON.REMOVE(object, keys)
JSON.REPLACE(object, keys, values)

JSON.INDEX(object, {k1;k2;...}) -> {v1;v2;...}
JSON.INDEX(object, {k1,k2,...}) -> v[k1][k2]...
*/
#include <regex>
#include "json.h"

using namespace xll;

static AddInX xai_json_parse(
	FunctionX(XLL_LPOPERX, _T("?xll_json_parse"), _T("JSON.PARSE"))
	.Arg(XLL_CSTRINGX, _T("String"), _T("is a JSON string"))
	.Category(CATEGORY_JSON)
	.FunctionHelp(_T("Parse a JSON string into key-value pairs."))
	.Documentation(_T(""))
);
LPOPERX WINAPI
xll_json_parse(xcstr object)
{
#pragma XLLEXPORT
	static OPERX xResult;

	try {
		ensure (object == object);
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());

		return 0;
	}

	return &xResult;
}

#ifdef _DEBUG

using namespace JSON;

void
test_json_parse()
{
	xcstr s = _T(" \t\n\rx");
	s = skip_white<XLOPERX>(s);
	ensure (*s == 'x');

	ensure (is_null<XLOPERX>(_T("null")));
	ensure (!is_null<XLOPERX>(_T("nUll")));
	ensure (!is_null<XLOPERX>(_T("foo")));

	// parse_string
	OPERX o;
	s = _T("\"Foo\": bar");
	o = parse_string<XLOPERX>(&s);
	ensure (o == _T("Foo"));
	ensure (*s == ':');

	s = _T("\"F\\\"oo\": bar");
	o = parse_string<XLOPERX>(&s);
	ensure (o == _T("F\\\"oo"));
	ensure (*s == ':');

	// parse_value
	s = _T("\"Foo\"");
	o = parse_value<XLOPERX>(&s);
	ensure (o == _T("Foo"));
	ensure (*s == 0);

	s = _T("\"Foo\" : 1.23");
	o = parse_value<XLOPERX>(&s);
	ensure (o == _T("Foo"));
	ensure (*s == ' ');
	ensure (*(s + 1) == ':');

	s = _T("1.23");
	o = OPERX(s);
	o = parse_value<XLOPERX>(&s);
	ensure (o == 1.23);

	s = _T("true");
	o = parse_value<XLOPERX>(&s);
	ensure (o == true);

	s = _T("false");
	o = parse_value<XLOPERX>(&s);
	ensure (o == false);

	// parse_elements
	s = _T("123, true, null, \"abc\"");
	o = parse_elements<XLOPERX>(&s);
	ensure (o.xltype == xltypeMulti);
	ensure (o.size() == 4);
	ensure (o[0] == 123);
	ensure (o[1] = true);
	ensure (o[2].xltype == xltypeErr);
	ensure (o[2].val.err == xlerrNull);
	ensure (o[3] == _T("abc"));
	ensure (!*s)

	s = _T("123, \"string with space\tand tab\" ,\t\r\nnull, true  ");
	o = parse_elements<XLOPERX>(&s);
	ensure (o.xltype == xltypeMulti);
	ensure (o.size() == 4);
	ensure (o[0] == 123);
	ensure (o[1] == _T("string with space\tand tab"));
	ensure (o[2] == ErrX(xlerrNull));
	ensure (o[3] == true);
	ensure (!*s)

	// parse_array
	s = _T("[	 123, true, null, \"abc\"	]");
	o = parse_array<XLOPERX>(&s);
	ensure (o.xltype == xltypeMulti);
	ensure (o.rows() == 1);
	ensure (o.columns() == 4);
	ensure (o[0] == 123);
	ensure (o[1] = true);
	ensure (o[2].xltype == xltypeErr);
	ensure (o[2].val.err == xlerrNull);
	ensure (o[3] == _T("abc"));
	ensure (!*s);

	// parse pair
	s = _T("\"key\":123");
	o = parse_pair<XLOPERX>(&s);
	ensure (o.size() == 2);
	ensure (o.rows() == 1);
	ensure (o.columns() == 2);
	ensure (o[0] == _T("key"));
	ensure (o[1] == 123);

	s = _T(" \t\n\r\"key\"\t\t:\r 123\r\t\n");
	o = parse_pair<XLOPERX>(&s);
	ensure (o.size() == 2);
	ensure (o.rows() == 1);
	ensure (o.columns() == 2);
	ensure (o[0] == _T("key"));
	ensure (o[1] == 123);

	s = _T("\"k1\" : \"v1\", \"k2\"\t:\ntrue, \"k3\":1.23 ");
	o = parse_members<XLOPERX>(&s);
	ensure (o.size() == 6);
	ensure (o.rows() == 3);
	ensure (o.columns() == 2);
	ensure (o(0,0) == _T("k1"));
	ensure (o(0,1) == _T("v1"));
	ensure (o(1,0) == _T("k2"));
	ensure (o(1,1) == true);
	ensure (o(2,0) == _T("k3"));
	ensure (o(2,1) == 1.23);

	s = _T("\"k1\" : \"v1\", \"k2\"\t:\n[\"fie\", 123, null], \"k3\":1.23 ");
	o = parse_members<XLOPERX>(&s);
	ensure (o.size() == 6);
	ensure (o.rows() == 3);
	ensure (o.columns() == 2);
	ensure (o(0,0) == _T("k1"));
	ensure (o(0,1) == _T("v1"));
	ensure (o(1,0) == _T("k2"));

	ensure (o(1,1).xltype == xltypeMulti);
	ensure (o(1,1).size() == 3);
	ensure (o(1,1).columns() == 3);
	ensure (o(1,1).rows() == 1);
	ensure (o(1,1)[0] == _T("fie"));
	ensure (o(1,1)[1] == 123);
	ensure (o(1,1)[2] == ErrX(xlerrNull));

	ensure (o(2,0) == _T("k3"));
	ensure (o(2,1) == 1.23);
/*
	s = _T("{\"menu\": {\"id\": \"file\",")
		_T("\"value\": \"File\",")
		_T("\"popup\": {\"menuitem\": [")
		_T("{\"value\": \"New\", \"onclick\": \"CreateNewDoc()\"},")
		_T("{\"value\": \"Open\", \"onclick\": \"OpenDoc()\"},")
		_T("{\"value\": \"Close\", \"onclick\": \"CloseDoc()\"}]")
		_T("}}");
*/
	s = _T("{\"id\": 1,\"name\": \"Foo\",   \"price\": 123,  \"tags\": [\"Bar\",\"Eek\"]}");
	o = parse_object<XLOPERX>(&s);
	ensure (o.xltype == xltypeMulti);
	ensure (o.rows() == 4);
	ensure (o.columns() == 2);
	ensure (o(3,0) == _T("tags"));
	ensure (o(3,1).xltype == xltypeMulti);
	ensure (o(3,1)[0] == _T("Bar"));
	ensure (o(3,1)[1] == _T("Eek"));
}

void
test_json(void)
{
	test_json_parse();
}

int
xll_test_json(void)
{
	try {
		test_json();
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());

		return 0;
	}

	return 1;
}
static Auto<OpenAfter> xai_test_json(xll_test_json);

#endif // _DEBUG