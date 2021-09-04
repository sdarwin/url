//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_IMPL_USERINFO_IPP
#define BOOST_URL_RFC_IMPL_USERINFO_IPP

#include <boost/url/rfc/userinfo.hpp>
#include <boost/url/error.hpp>
#include <boost/url/string.hpp>
#include <boost/url/rfc/char_sets.hpp>
#include <boost/url/bnf/literal.hpp>
#include <boost/url/rfc/pct_encoded.hpp>
#include <boost/url/rfc/pct_encoding.hpp>

namespace boost {
namespace urls {
namespace rfc {

char const*
parse(
    char const* const start,
    char const* const end,
    error_code& ec,
    userinfo& t)
{
    pct_encoded_str user;
    pct_encoded_str pass;
    optional<
        bnf::literal<':'>> colon;
    auto it = parse(start, end, ec,
        pct_encoded<
            unsub_char_mask>{user},
        colon,
        pct_encoded<
            unsub_char_mask |
            colon_char_mask>{pass});
    if(ec)
        return start;
    t.str = string_view(
        start, it - start);
    t.user = user;
    if(colon.has_value())
        t.pass = pass;
    else
        t.pass.reset();
    return it;
}

} // rfc
} // urls
} // boost

#endif
