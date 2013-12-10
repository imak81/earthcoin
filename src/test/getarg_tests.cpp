#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-eac");
    BOOST_CHECK(GetBoolArg("-eac"));
    BOOST_CHECK(GetBoolArg("-eac", false));
    BOOST_CHECK(GetBoolArg("-eac", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-eaco"));
    BOOST_CHECK(!GetBoolArg("-eaco", false));
    BOOST_CHECK(GetBoolArg("-eaco", true));

    ResetArgs("-eac=0");
    BOOST_CHECK(!GetBoolArg("-eac"));
    BOOST_CHECK(!GetBoolArg("-eac", false));
    BOOST_CHECK(!GetBoolArg("-eac", true));

    ResetArgs("-eac=1");
    BOOST_CHECK(GetBoolArg("-eac"));
    BOOST_CHECK(GetBoolArg("-eac", false));
    BOOST_CHECK(GetBoolArg("-eac", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noeac");
    BOOST_CHECK(!GetBoolArg("-eac"));
    BOOST_CHECK(!GetBoolArg("-eac", false));
    BOOST_CHECK(!GetBoolArg("-eac", true));

    ResetArgs("-noeac=1");
    BOOST_CHECK(!GetBoolArg("-eac"));
    BOOST_CHECK(!GetBoolArg("-eac", false));
    BOOST_CHECK(!GetBoolArg("-eac", true));

    ResetArgs("-eac -noeac");  // -eac should win
    BOOST_CHECK(GetBoolArg("-eac"));
    BOOST_CHECK(GetBoolArg("-eac", false));
    BOOST_CHECK(GetBoolArg("-eac", true));

    ResetArgs("-eac=1 -noeac=1");  // -eac should win
    BOOST_CHECK(GetBoolArg("-eac"));
    BOOST_CHECK(GetBoolArg("-eac", false));
    BOOST_CHECK(GetBoolArg("-eac", true));

    ResetArgs("-eac=0 -noeac=0");  // -eac should win
    BOOST_CHECK(!GetBoolArg("-eac"));
    BOOST_CHECK(!GetBoolArg("-eac", false));
    BOOST_CHECK(!GetBoolArg("-eac", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--eac=1");
    BOOST_CHECK(GetBoolArg("-eac"));
    BOOST_CHECK(GetBoolArg("-eac", false));
    BOOST_CHECK(GetBoolArg("-eac", true));

    ResetArgs("--noeac=1");
    BOOST_CHECK(!GetBoolArg("-eac"));
    BOOST_CHECK(!GetBoolArg("-eac", false));
    BOOST_CHECK(!GetBoolArg("-eac", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-eac", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-eac", "eleven"), "eleven");

    ResetArgs("-eac -bar");
    BOOST_CHECK_EQUAL(GetArg("-eac", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-eac", "eleven"), "");

    ResetArgs("-eac=");
    BOOST_CHECK_EQUAL(GetArg("-eac", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-eac", "eleven"), "");

    ResetArgs("-eac=11");
    BOOST_CHECK_EQUAL(GetArg("-eac", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-eac", "eleven"), "11");

    ResetArgs("-eac=eleven");
    BOOST_CHECK_EQUAL(GetArg("-eac", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-eac", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-eac", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-eac", 0), 0);

    ResetArgs("-eac -bar");
    BOOST_CHECK_EQUAL(GetArg("-eac", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-eac=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-eac", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-eac=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-eac", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--eac");
    BOOST_CHECK_EQUAL(GetBoolArg("-eac"), true);

    ResetArgs("--eac=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-eac", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noeac");
    BOOST_CHECK(!GetBoolArg("-eac"));
    BOOST_CHECK(!GetBoolArg("-eac", true));
    BOOST_CHECK(!GetBoolArg("-eac", false));

    ResetArgs("-noeac=1");
    BOOST_CHECK(!GetBoolArg("-eac"));
    BOOST_CHECK(!GetBoolArg("-eac", true));
    BOOST_CHECK(!GetBoolArg("-eac", false));

    ResetArgs("-noeac=0");
    BOOST_CHECK(GetBoolArg("-eac"));
    BOOST_CHECK(GetBoolArg("-eac", true));
    BOOST_CHECK(GetBoolArg("-eac", false));

    ResetArgs("-eac --noeac");
    BOOST_CHECK(GetBoolArg("-eac"));

    ResetArgs("-noeac -eac"); // eac always wins:
    BOOST_CHECK(GetBoolArg("-eac"));
}

BOOST_AUTO_TEST_SUITE_END()
