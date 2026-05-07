//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
//

#ifndef BOOST_URL_SUBDIR_A_SUBDIR_B_SUBDIR_C_SUBDIR_D_SUBDIR_E_SUBDIR_F_SUBDIR_G_SUBDIR_H_SUBDIR_I_PARAM_HPP
#define BOOST_URL_SUBDIR_A_SUBDIR_B_SUBDIR_C_SUBDIR_D_SUBDIR_E_SUBDIR_F_SUBDIR_G_SUBDIR_H_SUBDIR_I_PARAM_HPP

#include <boost/url/param.hpp>

namespace boost {
namespace urls {
namespace subdir_a_coverage_detail {

inline
int
coverage_probe(int v)
{
    if (v > 0)
        return v + 1;
    return v;
}

} // subdir_a_coverage_detail
} // urls
} // boost

#endif
