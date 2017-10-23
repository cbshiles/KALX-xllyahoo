// xllyahoo.cpp - Yahoo! financial data
#include "xllyahoo.h"

using namespace xll;

#define STR(s) s.val.str + 1, s.val.str[0]
//http://finance.yahoo.com/q/os
#define YAHOO_QUOTE_SERVER L"download.finance.yahoo.com"
#define YAHOO_QUOTE_SUFFIX _T("/d/quotes.csv?")

#define YAHOO_HISTORICAL_SERVER L"ichart.yahoo.com"
#define YAHOO_HISTORICAL_SUFFIX _T("/table.csv?")

static AddInX xai_yahoo(
	DocumentX(CATEGORY)
	.Documentation(
		_T("Various functions for accessing data from Yahoo! Finance. ")
		_T("Use the functions in the <codeInline>Yahoo! Fields</codeInline> enumerations ")
		_T("to specify long names for the Yahoo! field codes. ")
	)
);

#define CATEGORY_ENUM _T("Yahoo! Fields")

XLL_ENUM(_T("c8"), Yahoo_AfterHoursChangeRealtime, CATEGORY_ENUM, _T("After Hours Change (Realtime)"));
XLL_ENUM(_T("g3"), Yahoo_AnnualizedGain, CATEGORY_ENUM, _T("Annualized Gain"));
XLL_ENUM(_T("a0"), Yahoo_Ask, CATEGORY_ENUM, _T("Ask"));
XLL_ENUM(_T("b2"), Yahoo_AskRealtime, CATEGORY_ENUM, _T("Ask (Realtime)"));
XLL_ENUM(_T("a5"), Yahoo_AskSize, CATEGORY_ENUM, _T("Ask Size"));
XLL_ENUM(_T("a2"), Yahoo_AverageDailyVolume, CATEGORY_ENUM, _T("Average Daily Volume"));
XLL_ENUM(_T("b0"), Yahoo_Bid, CATEGORY_ENUM, _T("Bid"));
XLL_ENUM(_T("b3"), Yahoo_BidRealtime, CATEGORY_ENUM, _T("Bid (Realtime)"));
XLL_ENUM(_T("b6"), Yahoo_BidSize, CATEGORY_ENUM, _T("Bid Size"));
XLL_ENUM(_T("b4"), Yahoo_BookValuePerShare, CATEGORY_ENUM, _T("Book Value Per Share"));
XLL_ENUM(_T("c1"), Yahoo_Change, CATEGORY_ENUM, _T("Change"));
XLL_ENUM(_T("c0"), Yahoo_Change_ChangeInPercent, CATEGORY_ENUM, _T("Change Change In Percent "));
XLL_ENUM(_T("m7"), Yahoo_ChangeFromFiftydayMovingAverage, CATEGORY_ENUM, _T("Change From Fiftyday Moving Average"));
XLL_ENUM(_T("m5"), Yahoo_ChangeFromTwoHundreddayMovingAverage, CATEGORY_ENUM, _T("Change From Two Hundredday Moving Average"));
XLL_ENUM(_T("k4"), Yahoo_ChangeFromYearHigh, CATEGORY_ENUM, _T("Change From Year High"));
XLL_ENUM(_T("j5"), Yahoo_ChangeFromYearLow, CATEGORY_ENUM, _T("Change From Year Low"));
XLL_ENUM(_T("p2"), Yahoo_ChangeInPercent, CATEGORY_ENUM, _T("Change In Percent"));
XLL_ENUM(_T("k2"), Yahoo_ChangeInPercentRealtime, CATEGORY_ENUM, _T("Change In Percent (Realtime)"));
XLL_ENUM(_T("c6"), Yahoo_ChangeRealtime, CATEGORY_ENUM, _T("Change (Realtime)"));
XLL_ENUM(_T("c3"), Yahoo_Commission, CATEGORY_ENUM, _T("Commission"));
XLL_ENUM(_T("c4"), Yahoo_Currency, CATEGORY_ENUM, _T("Currency"));
XLL_ENUM(_T("h0"), Yahoo_DaysHigh, CATEGORY_ENUM, _T("Days High"));
XLL_ENUM(_T("g0"), Yahoo_DaysLow, CATEGORY_ENUM, _T("Days Low"));
XLL_ENUM(_T("m0"), Yahoo_DaysRange, CATEGORY_ENUM, _T("Days Range"));
XLL_ENUM(_T("m2"), Yahoo_DaysRangeRealtime, CATEGORY_ENUM, _T("Days Range (Realtime)"));
XLL_ENUM(_T("w1"), Yahoo_DaysValueChange, CATEGORY_ENUM, _T("Days Value Change"));
XLL_ENUM(_T("w4"), Yahoo_DaysValueChangeRealtime, CATEGORY_ENUM, _T("Days Value Change (Realtime)"));
XLL_ENUM(_T("r1"), Yahoo_DividendPayDate, CATEGORY_ENUM, _T("Dividend Pay Date"));
XLL_ENUM(_T("d0"), Yahoo_TrailingAnnualDividendYield, CATEGORY_ENUM, _T("Trailing Annual Dividend Yield"));
XLL_ENUM(_T("y0"), Yahoo_TrailingAnnualDividendYieldInPercent, CATEGORY_ENUM, _T("Trailing Annual Dividend Yield In Percent"));
XLL_ENUM(_T("e0"), Yahoo_DilutedEPS, CATEGORY_ENUM, _T("Diluted E P S"));
XLL_ENUM(_T("j4"), Yahoo_EBITDA, CATEGORY_ENUM, _T("E B I T D A"));
XLL_ENUM(_T("e7"), Yahoo_EPSEstimateCurrentYear, CATEGORY_ENUM, _T("E P S Estimate Current Year"));
XLL_ENUM(_T("e9"), Yahoo_EPSEstimateNextQuarter, CATEGORY_ENUM, _T("E P S Estimate Next Quarter"));
XLL_ENUM(_T("e8"), Yahoo_EPSEstimateNextYear, CATEGORY_ENUM, _T("E P S Estimate Next Year"));
XLL_ENUM(_T("q0"), Yahoo_ExDividendDate, CATEGORY_ENUM, _T("Ex Dividend Date"));
XLL_ENUM(_T("m3"), Yahoo_FiftydayMovingAverage, CATEGORY_ENUM, _T("Fiftyday Moving Average"));
XLL_ENUM(_T("f6"), Yahoo_SharesFloat, CATEGORY_ENUM, _T("Shares Float"));
XLL_ENUM(_T("l2"), Yahoo_HighLimit, CATEGORY_ENUM, _T("High Limit"));
XLL_ENUM(_T("g4"), Yahoo_HoldingsGain, CATEGORY_ENUM, _T("Holdings Gain"));
XLL_ENUM(_T("g1"), Yahoo_HoldingsGainPercent, CATEGORY_ENUM, _T("Holdings Gain Percent"));
XLL_ENUM(_T("g5"), Yahoo_HoldingsGainPercentRealtime, CATEGORY_ENUM, _T("Holdings Gain Percent (Realtime)"));
XLL_ENUM(_T("g6"), Yahoo_HoldingsGainRealtime, CATEGORY_ENUM, _T("Holdings Gain (Realtime)"));
XLL_ENUM(_T("v1"), Yahoo_HoldingsValue, CATEGORY_ENUM, _T("Holdings Value"));
XLL_ENUM(_T("v7"), Yahoo_HoldingsValueRealtime, CATEGORY_ENUM, _T("Holdings Value (Realtime)"));
XLL_ENUM(_T("d1"), Yahoo_LastTradeDate, CATEGORY_ENUM, _T("Last Trade Date"));
XLL_ENUM(_T("l1"), Yahoo_LastTradePriceOnly, CATEGORY_ENUM, _T("Last Trade Price Only"));
XLL_ENUM(_T("k1"), Yahoo_LastTradeRealtimeWithTime, CATEGORY_ENUM, _T("Last Trade (Realtime) With Time"));
XLL_ENUM(_T("k3"), Yahoo_LastTradeSize, CATEGORY_ENUM, _T("Last Trade Size"));
XLL_ENUM(_T("t1"), Yahoo_LastTradeTime, CATEGORY_ENUM, _T("Last Trade Time"));
XLL_ENUM(_T("l0"), Yahoo_LastTradeWithTime, CATEGORY_ENUM, _T("Last Trade With Time"));
XLL_ENUM(_T("l3"), Yahoo_LowLimit, CATEGORY_ENUM, _T("Low Limit"));
XLL_ENUM(_T("j1"), Yahoo_MarketCapitalization, CATEGORY_ENUM, _T("Market Capitalization"));
XLL_ENUM(_T("j3"), Yahoo_MarketCapRealtime, CATEGORY_ENUM, _T("Market Cap (Realtime)"));
XLL_ENUM(_T("i0"), Yahoo_MoreInfo, CATEGORY_ENUM, _T("More Info"));
XLL_ENUM(_T("n0"), Yahoo_Name, CATEGORY_ENUM, _T("Name"));
XLL_ENUM(_T("n4"), Yahoo_Notes, CATEGORY_ENUM, _T("Notes"));
XLL_ENUM(_T("t8"), Yahoo_OneyrTargetPrice, CATEGORY_ENUM, _T("Oneyr Target Price"));
XLL_ENUM(_T("o0"), Yahoo_Open, CATEGORY_ENUM, _T("Open"));
XLL_ENUM(_T("i5"), Yahoo_OrderBookRealtime, CATEGORY_ENUM, _T("Order Book (Realtime)"));
XLL_ENUM(_T("r5"), Yahoo_PEGRatio, CATEGORY_ENUM, _T("P E G Ratio"));
XLL_ENUM(_T("r0"), Yahoo_PERatio, CATEGORY_ENUM, _T("P E Ratio"));
XLL_ENUM(_T("r2"), Yahoo_PERatioRealtime, CATEGORY_ENUM, _T("P E Ratio (Realtime)"));
XLL_ENUM(_T("m8"), Yahoo_PercentChangeFromFiftydayMovingAverage, CATEGORY_ENUM, _T("Percent Change From Fiftyday Moving Average"));
XLL_ENUM(_T("m6"), Yahoo_PercentChangeFromTwoHundreddayMovingAverage, CATEGORY_ENUM, _T("Percent Change From Two Hundredday Moving Average"));
XLL_ENUM(_T("k5"), Yahoo_ChangeInPercentFromYearHigh, CATEGORY_ENUM, _T("Change In Percent From Year High"));
XLL_ENUM(_T("j6"), Yahoo_PercentChangeFromYearLow, CATEGORY_ENUM, _T("Percent Change From Year Low"));
XLL_ENUM(_T("p0"), Yahoo_PreviousClose, CATEGORY_ENUM, _T("Previous Close"));
XLL_ENUM(_T("p6"), Yahoo_PriceBook, CATEGORY_ENUM, _T("Price Book"));
XLL_ENUM(_T("r6"), Yahoo_PriceEPSEstimateCurrentYear, CATEGORY_ENUM, _T("Price E P S Estimate Current Year"));
XLL_ENUM(_T("r7"), Yahoo_PriceEPSEstimateNextYear, CATEGORY_ENUM, _T("Price E P S Estimate Next Year"));
XLL_ENUM(_T("p1"), Yahoo_PricePaid, CATEGORY_ENUM, _T("Price Paid"));
XLL_ENUM(_T("p5"), Yahoo_PriceSales, CATEGORY_ENUM, _T("Price Sales"));
XLL_ENUM(_T("s6"), Yahoo_Revenue, CATEGORY_ENUM, _T("Revenue"));
XLL_ENUM(_T("s1"), Yahoo_SharesOwned, CATEGORY_ENUM, _T("Shares Owned"));
XLL_ENUM(_T("j2"), Yahoo_SharesOutstanding, CATEGORY_ENUM, _T("Shares Outstanding"));
XLL_ENUM(_T("s7"), Yahoo_ShortRatio, CATEGORY_ENUM, _T("Short Ratio"));
XLL_ENUM(_T("x0"), Yahoo_StockExchange, CATEGORY_ENUM, _T("Stock Exchange"));
XLL_ENUM(_T("s0"), Yahoo_Symbol, CATEGORY_ENUM, _T("Symbol"));
XLL_ENUM(_T("t7"), Yahoo_TickerTrend, CATEGORY_ENUM, _T("Ticker Trend"));
XLL_ENUM(_T("d2"), Yahoo_TradeDate, CATEGORY_ENUM, _T("Trade Date"));
XLL_ENUM(_T("t6"), Yahoo_TradeLinks, CATEGORY_ENUM, _T("Trade Links"));
XLL_ENUM(_T("f0"), Yahoo_TradeLinksAdditional, CATEGORY_ENUM, _T("Trade Links Additional"));
XLL_ENUM(_T("m4"), Yahoo_TwoHundreddayMovingAverage, CATEGORY_ENUM, _T("Two Hundredday Moving Average"));
XLL_ENUM(_T("v0"), Yahoo_Volume, CATEGORY_ENUM, _T("Volume"));
XLL_ENUM(_T("k0"), Yahoo_YearHigh, CATEGORY_ENUM, _T("Year High"));
XLL_ENUM(_T("j0"), Yahoo_YearLow, CATEGORY_ENUM, _T("Year Low"));
XLL_ENUM(_T("w0"), Yahoo_YearRange, CATEGORY_ENUM, _T("Year Range"));


