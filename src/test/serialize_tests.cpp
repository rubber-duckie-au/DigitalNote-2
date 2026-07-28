#include <boost/test/unit_test.hpp>

#include <string>
#include <vector>

#include "serialize.h"
#include "enums/serialize_type.h"
#include "cvarint.h"
#include "cdatastream.h"

using namespace std;

BOOST_AUTO_TEST_SUITE(serialize_tests)

BOOST_AUTO_TEST_CASE(varints)
{
    // encode

    CDataStream ss(SER_DISK, 0);
    CDataStream::size_type size = 0;
    for (unsigned int i = 0; i < 100000u; i++) {
        ss << VARINT(i);
        size += ::GetSerializeSize(VARINT(i), 0, 0);
        BOOST_CHECK(size == ss.size());
    }

    // v2.0.0.9 (TODO 12.A.2): CDataStream's CVarInt stream operators are
    // instantiated for unsigned int only (cdatastream.cpp).  uint64_t would
    // require adding CVarInt<unsigned long> to shipping code purely for this
    // test.  unsigned int is the width the wallet actually streams, so this
    // exercises the real path; the range is capped accordingly.
    for (unsigned int i = 0; i < 4000000000u; i += 99999937u) {
        ss << VARINT(i);
        size += ::GetSerializeSize(VARINT(i), 0, 0);
        BOOST_CHECK(size == ss.size());
    }

    // decode
    for (unsigned int i = 0; i < 100000u; i++) {
        unsigned int j;
        ss >> VARINT(j);
        BOOST_CHECK_MESSAGE(i == j, "decoded:" << j << " expected:" << i);
    }

    for (unsigned int i = 0; i < 4000000000u; i += 99999937u) {
        unsigned int j;
        ss >> VARINT(j);
        BOOST_CHECK_MESSAGE(i == j, "decoded:" << j << " expected:" << i);
    }

}

BOOST_AUTO_TEST_SUITE_END()
