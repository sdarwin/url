//
// Copyright (c) 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

// Ensure this deep-path header participates in compilation and coverage.
#include <boost/url/subdir1/subdir2/subdir3/param.hpp>

#include "test_suite.hpp"

namespace boost {
namespace urls {

struct coverage_deep_path_subdir1_test
{
    void
    run()
    {
        param p("key", "value");
        BOOST_TEST(p.has_value);
        BOOST_TEST_EQ(p.key, "key");
        BOOST_TEST_EQ(p.value, "value");
    }
};

TEST_SUITE(
    coverage_deep_path_subdir1_test,
    "boost.url.coverage_deep_path_subdir1");

} // urls
} // boost