static AddInX xai_yahoo_quote(
	FunctionX(XLL_LPOPERX, _T("?xll_yahoo_quote"), _T("YAHOO.QUOTE"))
	.Arg(XLL_LPOPERX, _T("Tickers"), _T("is a range of instrument tickers."))
	.Arg(XLL_LPOPERX, _T("Fields"), _T("is a range containing the fields to return. "))
	.Category(CATEGORY)
	.FunctionHelp(_T("Retrieve market quotes from Yahoo! Finance."))
	.Documentation(
		_T("Tickers is an array of symbols used by Yahoo! for financial instruments. ")
		_T("Fields are the codes used by Yahoo! for indicating what type of data to return. ")
		_T("The functions in the <codeInline>Yahoo! Fields</codeInline> enumerations allow you ")
		_T("to specify long names for these fields. ")
	)
);
LPOPERX WINAPI
xll_yahoo_quote(LPOPERX pTicker, LPOPERX pField)
{
#pragma XLLEXPORT
	static OPERX xResult;
	static std::basic_regex<xchar> RS(_T("(\r\n)+"));

	try {
		const OPERX& t(*pTicker);
		const OPERX& f(*pField);
		xstring query(YAHOO_QUOTE_SUFFIX);

		query.append(_T("s="));
		query.append(to_string<XLOPERX>(t, _T(","), _T(",")));

		if (pField->xltype != xltypeMissing) {
			query.append(_T("&f="));
			query.append(to_string<XLOPERX>(f));
		}
		query.append(_T("&e=.csv"));

#ifdef EXCEL12
		std::string gd = GetData(YAHOO_QUOTE_SERVER, query);
		xResult = Eval(to_wcs(gd), RS);
#else
		std::string gd = GetData(YAHOO_QUOTE_SERVER, to_wcs(query));
		xResult = Eval(gd, RS);
#endif
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());

		xResult = ErrX(xlerrValue);
	}
	
	return &xResult;
}

