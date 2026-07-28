#include <boost/test/unit_test.hpp>

using namespace std;

// v2.0.0.9 (TODO 12.A.2): rewritten from mruset<int> to mruset<CAddress>.
//
// WHY.  mruset keeps its template body in mruset.cpp with an explicit
// instantiation list, and that list names only the two types the wallet
// actually uses: CAddress and CInv (net/cnode.h -- setAddrKnown and
// setInventoryKnown, the per-peer relay de-duplication caches).  mruset<int>
// is not instantiated, so the original test could not link.  Adding an
// instantiation to shipping code purely to satisfy a test was rejected.
//
// CAddress is the right substitute and CInv is not: this test's assertions all
// rest on comparing an mruset against a std::set, which needs BOTH operator<
// (for std::set ordering) and operator== (for the comparison).  CAddress
// inherits both from CService/CNetAddr.  CInv has operator< but NO operator==.
//
// Testing CAddress also means testing a type the wallet genuinely stores in an
// mruset, rather than a stand-in it never uses.
#include "mruset.h"
#include "util.h"
#include "caddress.h"
#include "net/cservice.h"
#include "net/cnetaddr.h"
#include "compat.h"

#define NUM_TESTS 16
#define MAX_SIZE 100

// Map an integer key onto a distinct CAddress.  Addresses are ordered by
// CNetAddr then port (cservice.cpp:131-134), so distinct n gives distinct,
// well-ordered addresses.  10.x.y.z keeps them inside RFC1918 space so nothing
// here resembles a routable host.
static CAddress addr(int n)
{
    struct in_addr ip;
    memset(&ip, 0, sizeof(ip));
    ip.s_addr = htonl(0x0A000000u | (static_cast<unsigned int>(n) & 0x00FFFFFFu));

    return CAddress(CService(CNetAddr(ip), static_cast<unsigned short>(18092)));
}

class mrutester
{
private:
    mruset<CAddress> mru;
    std::set<CAddress> set;

public:
    mrutester() { mru.max_size(MAX_SIZE); }
    int size() const { return set.size(); }

    void insert(int n)
    {
        mru.insert(addr(n));
        set.insert(addr(n));
        BOOST_CHECK(mru == set);
    }
};

BOOST_AUTO_TEST_SUITE(mruset_tests)

// Test that an mruset behaves like a set, as long as no more than MAX_SIZE elements are in it
BOOST_AUTO_TEST_CASE(mruset_like_set)
{

    for (int nTest=0; nTest<NUM_TESTS; nTest++)
    {
        mrutester tester;
        while (tester.size() < MAX_SIZE)
            tester.insert(GetRandInt(2 * MAX_SIZE));
    }

}

// Test that an mruset's size never exceeds its max_size
BOOST_AUTO_TEST_CASE(mruset_limited_size)
{
    for (int nTest=0; nTest<NUM_TESTS; nTest++)
    {
        mruset<CAddress> mru(MAX_SIZE);
        for (int nAction=0; nAction<3*MAX_SIZE; nAction++)
        {
            int n = GetRandInt(2 * MAX_SIZE);
            mru.insert(addr(n));
            BOOST_CHECK(mru.size() <= MAX_SIZE);
        }
    }
}

// 16-bit permutation function
int static permute(int n)
{
    // hexadecimals of pi; verified to be linearly independent
    static const int table[16] = {0x243F, 0x6A88, 0x85A3, 0x08D3, 0x1319, 0x8A2E, 0x0370, 0x7344,
                                  0xA409, 0x3822, 0x299F, 0x31D0, 0x082E, 0xFA98, 0xEC4E, 0x6C89};

    int ret = 0;
    for (int bit=0; bit<16; bit++)
         if (n & (1<<bit))
             ret ^= table[bit];

    return ret;
}

// Test that an mruset acts like a moving window, if no duplicate elements are added
BOOST_AUTO_TEST_CASE(mruset_window)
{
    mruset<CAddress> mru(MAX_SIZE);
    for (int n=0; n<10*MAX_SIZE; n++)
    {
        mru.insert(addr(permute(n)));

        set<CAddress> tester;
        for (int m=max(0,n-MAX_SIZE+1); m<=n; m++)
            tester.insert(addr(permute(m)));

        BOOST_CHECK(mru == tester);
    }
}

BOOST_AUTO_TEST_SUITE_END()
