// json.h - JSON objects for Excel
/*
k1 | v1
k2 | {k3 | v3         -- nested objects
      k4 | v4}
k5 | [v6 | v7 | v8 ]  -- array is one row (what if 2 elements???)

Make generic.
template<type T>
struct json_traits {
	T push_back(T);
	T string(string, count?);
	T number(double);
	T object(?);
	T array(
	...
}
*/
//#define EXCEL12
#include "xll/xll.h"

#define CATEGORY_JSON _T("JSON")

typedef xll::traits<XLOPERX>::xcstr xcstr;

namespace JSON {

	template<class X>
	inline typename xll::traits<X>::xcstr eat(typename xll::traits<X>::xcstr str, int c)
	{
		str = skip_white<X>(str);
		
		ensure (*str++ == c);
		
		return str;
	}
	template<class X>
	inline bool peek(typename xll::traits<X>::xcstr* pstr, int c)
	{
		*pstr = skip_white<X>(*pstr);

		return **pstr == c ? ++*pstr, true : false;
	}

	template<class X>
	inline bool is_white(typename xll::traits<X>::xchar c)
	{
		return c == ' ' || c == '\t' || c == '\n' || c == '\r';
	}
	template<class X>
	inline typename xll::traits<X>::xcstr skip_white(typename xll::traits<X>::xcstr str)
	{
		while (is_white<X>(*str))
			++str;

		return str;
	}
	template<class X>
	inline bool is_null(typename xll::traits<X>::xcstr c)
	{
		return *c++ == 'n' && (*c && *c++ == 'u') && (*c && *c++ == 'l') && (*c && *c == 'l');
	}
	// increment past escaped char
	template<class X>
	inline typename xll::traits<X>::xcstr incr(typename xll::traits<X>::xcstr str)
	{
		return ++str;
	}
	template<class X>
	inline typename xll::traits<X>::xchar len(typename xll::traits<X>::xcstr b, typename xll::traits<X>::xcstr e)
	{
		return static_cast<xll::traits<X>::xchar>(e - b);
	}

	// string: " chars "
	template<class X>
	inline XOPER<X> parse_string(typename xll::traits<X>::xcstr* pstr)
	{
		OPERX o;
		xll::traits<X>::xcstr b, e;

		*pstr = skip_white<X>(*pstr);

		ensure (**pstr == '\"');
		b = *pstr + 1;
		for (e = b; *e != 0 && *e != '\"'; e = incr<X>(e))
			if (*e == '\\' && *(e + 1) == '\"')
				e = incr<X>(e);

		ensure (*e == '\"');

		*pstr = e + 1;

		o = XOPER<X>(b, len<X>(b, e));

		return o;
	}

	// value: string, number, object, array, true, false, null
	template<class X>
	inline XOPER<X> parse_value(typename xll::traits<X>::xcstr* pstr)
	{
		XOPER<X> o;
		xll::traits<X>::xcstr b, e;

		*pstr = skip_white<X>(*pstr);

		b = e = *pstr;
		if (*b == '{') {
			o = parse_object<X>(&e);
		}
		if (*b == '[') {
			o = parse_array<X>(&e);
		}
		else if (*b == '"') {
			o = parse_string<X>(&e);
		}
		else if (is_null<X>(b)) {
			e = b + 4;
			o = XErr<X>(xlerrNull);
		}
		else {
			for (e = b; *e != 0 && *e != ',' && !is_white<X>(*e); e = incr<X>(e)) {
				if (*e == '\\')
					++e; // skip escaped chars
			}

			o = Excel<X>(xlfEvaluate, XOPER<X>(b, len<X>(b, e)));
		}
		
		*pstr = e;

		return o;
	}
	// elements: value or value , elements
	template<class X>
	inline XOPER<X> parse_elements(typename xll::traits<X>::xcstr* pstr)
	{
		XOPER<X> o;

		*pstr = skip_white<X>(*pstr);

		o = parse_value<X>(pstr);
		while (peek<X>(pstr, ','))
			o.push_back(parse_value<X>(pstr));

		return o;
	}
	// array: [ ], [ elements ]
	template<class X>
	inline XOPER<X> parse_array(typename xll::traits<X>::xcstr* pstr)
	{
		XOPER<X> o;

		*pstr = eat<X>(*pstr, '[');
		
		o = parse_elements<X>(pstr);
		o.resize(1, o.size());

		*pstr = eat<X>(*pstr, ']');

		return o;
	}
	// pair: string : value
	template<class X>
	inline XOPER<X> parse_pair(typename xll::traits<X>::xcstr* pstr)
	{
		XOPER<X> o(1, 2);

		o[0] = parse_string<X>(pstr);
		*pstr = eat<X>(*pstr, ':');
		o[1] = parse_value<X>(pstr);

		return o;
	}
	// members: pair or pair , members
	template<class X>
	inline XOPER<X> parse_members(typename xll::traits<X>::xcstr* pstr)
	{
		XOPER<X> o;

		o.push_back(parse_pair<X>(pstr));
		while (peek<X>(pstr, ','))
			o.push_back(parse_pair<X>(pstr));

		return o;
	}
	// object: { }, { members }
	template<class X>
	inline XOPER<X> parse_object(typename xll::traits<X>::xcstr* pstr)
	{
		XOPER<X> o;

		*pstr = eat<X>(*pstr, '{');
		o = parse_members<X>(pstr);
		*pstr = eat<X>(*pstr, '}');

		return o;
	}