inline OPERX
parse_historical(xcstr data, size_t len, BOOL headers)
{
	OPERX x;
	size_t b(0), e(0);
	xstring xs(data, len);

	if (headers) {
		x.resize(1, 7);
		for (xword i = 0; i < 7; ++i) {
			e = xs.find_first_of(i == 6 ? _T('\n') : _T(','), b);
			xchar n = static_cast<xchar>(e - b);
			x[i] = OPERX(&xs[b], n);
			b = e + 1;
		}
	}
	else {
		b = xs.find_first_of(_T('\n'));
		++b;
		e = b;
	}
	OPERX r(1, 7);
	while (b < len) {
		// Date followed by six numbers.
		for (xword i = 0; i < 7; ++i) {
				e = xs.find_first_of(i == 6 ? _T('\n') : _T(','), b);
				xchar n = static_cast<xchar>(e - b);
				r[i] = ExcelX(xlfValue, OPERX(&xs[b], n));
				b = e + 1;
		}
		x.push_back(r);
	}

	return x;
}

static AddInX xai_yahoo_historical(
	FunctionX(XLL_LPOPERX, _T("?xll_yahoo_historical"), _T("YAHOO.HISTORICAL"))
	.Arg(XLL_CSTRINGX, _T("Ticker"), _T("is the instrument ticker."))
	.Arg(XLL_DOUBLEX, _T("From"), _T("is first date for which data are returned. "))
	.Arg(XLL_DOUBLEX, _T("To"), _T("is last date for which data are returned. "))
	.Arg(XLL_CSTRINGX, _T("Frequency?"), _T("is \"d\", \"w\", or \"m\" with daily by default."))
	.Arg(XLL_BOOLX, _T("Headers?"), _T("is a boolean indicating whether on not headers should be returned."))
	.Category(CATEGORY)
	.FunctionHelp(_T("Retrieve historical Date, Open, High, Low, Close, Volume, and Adjusted Close from Yahoo! Finance."))
	.Documentation(
		_T("Tickers is an array of symbols used by Yahoo! for financial instruments. ")
		_T("Fields are the codes used by Yahoo! for indicating what type of data to return. ")
		_T("The functions in the <codeInline>Yahoo! Fields</codeInline> enumerations allow you ")
		_T("to specify long names for these fields. ")
	)
);
LPOPERX WINAPI
xll_yahoo_historical(xcstr ticker, double from, double to, xcstr freq, BOOL headers)
{
#pragma XLLEXPORT
	static OPERX xResult;
	static std::basic_regex<xchar> RS(_T("\n"));

	try {
		ensure (from < to);
		if (!*freq)
			freq = _T("d");

		xstring query(YAHOO_HISTORICAL_SUFFIX);
		query.append(_T("s="));
		query.append(ticker);

		OPERX hash(_T("#"));
		OPERX d(from);

		query.append(_T("&a="));
		OPERX x = ExcelX(xlfMonth, d);
		x = x - 1;
		x = ExcelX(xlfText, x, hash);
		query.append(STR(x));

		query.append(_T("&b="));
		x = ExcelX(xlfText, ExcelX(xlfDay, d), hash);
		query.append(STR(x));

		d = to;

		query.append(_T("&c="));
		x = ExcelX(xlfText, ExcelX(xlfYear, d), hash);
		query.append(STR(x));

		query.append(_T("&d="));
		x = ExcelX(xlfMonth, d);
		x = x - 1;
		x = ExcelX(xlfText, x, hash);
		query.append(STR(x));

		query.append(_T("&e="));
		x = ExcelX(xlfText, ExcelX(xlfDay, d), hash);
		query.append(STR(x));

		query.append(_T("&f="));
		x = ExcelX(xlfText, ExcelX(xlfYear, d), hash);
		query.append(STR(x));

		query.append(_T("&g="));
		query.append(freq);

		query.append(_T("&ignore=.csv"));

		std::string gd;
#ifdef EXCEL12
		gd = GetData(YAHOO_HISTORICAL_SERVER, query);
		xResult = parse_historical(to_wcs(gd).c_str(), gd.length(), headers);
#else
		gd = GetData(YAHOO_HISTORICAL_SERVER, to_wcs(query));
		xResult = parse_historical(gd.c_str(), gd.length(), headers);
#endif
	}
	catch (const std::exception& ex) {
		XLL_ERROR(ex.what());

		xResult = ErrX(xlerrValue);
	}
	
	return &xResult;
}

