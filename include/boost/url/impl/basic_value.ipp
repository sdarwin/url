//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#ifndef BOOST_URL_IMPL_BASIC_VALUE_IPP
#define BOOST_URL_IMPL_BASIC_VALUE_IPP

#include <boost/url/error.hpp>
#include <boost/url/basic_value.hpp>
#include <boost/url/detail/parse.hpp>
#include <cstring>
#include <stdexcept>

namespace boost {
namespace url {

//----------------------------------------------------------

basic_value::
basic_value(
    detail::storage& a) noexcept
    : a_(a)
    , s_(nullptr)
{
}

basic_value::
basic_value(
    detail::storage& a,
    string_view s)
    : a_(a)
{
    set_encoded_url(s);
}

string_view
basic_value::
encoded_url() const
{
    return pt_.get(
        detail::id_scheme,
        detail::id_end,
        s_);
}

string_view
basic_value::
encoded_origin() const noexcept
{
    return pt_.get(
        detail::id_scheme,
        detail::id_path,
        s_);
}

basic_value&
basic_value::
set_encoded_url(
    string_view s)
{
    if(s.empty())
    {
        if(s_)
            resize(
                detail::id_scheme,
                detail::id_end, 0);
        return *this;
    }
    error_code ec;
    detail::parser pr(s);
    detail::parts pt;
    detail::parse_url(pt, s, ec);
    if(ec)
        invalid_part::raise();
    s_ = a_.resize(s.size());
    //---
    pt_ = pt;
    std::memcpy(
        s_, s.data(), s.size());
    return *this;
}

basic_value&
basic_value::
set_encoded_origin(
    string_view s)
{
    if(s.empty())
    {
        resize(
            detail::id_scheme,
            detail::id_path, 0);
        pt_.port = {};
        return *this;
    }

    error_code ec;
    detail::parts pt;
    detail::parse_origin(pt, s, ec);
    if(ec)
        invalid_part::raise();
    auto const dest =
        resize(
            detail::id_scheme,
            detail::id_path,
            s.size());
    s.copy(dest, s.size());
    pt_.split(
        detail::id_scheme,
        pt.length(detail::id_scheme));
    pt_.split(
        detail::id_username,
        pt.length(detail::id_username));
    pt_.split(
        detail::id_password,
        pt.length(detail::id_password));
    pt_.split(
        detail::id_hostname,
        pt.length(detail::id_hostname));
    pt_.split(
        detail::id_port, pt.length(detail::id_port));
    pt_.port = pt.port;
    return *this;
}

//----------------------------------------------------------
//
// scheme
//
//----------------------------------------------------------

string_view
basic_value::
scheme() const noexcept
{
    auto s = pt_.get(
        detail::id_scheme,
        s_);
    if(s.empty())
        return s;
    BOOST_ASSERT(s.back() == ':');
    s.remove_suffix(1); // ':'
    return s;
}

basic_value&
basic_value::
set_scheme(
    string_view s)
{
    if(s.empty())
    {
        resize(detail::id_scheme, 0);
        return *this;
    }

    detail::parts pr;
    detail::parse_scheme(pr, s);
    auto const n = s.size();
    auto const dest =
        resize(detail::id_scheme, n + 1);
    s.copy(dest, n);
    dest[n] = ':';
    return *this;
}

//----------------------------------------------------------
//
// authority
//
//----------------------------------------------------------

string_view
basic_value::
encoded_authority() const noexcept
{
    auto s = pt_.get(
        detail::id_username,
        detail::id_path,
        s_);
    if(! s.empty())
    {
        BOOST_ASSERT(s.size() >= 2);
        BOOST_ASSERT(
            s.substr(0, 2) == "//");
        s.remove_prefix(2);
    }
    return s;
}

basic_value&
basic_value::
set_encoded_authority(
    string_view s)
{
    if(s.empty())
    {
        resize(
            detail::id_username,
            detail::id_path, 0);
        pt_.port = {};
        return *this;
    }

    detail::parts pt;
    detail::parse_authority(pt, s);
    auto const dest = resize(
        detail::id_username,
        detail::id_path,
        2 + s.size());
    //---
    dest[0] = '/';
    dest[1] = '/';
    s.copy(dest + 2, s.size());
    pt_.split(
        detail::id_username,
        2 + pt.length(detail::id_username));
    pt_.split(
        detail::id_password,
        pt.length(detail::id_password));
    pt_.split(
        detail::id_hostname,
        pt.length(detail::id_hostname));
    BOOST_ASSERT(
        pt_.length(detail::id_port) ==
            pt.length(detail::id_port));
    pt_.port = pt.port;
    return *this;
}

//
// userinfo
//

string_view
basic_value::
encoded_userinfo() const noexcept
{
    auto s = pt_.get(
        detail::id_username,
        detail::id_hostname,
        s_);
    if(s.empty())
        return s;
    if(s.back() == '@')
        s.remove_suffix(1);
    BOOST_ASSERT(s.size() >= 2);
    BOOST_ASSERT(
        s.substr(0, 2) == "//");
    s.remove_prefix(2);
    return s;
}

basic_value&
basic_value::
set_encoded_userinfo(
    string_view s)
{
    if(s.empty())
    {
        if(pt_.length(
            detail::id_hostname,
            detail::id_path) == 0)
        {
            // no authority
            resize(
                detail::id_username,
                detail::id_hostname, 0);
            return *this;
        }
        // keep "//"
        resize(
            detail::id_username,
            detail::id_hostname, 2);
        return *this;
    }

    error_code ec;
    auto e =
        detail::userinfo_nc_pct_set();
    auto p = e.parse(
        s.begin(), s.end(), ec);
    if(ec)
        invalid_part::raise();
    auto const n0 = static_cast<
        std::size_t>(p - s.begin());
    e = detail::userinfo_pct_set();
    p = e.parse(p, s.end(), ec);
    if(ec)
        invalid_part::raise();
    if(p != s.end())
        invalid_part::raise();

    auto dest = resize(
        detail::id_username,
        detail::id_hostname,
        2 + s.size() + 1);
    dest[0] = '/';
    dest[1] = '/';
    dest += 2;
    s.copy(dest, s.size());
    pt_.split(detail::id_username, n0);
    dest[s.size()] = '@';
    return *this;
}

string_view
basic_value::
encoded_username() const noexcept
{
    auto s = pt_.get(
        detail::id_username,
        s_);
    if(! s.empty())
    {
        BOOST_ASSERT(s.size() >= 2);
        BOOST_ASSERT(
            s.substr(0, 2) == "//");
        s.remove_prefix(2);
    }
    return s;
}

basic_value&
basic_value::
set_username(
    string_view s)
{
    if(s.empty())
    {
        if(pt_.length(
            detail::id_username) == 0)
            return *this;
        BOOST_ASSERT(pt_.get(
            detail::id_password, s_).back() == '@');
        BOOST_ASSERT(pt_.get(
            detail::id_username, s_).size() >= 2);
        BOOST_ASSERT(pt_.get(
            detail::id_username, s_)[0] == '/');
        BOOST_ASSERT(pt_.get(
            detail::id_username, s_)[1] == '/');
        if(pt_.length(
            detail::id_password) == 1)
        {
            // remove '@'
            resize(
                detail::id_username,
                detail::id_hostname, 2);
        }
        else
        {
            resize(detail::id_username, 2);
        }
        return *this;
    }

    auto const e =
        detail::userinfo_nc_pct_set();
    if(pt_.length(
        detail::id_password) != 0)
    {
        BOOST_ASSERT(pt_.get(
            detail::id_password, s_).back() == '@');
        // preserve "//"
        auto const dest = resize(
            detail::id_username,
            2 + e.encoded_size(s));
        e.encode(dest + 2, s);
        return *this;
    }
    auto const n = e.encoded_size(s);
    auto const dest = resize(
        detail::id_username, 2 + n + 1);
    dest[0] = '/';
    dest[1] = '/';
    dest[2 + n] = '@';
    pt_.split(
        detail::id_username,
        2 + n);
    e.encode(dest + 2, s);
    return *this;
}

basic_value&
basic_value::
set_encoded_username(
    string_view s)
{
    if(s.empty())
        return set_username(s);

    auto const e =
        detail::userinfo_nc_pct_set();
    e.validate(s);

    auto const n = s.size();
    if(pt_.length(detail::id_password) != 0)
    {
        BOOST_ASSERT(pt_.get(
            detail::id_password, s_).back() == '@');
        // preserve "//"
        auto const dest = resize(
            detail::id_username, 2 + n);
        s.copy(dest + 2, n);
        return *this;
    }

    // add '@'
    auto const dest = resize(
        detail::id_username,
        2 + n + 1);
    dest[0] = '/';
    dest[1] = '/';
    dest[2 + n] = '@';
    pt_.split(
        detail::id_username,
        2 + n);
    s.copy(dest + 2, n);
    return *this;
}

string_view
basic_value::
encoded_password() const noexcept
{
    auto s = pt_.get(
        detail::id_password,
        s_);
    switch(s.size())
    {
    case 1:
        BOOST_ASSERT(s.front() == '@');
    case 0:
        return {};
    default:
        BOOST_ASSERT(s.back() == '@');
        s.remove_suffix(1);
        if(s.front() == ':')
            s.remove_prefix(1);
        return s;
    }
}

basic_value&
basic_value::
set_password(
    string_view s)
{
    if(s.empty())
    {
        auto const n = pt_.length(
            detail::id_password);
        if(n == 0)
            return *this;
        BOOST_ASSERT(pt_.get(
            detail::id_password, s_).back() == '@');
        BOOST_ASSERT(pt_.get(
            detail::id_username, s_).size() >= 2);
        BOOST_ASSERT(pt_.get(
            detail::id_username, s_)[0] == '/');
        BOOST_ASSERT(pt_.get(
            detail::id_username, s_)[1] == '/');
        if(pt_.length(detail::id_username) == 2)
        {
            // remove '@'
            resize(detail::id_password, 0);
            return *this;
        }
        // retain '@'
        *resize(detail::id_password, 1) = '@';
        return *this;
    }

    auto const e =
        detail::userinfo_pct_set();
    auto const n =
        e.encoded_size(s);
    if(pt_.length(detail::id_username) != 0)
    {
        auto const dest = resize(
            detail::id_password, 1 + n + 1);
        dest[0] = ':';
        dest[n + 1] = '@';
        e.encode(dest + 1, s);
        return *this;
    }
    auto const dest = resize(
        detail::id_username,
        detail::id_hostname,
        2 + 1 + n + 1);
    dest[0] = '/';
    dest[1] = '/';
    dest[2] = ':';
    dest[2 + n + 1] = '@';
    e.encode(dest + 3, s);
    pt_.split(detail::id_username, 2);
    return *this;
}

basic_value&
basic_value::
set_encoded_password(
    string_view s)
{
    if(s.empty())
        return set_password(s);

    auto const e =
        detail::userinfo_pct_set();
    if(s[0] == ':')
        invalid_part::raise();
    e.validate(s);

    auto const n = s.size();
    if(pt_.length(detail::id_username) != 0)
    {
        auto const dest = resize(
            detail::id_password, 1 + n + 1);
        dest[0] = ':';
        dest[n + 1] = '@';
        s.copy(dest + 1, n);
        return *this;
    }
    auto const dest = resize(
        detail::id_username,
        detail::id_hostname,
        2 + 1 + n + 1);
    dest[0] = '/';
    dest[1] = '/';
    dest[2] = ':';
    dest[2 + n + 1] = '@';
    s.copy(dest + 3, n);
    pt_.split(detail::id_username, 2);
    return *this;
}

//
// host
//

string_view
basic_value::
encoded_host() const noexcept
{
    return pt_.get(
        detail::id_hostname,
        detail::id_path,
        s_);
}

basic_value&
basic_value::
set_encoded_host(
    string_view s)
{
    if(s.empty())
    {
        resize(
            detail::id_hostname,
            detail::id_path, 0);
        pt_.port = {};
        return *this;
    }

    error_code ec;
    detail::parts pt;
    detail::parse_host(pt, s);
    BOOST_ASSERT(s.size() ==
        pt.length(detail::id_hostname) +
        pt.length(detail::id_port));

    if(pt_.length(
        detail::id_username,
        detail::id_path) == 0)
    {
        // add authority
        auto const dest = resize(
            detail::id_username,
            2 + s.size());
        dest[0] = '/';
        dest[1] = '/';
        s.copy(dest + 2, s.size());
        pt_.split(detail::id_username, 2);
        pt_.split(detail::id_password, 0);
        pt_.split(detail::id_hostname,
            pt.length(detail::id_hostname));
    }
    else
    {
        auto const dest = resize(
            detail::id_hostname,
            detail::id_path,
            s.size());
        s.copy(dest, s.size());
        pt_.split(
            detail::id_hostname,
            pt.length(detail::id_hostname));
    }
    BOOST_ASSERT(pt_.length(
        detail::id_port) == pt.length(
            detail::id_port));
    pt_.host = pt.host;
    pt_.port = pt.port;
    return *this;
}

string_view
basic_value::
encoded_hostname() const noexcept
{
    return pt_.get(
        detail::id_hostname,
        s_);
}

basic_value&
basic_value::
set_hostname(
    string_view s)
{
    if(s.empty())
    {
        resize(detail::id_hostname, 0);
        return *this;
    }

    auto const e =
        detail::reg_name_pct_set();
    auto const dest = resize(
        detail::id_hostname,
        e.encoded_size(s));
    e.encode(dest, s);
    detail::parts pt;
    detail::parse_hostname(
        pt, encoded_host());
    pt_.host = pt.host;
    return *this;
}

basic_value&
basic_value::
set_encoded_hostname(
    string_view s)
{
    if(s.empty())
        return set_hostname(s);

    detail::parts pt;
    detail::parse_hostname(pt, s);
    auto const dest = resize(
        detail::id_hostname,
        s.size());
    s.copy(dest, s.size());
    pt_.host = pt.host;
    return *this;
}

string_view
basic_value::
port_string() const noexcept
{
    auto s = pt_.get(
        detail::id_port,
        s_);
    BOOST_ASSERT(
        s.empty() || s.front() == ':');
    if(! s.empty())
        s.remove_prefix(1);
    return s;
}

basic_value&
basic_value::
set_port(
    optional<unsigned short> num)
{
    if(! num.has_value())
    {
        resize(detail::id_port, 0);
        pt_.port = {};
        return *this;
    }

    detail::port_string s(*num);
    if(pt_.length(
        detail::id_username,
        detail::id_path) == 0)
    {
        // add authority
        auto const dest = resize(
            detail::id_username,
            3 + s.size());
        dest[0] = '/';
        dest[1] = '/';
        dest[2] = ':';
        pt_.split(
            detail::id_username, 2);
        pt_.split(
            detail::id_password, 0);
        pt_.split(
            detail::id_hostname, 0);
        s.get().copy(
            dest + 3, s.size());
    }
    else
    {
        auto const dest = resize(
            detail::id_port,
            1 + s.size());
        dest[0] = ':';
        s.get().copy(
            dest + 1, s.size());
    }
    pt_.port = num;
    return *this;
}

basic_value&
basic_value::
set_port_string(string_view s)
{
    if(s.empty())
        return set_port({});

    detail::parts pt;
    detail::parse_port(pt, s);
    set_port(pt.port);
    return *this;
}

//----------------------------------------------------------
//
// path
//
//----------------------------------------------------------

bool
basic_value::
is_relative() const noexcept
{
    auto const s = pt_.get(
        detail::id_path, s_);
    return ! s.empty() &&
        s.front() != '/';
}

string_view
basic_value::
encoded_path() const noexcept
{
    return pt_.get(
        detail::id_path,
        s_);
}

basic_value&
basic_value::
set_encoded_path(
    string_view s)
{
    if(s.empty())
    {
        resize(
            detail::id_path,
            detail::id_query, 0);
        return *this;
    }

    detail::parts pt;
    detail::parse_path_abempty(pt, s);
    auto const dest = resize(
        detail::id_path,
        detail::id_query,
        s.size());
    s.copy(dest, s.size());
    return *this;
}

//----------------------------------------------------------
//
// query
//
//----------------------------------------------------------

string_view
basic_value::
encoded_query() const noexcept
{
    return pt_.get(
        detail::id_query,
        detail::id_frag,
        s_);
}

basic_value&
basic_value::
set_query(
    string_view s)
{
    if(s.empty())
    {
        resize(detail::id_query, 0);
        return *this;
    }

    if(s.front() == '?')
        s = s.substr(1);
    auto const e =
        detail::query_pct_set();
    auto const n =
        e.encoded_size(s);

    auto const dest = resize(
        detail::id_query,
        1 + n);
    dest[0] = '?';
    e.encode(dest + 1, s);
    return *this;
}

basic_value&
basic_value::
set_encoded_query(
    string_view s)
{
    if(s.empty())
    {
        resize(detail::id_query, 0);
        return *this;
    }

    if(s.front() == '?')
        s = s.substr(1);
    auto const e =
        detail::query_pct_set();
    e.validate(s);

    auto const dest = resize(
        detail::id_query,
        1 + s.size());
    dest[0] = '?';
    s.copy(dest + 1, s.size());
    return *this;
}

//----------------------------------------------------------
//
// fragment
//
//----------------------------------------------------------

string_view
basic_value::
encoded_fragment() const noexcept
{
    auto s = pt_.get(
        detail::id_frag,
        detail::id_end,
        s_);
    if(s.empty())
        return s;
    BOOST_ASSERT(
        s.front() == '#');
    return s.substr(1);
}

string_view
basic_value::
fragment_part() const noexcept
{
    auto s = pt_.get(
        detail::id_frag,
        detail::id_end,
        s_);
    if(s.empty())
        return s;
    BOOST_ASSERT(
        s.front() == '#');
    return s;
}

basic_value&
basic_value::
set_fragment(
    string_view s)
{
    if(s.empty())
    {
        resize(detail::id_frag, 0);
        return *this;
    }
    auto const e =
        detail::frag_pct_set();
    auto const n =
        e.encoded_size(s);
    auto const dest = resize(
        detail::id_frag, 1 + n);
    dest[0] = '#';
    e.encode(dest + 1, s);
    return *this;
}

basic_value&
basic_value::
set_encoded_fragment(
    string_view s)
{
    if(s.empty())
    {
        resize(detail::id_frag, 0);
        return *this;
    }
    auto const e =
        detail::frag_pct_set();
    e.validate(s);
    auto const dest = resize(
        detail::id_frag,
        1 + s.size());
    dest[0] = '#';
    s.copy(dest + 1, s.size());
    return *this;
}

basic_value&
basic_value::
set_fragment_part(
    string_view s)
{
    if(s.empty())
        return set_fragment(s);
    if(s.front() != '#')
        invalid_part::raise();
    s = s.substr(1);
    auto const e =
        detail::frag_pct_set();
    e.validate(s);
    auto const dest = resize(
        detail::id_frag,
        1 + s.size());
    dest[0] = '#';
    s.copy(dest + 1, s.size());
    return *this;
}

//----------------------------------------------------------
//
// segments_type
//
//----------------------------------------------------------

basic_value::
segments_type::
iterator::
iterator() noexcept
    : v_(nullptr)
    , off_(0)
    , n_(0)
{
}

basic_value::
segments_type::
iterator::
iterator(
    basic_value* v,
    bool end) noexcept
    : v_(v)
{
    if( end ||
        v_->pt_.nseg == 0)
    {
        off_ = v_->pt_.offset[
            detail::id_query];
        n_ = 0;
    }
    else
    {
        off_ = v_->pt_.offset[
            detail::id_path];
        parse();
    }
}

auto
basic_value::
segments_type::
iterator::
operator*() const noexcept ->
    value_type
{
    string_view s = {
        v_->s_ + off_, n_ };
    if(! s.empty() &&
        s.front() == '/')
        s = s.substr(1);    
    return value_type(s);
}

auto
basic_value::
segments_type::
iterator::
operator++() noexcept ->
    iterator&
{
    BOOST_ASSERT(
        off_ != v_->pt_.offset[
            detail::id_frag]);
    off_ = off_ + n_;
    if(off_ == v_->pt_.offset[
        detail::id_frag])
    {
        // end
        n_ = 0;
    }
    else
    {
        parse();
    }
    return *this;
}

auto
basic_value::
segments_type::
iterator::
operator--() noexcept ->
    iterator&
{
    BOOST_ASSERT(
        off_ != v_->pt_.offset[
            detail::id_path]);
    auto const begin =
        v_->s_ + v_->pt_.offset[
            detail::id_path];
    auto p = v_->s_ + off_;
    while(--p > begin)
    {
        if(*p == '/')
        {
            off_ = p - v_->s_;
            parse();
            return *this;
        }
    }
    // fails for relative-uri
    //BOOST_ASSERT(*p == '/');
    auto const off = p - v_->s_;
    n_ = off_ - off;
    off_ = off;
    return *this;
}

void
basic_value::
segments_type::
iterator::
parse() noexcept
{
    BOOST_ASSERT(off_ !=
        v_->pt_.offset[
            detail::id_frag]);
    auto const end =
        v_->s_ + v_->pt_.offset[
            detail::id_frag];
    auto const p0 =
        v_->s_ + off_;
    auto p = p0;
    if(*p == '/')
        ++p;
    while(p < end)
    {
        if(*p == '/')
            break;
        ++p;
    }
    n_ = p - p0;
}

//----------------------------------------------------------

auto
basic_value::
segments_type::
begin() const noexcept ->
    iterator
{
    BOOST_ASSERT(v_);
    return iterator(v_, false);
}

auto
basic_value::
segments_type::
end() const noexcept ->
    iterator
{
    BOOST_ASSERT(v_);
    return iterator(v_, true);
}

//----------------------------------------------------------
//
// params_type
//
//----------------------------------------------------------

basic_value::
params_type::
iterator::
iterator() noexcept
    : v_(nullptr)
    , off_(0)
    , nk_(0)
    , nv_(0)
{
}

basic_value::
params_type::
iterator::
iterator(
    basic_value* v,
    bool end) noexcept
    : v_(v)
{
    if( end ||
        v_->pt_.nparam == 0)
    {
        off_ = v_->pt_.offset[
            detail::id_frag];
        nk_ = 0;
        nv_ = 0;
    }
    else
    {
        off_ = v_->pt_.offset[
            detail::id_query];
        parse();
    }
}

auto
basic_value::
params_type::
iterator::
operator*() const noexcept ->
    value_type
{
    BOOST_ASSERT(nk_ > 0);
    BOOST_ASSERT(
        off_ == v_->pt_.offset[
            detail::id_query] ?
        v_->s_[off_] == '?' :
        v_->s_[off_] == '&');
    string_view const k = {
        v_->s_ + off_ + 1,
        nk_ - 1 };

    BOOST_ASSERT(nv_ == 0 ||
        v_->s_[off_ + nk_] == '=');
    string_view const v = {
        v_->s_ + off_ + nk_ + 1,
        nv_ - 1};

    return { k, v };
}

auto
basic_value::
params_type::
iterator::
operator++() noexcept ->
    iterator&
{
    BOOST_ASSERT(
        off_ != v_->pt_.offset[
            detail::id_frag]);
    off_ = off_ + nv_ + nk_;
    if(off_ == v_->pt_.offset[
        detail::id_frag])
    {
        // end
        nv_ = 0;
        nk_ = 0;
    }
    else
    {
        parse();
    }
    return *this;
}

auto
basic_value::
params_type::
iterator::
operator--() noexcept ->
    iterator&
{
    BOOST_ASSERT(
        off_ != v_->pt_.offset[
            detail::id_query]);
    auto const begin =
        v_->s_ + v_->pt_.offset[
            detail::id_query];
    auto p = v_->s_ + off_;
    while(--p > begin)
    {
        if(*p == '&')
        {
            off_ = p - v_->s_;
            parse();
            return *this;
        }
    }
    BOOST_ASSERT(*p == '?');
    off_ = p - v_->s_;
    return *this;
}

void
basic_value::
params_type::
iterator::
parse() noexcept
{
    auto const end =
        v_->s_ + v_->pt_.offset[
            detail::id_end];
    char const* p = v_->s_ + off_;
    BOOST_ASSERT(
        ( off_ == v_->pt_.offset[
            detail::id_query] &&
            *p == '?' ) ||
        ( off_ != v_->pt_.offset[
            detail::id_query] &&
            *p == '&' ));
    auto p0 = p++;
    auto const ek =
        detail::qkey_pct_set();
    error_code ec;
    p = ek.parse(p, end, ec);
    BOOST_ASSERT(! ec);
    nk_ = p - p0;
    if(p == end)
    {
        nv_ = 0;
        return;
    }
    auto const ev =
        detail::qval_pct_set();
    BOOST_ASSERT(*p == '=');
    p0 = p++;
    p = ev.parse(p, end, ec);
    BOOST_ASSERT(! ec);
    nv_ = p - p0;
}

//----------------------------------------------------------

auto
basic_value::
params_type::
begin() const noexcept ->
    iterator
{
    BOOST_ASSERT(v_);
    return iterator(v_, false);
}

auto
basic_value::
params_type::
end() const noexcept ->
    iterator
{
    BOOST_ASSERT(v_);
    return iterator(v_, true);
}

bool
basic_value::
params_type::
contains(string_view key) const noexcept
{
    BOOST_ASSERT(v_);
    for(auto e : *this)
        if(detail::key_equal(
            e.encoded_key(),
            key))
            return true;
    return false;
}

std::size_t
basic_value::
params_type::
count(string_view key) const noexcept
{
    BOOST_ASSERT(v_);
    std::size_t n = 0;
    for(auto e : *this)
        if(detail::key_equal(
            e.encoded_key(),
            key))
            ++n;
    return n;
}

auto
basic_value::
params_type::
find(string_view key) const noexcept ->
    iterator
{
    BOOST_ASSERT(v_);
    auto it = begin();
    for(auto const last = end();
        it != last; ++it)
        if(detail::key_equal(
            it->encoded_key(),
            key))
            break;
    return it;
}

std::string
basic_value::
params_type::
operator[](string_view key) const
{
    BOOST_ASSERT(v_);
    auto const it = find(key);
    if(it == end())
        return "";
    return it->value();
}

//----------------------------------------------------------

char*
basic_value::
resize(
    int id,
    std::size_t new_size)
{
    auto const len = pt_.length(id);
    if(len == new_size)
        return s_ + pt_.offset[id];

    // shrink
    if(new_size <= len)
    {
        auto const n = static_cast<
            std::size_t>(len - new_size);
        auto const pos = pt_.offset[id + 1];
        std::memmove(
            s_ + pos - n,
            s_ + pos,
            pt_.offset[
                detail::id_end] - pos + 1);
        for(auto i = id + 1;
            i <= detail::id_end; ++i)
            pt_.offset[i] -= n;
        return s_ + pt_.offset[id];
    }

    // grow
    if(new_size - len > (
        (std::size_t)-1)- size())
        too_large::raise();
    s_ = a_.resize(
        size() - len + new_size);
    auto const n = static_cast<
        std::size_t>(new_size - len);
    auto const pos =
        pt_.offset[id + 1];
    std::memmove(
        s_ + pos + n,
        s_ + pos,
        pt_.offset[detail::id_end] -
            pos + 1);
    for(auto i = id + 1;
        i <= detail::id_end; ++i)
        pt_.offset[i] += n;
    return s_ + pt_.offset[id];
}

char*
basic_value::
resize(
    int first,
    int last,
    std::size_t new_size)
{
    auto const len =
        pt_.length(first, last);
    if(new_size == 0 && len == 0)
        return s_ + pt_.offset[first];

    // shrink
    if(new_size <= len)
    {
        auto const n = static_cast<
            std::size_t>(len - new_size);
        auto const pos = pt_.offset[last];
        std::memmove(
            s_ + pos - n,
            s_ + pos,
            pt_.offset[
                detail::id_end] - pos + 1);
        for(auto i = first + 1;
            i < last; ++i)
            pt_.offset[i] =
                pt_.offset[last] - n;
        for(auto i = last;
            i <= detail::id_end; ++i)
            pt_.offset[i] -= n;
        return s_ + pt_.offset[first];
    }

    // grow
    if(new_size - len > (
        (std::size_t)-1)- size())
        too_large::raise();
    s_ = a_.resize(
        size() - len + new_size);
    auto const n = static_cast<
        std::size_t>(new_size - len);
    auto const pos =
        pt_.offset[last];
    std::memmove(
        s_ + pos + n,
        s_ + pos,
        pt_.offset[detail::id_end] -
            pos + 1);
    for(auto i = first + 1;
        i < last; ++i)
        pt_.offset[i] =
            pt_.offset[last] + n;
    for(auto i = last;
        i <= detail::id_end; ++i)
        pt_.offset[i] += n;
    return s_ + pt_.offset[first];
}

} // url
} // boost

#endif