	// stringify
	template<class X>
	inline typename xll::traits<X>::xstring to_elements(const XOPER<X>& e)
	{
		xll::traits<X>::xstring str;

		str.append(to_value(e[0]));
		for (xword i = 1, i < e.size(); ++i) {
			str.append(',').append(to_value(e[i]));
		}
	}

	template<class X>
	inline typename xll::traits<X>::xstring to_value(const XOPER<X>& v)
	{
		xll::traits<X>::xstring str;

		if (v == ErrX(xlerrNull)) {
			str = _T("null");
		}
		else if (v.size() == 1 && v.!(xltype & xltypeMulti)) {
			if (v.xltype == xltypeStr) {
				str.append('\"').append(v.val.str + 1, v.val.str[0]).str.append('\"');
			}
			else if (v.xltype == xltypeBool) {
				str = v.val.xbool ? _T("true") : _T("false");
			}
			else if (v.xltype == xltypeNum) {
				str = to_string(v);
			}
			else if (v.rows() == 1 && (v.xltype&xlbitArray)) {
				str.append('[').append.to_elements(v).append(']');
			}
			else {
				str.append.('{').append(to_object(v)).append('}');
			}
		}
		else { // object or array
			if (v.rows() == 1 && (v.xltype&xlbitArray)) {
				str.append('[');
				str.append(to_elements(v));
				str.append(']');
			}
			else {
				str.append(to_object(v));
			}
		}

		return str;
	}

		
	template<class X>
	inline typename xll::traits<X>::xstring to_pair(const XOPER<X>& k, const XOPER<X>& v)
	{
		xll::traits<X>::xstring str;

		ensure k.xltype == xltypeStr;
		str.append('"').append(k.val.str + 1, k.val.str[0]).append('"').append(':').append(to_value(v));

		return str;
	}

	template<class X>
	inline typename xll::traits<X>::xstring to_member(const XOPER<X>& o)
	{
		xll::traits<X>::xstring str;

		if (o.size() == 0)
			return str(_T(""));

		ensure (o.columns() == 2);

		str.append(to_pair(o[0], o[1]));
		for (xword i = 1; i < rows(); ++i) {
			str.append(',').to_pair(o(i, 0), o(i, 1));
		}

		return str;
	}
	template<class X>
	inline typename xll::traits<X>::xstring to_object(const XOPER<X>& o)
	{
		xll::traits<X>::xstring str;

		str.append('{').append(to_members(o)).str.append('}');

		return str;
	}
	// JSON OPERX to string
	template<class X>
	inline typename xll::traits<X>::xstring stringify(const XOPER<X>& o)
	{
		return to_object(o);
	}

	template<class X>
	struct Value : public XOPER<X> {
		bool is_string(void) const
		{
			return size() == 1 && xltype == xltypeStr;
		}
		bool is_number(void) const
		{
			return size() == 1 && xltype == xltypeNum;
		}
		bool is_object(void) const
		{
			return columns() == 2 && operator[](0).xltype == xltypeStr; // might be an array
		}
		bool is_array(void) const
		{
			return rows() == 1 && columns() > 1;
		}
		bool is_true(void) const
		{
			return xltype == xltypeBool && val.xbool;
		}
		bool is_false(void) const
		{
			return xltype == xltypeBool && !val.bool;
		}
		bool is_null(void) const
		{
			return xltype == xltypeErr && val.err == xlerrNull;
		}
	};
	template<class X>
	class Array {
		XOPER<X> a_;
	public:
		Array(const Value& v)
		{
			a_.resize(1,1);
			a_[0] = v;
		}
	};
	template<class X>
	class Object {
		XOPER<X> o_;
	public:
		typedef typename xll::traits<X>::xcstr xcstr;
		typedef typename xll::traits<X>::xword xword;
		typedef typename xll::traits<X>::xstring xstring;
		Object(xcstr str)
		{
			o_ = JSON::parse_object(&str);
		}
		~Object()
		{ }

		xstring stringify(void) const
		{
			return JSON::stringify(o_);
		}
	};

} // namespace JSON