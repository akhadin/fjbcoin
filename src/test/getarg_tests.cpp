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
    ResetArgs("-FJB");
    BOOST_CHECK(GetBoolArg("-FJB"));
    BOOST_CHECK(GetBoolArg("-FJB", false));
    BOOST_CHECK(GetBoolArg("-FJB", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-FJBo"));
    BOOST_CHECK(!GetBoolArg("-FJBo", false));
    BOOST_CHECK(GetBoolArg("-FJBo", true));

    ResetArgs("-FJB=0");
    BOOST_CHECK(!GetBoolArg("-FJB"));
    BOOST_CHECK(!GetBoolArg("-FJB", false));
    BOOST_CHECK(!GetBoolArg("-FJB", true));

    ResetArgs("-FJB=1");
    BOOST_CHECK(GetBoolArg("-FJB"));
    BOOST_CHECK(GetBoolArg("-FJB", false));
    BOOST_CHECK(GetBoolArg("-FJB", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noFJB");
    BOOST_CHECK(!GetBoolArg("-FJB"));
    BOOST_CHECK(!GetBoolArg("-FJB", false));
    BOOST_CHECK(!GetBoolArg("-FJB", true));

    ResetArgs("-noFJB=1");
    BOOST_CHECK(!GetBoolArg("-FJB"));
    BOOST_CHECK(!GetBoolArg("-FJB", false));
    BOOST_CHECK(!GetBoolArg("-FJB", true));

    ResetArgs("-FJB -noFJB");  // -FJB should win
    BOOST_CHECK(GetBoolArg("-FJB"));
    BOOST_CHECK(GetBoolArg("-FJB", false));
    BOOST_CHECK(GetBoolArg("-FJB", true));

    ResetArgs("-FJB=1 -noFJB=1");  // -FJB should win
    BOOST_CHECK(GetBoolArg("-FJB"));
    BOOST_CHECK(GetBoolArg("-FJB", false));
    BOOST_CHECK(GetBoolArg("-FJB", true));

    ResetArgs("-FJB=0 -noFJB=0");  // -FJB should win
    BOOST_CHECK(!GetBoolArg("-FJB"));
    BOOST_CHECK(!GetBoolArg("-FJB", false));
    BOOST_CHECK(!GetBoolArg("-FJB", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--FJB=1");
    BOOST_CHECK(GetBoolArg("-FJB"));
    BOOST_CHECK(GetBoolArg("-FJB", false));
    BOOST_CHECK(GetBoolArg("-FJB", true));

    ResetArgs("--noFJB=1");
    BOOST_CHECK(!GetBoolArg("-FJB"));
    BOOST_CHECK(!GetBoolArg("-FJB", false));
    BOOST_CHECK(!GetBoolArg("-FJB", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-FJB", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-FJB", "eleven"), "eleven");

    ResetArgs("-FJB -bar");
    BOOST_CHECK_EQUAL(GetArg("-FJB", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-FJB", "eleven"), "");

    ResetArgs("-FJB=");
    BOOST_CHECK_EQUAL(GetArg("-FJB", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-FJB", "eleven"), "");

    ResetArgs("-FJB=11");
    BOOST_CHECK_EQUAL(GetArg("-FJB", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-FJB", "eleven"), "11");

    ResetArgs("-FJB=eleven");
    BOOST_CHECK_EQUAL(GetArg("-FJB", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-FJB", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-FJB", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-FJB", 0), 0);

    ResetArgs("-FJB -bar");
    BOOST_CHECK_EQUAL(GetArg("-FJB", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-FJB=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-FJB", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-FJB=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-FJB", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--FJB");
    BOOST_CHECK_EQUAL(GetBoolArg("-FJB"), true);

    ResetArgs("--FJB=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-FJB", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noFJB");
    BOOST_CHECK(!GetBoolArg("-FJB"));
    BOOST_CHECK(!GetBoolArg("-FJB", true));
    BOOST_CHECK(!GetBoolArg("-FJB", false));

    ResetArgs("-noFJB=1");
    BOOST_CHECK(!GetBoolArg("-FJB"));
    BOOST_CHECK(!GetBoolArg("-FJB", true));
    BOOST_CHECK(!GetBoolArg("-FJB", false));

    ResetArgs("-noFJB=0");
    BOOST_CHECK(GetBoolArg("-FJB"));
    BOOST_CHECK(GetBoolArg("-FJB", true));
    BOOST_CHECK(GetBoolArg("-FJB", false));

    ResetArgs("-FJB --noFJB");
    BOOST_CHECK(GetBoolArg("-FJB"));

    ResetArgs("-noFJB -FJB"); // FJB always wins:
    BOOST_CHECK(GetBoolArg("-FJB"));
}

BOOST_AUTO_TEST_SUITE_END()
