//
// Copyright (c) 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

// Ensure this deep-path header participates in compilation and coverage.
#include <boost/url/subdir_a/subdir_b/subdir_c/subdir_d/subdir_e/subdir_f/subdir_g/subdir_h/subdir_i/param.hpp>

#include "test_suite.hpp"

namespace boost {
namespace urls {

struct coverage_deep_path_subdir_a_test
{
    void
    run()
    {
        param p("alpha", "beta");
        BOOST_TEST(p.has_value);
        BOOST_TEST_EQ(p.key, "alpha");
        BOOST_TEST_EQ(p.value, "beta");
        BOOST_TEST_EQ(
            subdir_a_coverage_detail::coverage_probe(1),
            2);
    }
};

TEST_SUITE(
    coverage_deep_path_subdir_a_test,
    "boost.url.coverage_deep_path_subdir_a");

} // urls
} // boost
