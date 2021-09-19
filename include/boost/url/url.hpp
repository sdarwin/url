//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_URL_HPP
#define BOOST_URL_URL_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/ipv4_address.hpp>
#include <boost/url/ipv6_address.hpp>
#include <boost/url/scheme.hpp>
#include <boost/url/url_view.hpp>
#include <boost/url/detail/pct_encoding.hpp>
#include <cstdint>
#include <iosfwd>
#include <memory>
#include <string>
#include <utility>

namespace boost {
namespace urls {

#ifndef BOOST_URL_DOCS
class url_view;
#endif

/** A modifiable container for a URL.

    Objects of this type hold URLs which may be
    inspected and modified. The derived class
    is responsible for providing storage.

    The underlying string stored in the container
    is always null-terminated.

    @par Exception Safety

    @li Functions marked `noexcept` provide the
    no-throw guarantee, otherwise:

    @li Functions which throw offer the strong
    exception safety guarantee.

    @par Specification
    @li <a href="https://tools.ietf.org/html/rfc3986">
        Uniform Resource Identifier (URI): Generic Syntax (rfc3986)</a>
*/
class BOOST_SYMBOL_VISIBLE url
    : public url_view
{
#ifndef BOOST_URL_DOCS
protected:
#endif
    char* s_ = nullptr;
    std::size_t cap_ = 0;

    BOOST_URL_DECL
    url(char* buf,
        std::size_t cap) noexcept;

    BOOST_URL_DECL
    void
    copy(url_view const& u);

    BOOST_URL_DECL
    virtual
    char*
    allocate(
        std::size_t new_cap);

    BOOST_URL_DECL
    virtual
    void
    deallocate(char* s);

public:
    /** Destructor
    */
    BOOST_URL_DECL
    virtual
    ~url();

    /** Constructor
    */
    BOOST_URL_DECL
    url() noexcept;

    /** Constructor
    */
    BOOST_URL_DECL
    url(url&& u) noexcept;

    /** Constructor
    */
    BOOST_URL_DECL
    url(url_view const& u);

    /** Assignment
    */
    BOOST_URL_DECL
    url&
    operator=(url&& u) noexcept;

    /** Assignment
    */
    BOOST_URL_DECL
    url&
    operator=(url const& u);

    /** Assignment
    */
    BOOST_URL_DECL
    url&
    operator=(url_view const& u);

    //--------------------------------------------

    /** Return the encoded URL as a null-terminated string
    */
    char const*
    c_str() const noexcept
    {
        return cs_;
    }

    /** Returns the total number of bytes currently available to the container
    */
    std::size_t
    capacity_in_bytes() const noexcept
    {
        return cap_;
    }

    /** Clear the contents.
    
        @par Exception Safety
        No-throw guarantee.
    */
    BOOST_URL_DECL
    void
    clear() noexcept;

    /** Adjust the capacity
    */
    BOOST_URL_DECL
    void
    reserve(std::size_t n);

    //--------------------------------------------
    //
    // Scheme
    //
    //--------------------------------------------

private:
    void set_scheme_impl(string_view s, urls::scheme id);
public:

    /** Remove the scheme

        This function removes the scheme if it
        is present.

        @par BNF
        @code
        URI           = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
        @endcode

        @par Exception Safety
        Does not throw.

        @return A reference to the object, for chaining.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.1">
            3.1. Scheme (rfc3986)</a>

        @see @ref set_scheme.
    */
    BOOST_URL_DECL
    url&
    remove_scheme() noexcept;

    /** Set the scheme

        This function sets the scheme to the specified
        string, which must contain a valid scheme
        without a trailing colon (':'). If the scheme
        is invalid, an exception is thrown.

        @par Example
        @code
        url u;
        u.set_scheme( "http" );         // produces "http:"
        u.set_scheme( "1forall");       // throws, invalid scheme
        @endcode

        @par BNF
        @code
        scheme        = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
        @endcode

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @return A reference to the object, for chaining.

        @param s The scheme to set.

        @throw std::invalid_argument invalid scheme.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.1">
            3.1. Scheme (rfc3986)</a>
    */
    BOOST_URL_DECL
    url&
    set_scheme(string_view s);

    /** Set the scheme

        This function sets the scheme to the specified
        known @ref urls::scheme id, which may not be
        @ref scheme::unknown or else an exception is
        thrown. If the id is @ref scheme::none, this
        function behaves as if @ref remove_scheme
        were called.

        @par Example
        @code
        url u;
        u.set_scheme( scheme::http );           // produces "http:"
        u.set_scheme( scheme::none );           // produces ""
        u.set_scheme( scheme::unknown);         // throws, invalid scheme
        @endcode

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @return A reference to the object, for chaining.

        @param id The scheme to set.

        @throw std::invalid_argument invalid scheme.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.1">
            3.1. Scheme (rfc3986)</a>
    */
    BOOST_URL_DECL
    url&
    set_scheme(urls::scheme id);

    //--------------------------------------------
    //
    // Authority
    //
    //--------------------------------------------

private:
    char* set_user_impl(std::size_t n);
public:

    /** Remove the user

        If a user is present, it is removed. If the
        user was the only component present in the
        userinfo, then the userinfo is removed without
        removing the authority.

        @par Exception Safety
        Does not throw.

        @return A reference to the object, for chaining.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.1">
            3.2.1. User Information (rfc3986)</a>
    */
    BOOST_URL_DECL
    url&
    remove_user() noexcept;

    /** Set the user.

        The user is set to the specified string,
        replacing any previous user. If a userinfo
        was not present it is added, even if the
        user string is empty. The resulting URL
        will have an authority if it did not have
        one previously.

        Any special or reserved characters in the
        string are automatically percent-encoded.

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @return A reference to the object, for chaining.

        @param s The string to set. This string may
        contain any characters, including nulls.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.1">
            3.2.1. User Information (rfc3986)</a>
    */
    BOOST_URL_DECL
    url&
    set_user(
        string_view s);

    /** Set the user.

        The user is set to the specified string,
        replacing any previous user. If a userinfo
        was not present it is added, even if the
        user string is empty. The resulting URL
        will have an authority if it did not have
        one previously.

        The string must be a valid percent-encoded
        string for the user field, otherwise an
        exception is thrown.

        @par BNF
        @code
        user          = *( unreserved / pct-encoded / sub-delims )
        @endcode

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @return A reference to the object, for chaining.

        @param s The string to set.
    */
    BOOST_URL_DECL
    url&
    set_encoded_user(
        string_view s);

private:
    char* set_password_impl(std::size_t n);
public:

    /** Remove the password
    */
    BOOST_URL_DECL
    url&
    remove_password() noexcept;

    /** Set the password.

        This function sets the password to the specified
        string, replacing any previous password:

        @li If the string is empty, the password is
        cleared, and the first occurring colon (':') is
        removed from the userinfo if present, otherwise

        @li If ths string is not empty then the password
        is set to the new string.
        Any special or reserved characters in the
        string are automatically percent-encoded.
        If the URL previously did not have an authority
        (@ref has_authority returns `false`), a double
        slash ("//") is prepended to the userinfo.

        @par Exception Safety

        Strong guarantee.
        Calls to allocate may throw.

        @param s The string to set. This string may
        contain any characters, including nulls.
    */
    BOOST_URL_DECL
    url&
    set_password(
        string_view s);

    /** Set the password.

        The password is set to the encoded string `s`,
        replacing any previous password:

        @li If the string is empty, the password is
        cleared, and the first occurring colon (':') is
        removed from the userinfo if present, otherwise

        @li If ths string is not empty then the password
        is set to the new string.
        If the URL previously did not have an authority
        (@ref has_authority returns `false`), a double
        slash ("//") is prepended to the userinfo.
        The string must meet the syntactic requirements
        of <em>password</em> otherwise an exception is
        thrown.

        @par ANBF
        @code
        password      = *( unreserved / pct-encoded / sub-delims / ":" )
        @endcode

        @par Exception Safety

        Strong guarantee.
        Calls to allocate may throw.

        @param s The string to set.
    */
    BOOST_URL_DECL
    url&
    set_encoded_password(
        string_view s);

private:
    char* set_userinfo_impl(std::size_t n);
public:

    /** Remove the userinfo
    */
    BOOST_URL_DECL
    url&
    remove_userinfo() noexcept;

    /** Set the userinfo.

        Sets the userinfo of the URL to the given
        encoded string:

        @li If the string is empty, the userinfo is
        cleared, else

        @li If the string is not empty, then the userinfo
        is set to the given string. The user is set to
        the characters up to the first colon if any,
        while the password is set to the remaining
        characters if any.
        If the URL previously did not have an authority
        (@ref has_authority returns `false`), a double
        slash ("//") is prepended to the userinfo.
        The string must meet the syntactic requirements
        of <em>userinfo</em> otherwise an exception is
        thrown.

        @par BNF
        @code
        userinfo      = [ [ user ] [ ':' password ] ]

        user          = *( unreserved / pct-encoded / sub-delims )
        password      = *( unreserved / pct-encoded / sub-delims / ":" )
        @endcode

        @par Exception Safety

        Strong guarantee.
        Calls to allocate may throw.

        @param s The string to set.
    */
    BOOST_URL_DECL
    url&
    set_encoded_userinfo(
        string_view s);

    BOOST_URL_DECL
    url&
    set_userinfo(
        string_view s);

    //--------------------------------------------

private:
    char* set_host_impl(std::size_t n);
public:

    /** Set the host

        The host is set to the specified IPv4,
        address, replacing any previous host. If
        an authority was not present, it is added.

        @par Postconditions
        @code
        this->host_type() == host_type::ipv4 && this->ipv4_address() == addr
        @endcode

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.2">
            3.2.2. Host (rfc3986)</a>

        @return A reference to the object, for chaining.

        @param addr The address to set.
    */
#ifdef BOOST_URL_DOCS
    url& set_host( ipv4_address const& addr );
#else
    BOOST_URL_DECL
    url&
    set_host(
        urls::ipv4_address const& addr);
#endif

    /** Set the host

        The host is set to the specified IPv6,
        address, replacing any previous host.
        If an authority did not
        previously exist it is added by prepending
        a double slash ("//") at the beginning of
        the URL or after the scheme if a scheme is
        present.

        @par Postconditions
        @code
        this->host_type() == host_type::ipv6 && this->ipv6_address() == addr
        @endcode

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.2">
            3.2.2. Host (rfc3986)</a>

        @return A reference to the object, for chaining.

        @param addr The address to set.
    */
#ifdef BOOST_URL_DOCS
    url& set_host( urls::ipv6_address const& addr );
#else
    BOOST_URL_DECL
    url&
    set_host(
        urls::ipv6_address const& addr);
#endif

    /** Set the host

        The host is set to the specified plain
        string, subject to the following rules:

        @li If the string is a valid IPv4 address,
        the address is parsed and the host is set
        as if an instance of the equivalent
        @ref urls::ipv4_address were passed instead.
        In this case @ref url::host_type will return
        @ref host_type::ipv4. Otherwise,

        @li The plain string is percent-encoded and
        the result is set as the reg-name for the
        host. In this case @ref url::host_type will
        return @ref host_type::name.

        In all cases, if an authority did not
        previously exist it is added by prepending
        a double slash ("//") at the beginning of
        the URL or after the scheme if a scheme is
        present.

        @par Postconditions
        @code
        this->encoded_host() == s
        @endcode

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.2">
            3.2.2. Host (rfc3986)</a>

        @return A reference to the object, for chaining.

        @param s The string to set.
    */
    BOOST_URL_DECL
    url&
    set_host(
        string_view s);

    /** Set the host

        The host is set to the specified percent-
        encoded string, subject to the following
        rules:

        @li If the string is a valid IPv4 address,
        the address is parsed and the host is set
        as if an instance of the equivalent
        @ref urls::ipv4_address were passed instead. In
        this case @ref url::host_type will return
        @ref host_type::ipv4. Or,

        @li If the string is a valid IPv6 address
        enclosed in square brackets ('[' and ']'),
        the address is parsed and the host is set
        as if an instance of the equivalent
        @ref urls::ipv6_address were passed instead. In
        this case @ref url::host_type will return
        @ref host_type::ipv4. Or,

        @li If the string is a valid IPvFuture address
        enclosed in square brackets ('[' and ']'),
        the address is parsed and the host is set
        to the specified string. In this case
        @ref url::host_type will return
        @ref host_type::ipvfuture. Or,

        @li If the string is a valid percent-encoded
        string with no characters from the reserved
        character set, then it is set as the encoded
        host name. In this case @ref url::host_type
        will return @ref host_type::name. Otherwise,

        @li If the string does not contain a valid
        percent-encoding for the host field, an
        exception is thrown.

        In all cases, if an authority did not
        previously exist it is added by prepending
        a double slash ("//") at the beginning of
        the URL or after the scheme if a scheme is
        present.
        
        @par Postconditions
        @code
        this->encoded_host() == s
        @endcode

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.2">
            3.2.2. Host (rfc3986)</a>

        @return A reference to the object, for chaining.

        @param s The string to set.

        @throw std::invalid_argument the percent-encoding is invalid
    */
    BOOST_URL_DECL
    url&
    set_encoded_host(string_view s);

private:
    char* set_port_impl(std::size_t n);
public:

    /** Remove the port

        If a port is present, it is removed.
        The remainder of the authority component
        is left unchanged including the leading
        double slash ("//").

        @par Postconditions
        @code
        this->has_port() == false && this->port_number() == 0 && this->port() == ""
        @endcode

        @par Exception Safety
        Does not throw.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.3">
            3.2.3. Port (rfc3986)</a>

        @return A reference to the object, for chaining.
    */
    BOOST_URL_DECL
    url&
    remove_port() noexcept;

    /** Set the port

        The port of the URL is set to the specified
        integer, replacing any previous port.
        If an authority did not
        previously exist it is added by prepending
        a double slash ("//") at the beginning of
        the URL or after the scheme if a scheme is
        present.

        @par Postconditions
        @code
        this->has_port() == true && this->port_number() == n && this->port() == std::to_string(n)
        @endcode

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.3">
            3.2.3. Port (rfc3986)</a>

        @param n The port number to set.

        @return A reference to the object, for chaining.
    */
    BOOST_URL_DECL
    url&
    set_port(std::uint16_t n);

    /** Set the port

        The port of the URL is set to the specified
        string, replacing any previous port. The string
        must meet the syntactic requirements for PORT,
        which consists only of digits. The string
        may be empty. In this case the port is still
        defined, however it is the empty string. To
        remove the port call @ref remove_port.
        If an authority did not
        previously exist it is added by prepending
        a double slash ("//") at the beginning of
        the URL or after the scheme if a scheme is
        present.

        @par Postconditions
        @code
        this->has_port() == true && this->port_number() == n && this->port() == std::to_string(n)
        @endcode

        @par BNF
        @code
        port          = *DIGIT
        @endcode

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.3">
            3.2.3. Port (rfc3986)</a>

        @param s The port string to set.

        @return A reference to the object, for chaining.

        @throw std::invalid_argument `s` is not a valid port string.
    */
    BOOST_URL_DECL
    url&
    set_port(string_view s);

    //--------------------------------------------

    /** Remove the authority

        The full authority component is removed
        if present, which includes the leading
        double slashes ("//"), the userinfo,
        the host, and the port.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2">
            3.2. Authority (rfc3986)</a>

        @return A reference to the object, for chaining.
    */
    BOOST_URL_DECL
    url&
    remove_authority() noexcept;

    /** Set the authority

        This function sets the authority component
        to the specified encoded string. If a
        component was present it is replaced.
        Otherwise, the authority is added
        including leading double slashes ("//").

        The encoded string must be a valid
        authority or else an exception is thrown.

        @par BNF
        @code
        authority     = [ userinfo "@" ] host [ ":" port ]

        userinfo      = *( unreserved / pct-encoded / sub-delims / ":" )
        host          = IP-literal / IPv4address / reg-name
        port          = *DIGIT
        @endcode

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2">
            3.2. Authority (rfc3986)</a>

        @param s The authority string to set.

        @return A reference to the object, for chaining.

        @throw std::invalid_argument `s` is not a valid authority.
    */
    BOOST_URL_DECL
    url&
    set_encoded_authority(
        string_view s);

    //--------------------------------------------

    /** Remove the origin component

        The origin consists of the everything from the
        beginning of the URL up to but not including
        the path.

        @par Exception Safety
        Does not throw.
    */
    BOOST_URL_DECL
    url&
    remove_origin() noexcept;

    //--------------------------------------------
    //
    // Path
    //
    //--------------------------------------------

    /** Set the path.

        Sets the path of the URL to the specified
        encoded string. If this string is empty,
        any existing path is removed.

        The string must meet the syntactic requirements,
        which vary depending on the existing contents
        of the URL:

        @li If an authority is present (@ref has_authority
        returns `true`), the path syntax must match
        <em>path-abempty</em>, else

        @li If the new path starts with a forward
        slash ('/'), the path syntax must match
        <em>path-absolute</em>, else

        @li If a scheme is present, the path syntax
        must match <em>path-rootless</em>, otherwise

        @li The path syntax must match <em>path-noscheme</em>.

        If the path does not meet the syntactic
        requirements, an exception is thrown.

        @par BNF
        @code
        path          = [ "/" ] segment *( "/" segment )
        @endcode

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @param s The string to set.

        @throws std::exception invalid path.

    */
    BOOST_URL_DECL
    url&
    set_encoded_path(
        string_view s);

    /** Return a path segment by index

        This function returns an indexed
        path segment as a percent-encoded
        string. The behavior depends on
        `i`:

        @li If `i` is 0 the first path
        segment is returned;

        @li If `i` is positive, then
        the `i` + 1th path segment is
        returned. For example if `i == 2`
        then the third segment is returned.
        In other words, `i` is zero based.

        @li If `i` is negative, then the
        function negates `i`, and counts from
        the end of the path rather than the
        beginning. For example if `i == -1`
        then the last path segment is returned.

        If the `i` is out of range, an empty
        string is returned. To determine the
        number of segments, call @ref segment_count.

        @par Example
        @code
        url_view u = parse_relative_ref( "/path/to/the/file.txt" );

        assert( u.encoded_segment( -2 ) == "the" );
        assert( u.encoded_segment( -1 ) == "file.txt" );
        assert( u.encoded_segment(  0 ) == "path" );
        assert( u.encoded_segment(  1 ) == "to" );
        @endcode

        @par BNF
        @code
        path          = [ "/" ] segment *( "/" segment )
        @endcode

        @par Exception Safety
        Does not throw.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.3"
            >3.3. Path (rfc3986)</a>

        @param i The index of the segment to return.

        @see
            @ref encoded_path,
            @ref path,
            @ref path_view,
            @ref segment_count.
    */
    BOOST_URL_DECL
    virtual
    string_view
    encoded_segment(
        int i) const noexcept override;

    //--------------------------------------------
    //
    // Query
    //
    //--------------------------------------------

    /** Set the query.

        Sets the query of the URL to the specified
        plain string:

        @li If the string is empty, the query is
        cleared including the leading question
        mark ('?'), otherwise:

        @li If the string is not empty, the query
        is set to the given string, with a leading
        question mark added.
        Any special or reserved characters in the
        string are automatically percent-encoded.

        @par Exception Safety

        Strong guarantee.
        Calls to allocate may throw.

        @param s The string to set. This string may
        contain any characters, including nulls.
    */
    BOOST_URL_DECL
    url&
    set_query(
        string_view s);

    /** Set the query.

        Sets the query of the URL to the specified
        encoded string:

        @li If the string is empty, the query is
        cleared including the leading question mark ('?'),
        otherwise:

        @li If the string is not empty, the fragment
        is set to given string, with a leading question
        mark added.
        The string must meet the syntactic requirements
        of <em>query</em> otherwise an exception is
        thrown.

        @par BNF
        @code
        query         = *( pchar / "/" / "?" )
        @endcode

        @par Exception Safety

        Strong guarantee.
        Calls to allocate may throw.

        @param s The string to set.

        @throws std::exception invalid string.
    */
    BOOST_URL_DECL
    url&
    set_encoded_query(
        string_view s);

    /** Set the query.

        Sets the query of the URL to the specified
        encoded string.

        @li If the string is empty, the query is
        cleared including the leading question
        mark ('?'), otherwise:

        @li If the string is not empty, the query
        is set to given string.
        The string must meet the syntactic requirements
        of <em>query-part</em> otherwise an exception
        is thrown.

        @par BNF
        @code
        query-part    = [ "#" *( pchar / "/" / "?" ) ]
        @endcode

        @par Exception Safety

        Strong guarantee.
        Calls to allocate may throw.

        @param s The string to set.

        @throws std::exception invalid string.
    */
    BOOST_URL_DECL
    url&
    set_query_part(
        string_view s);

    //--------------------------------------------
    //
    // Fragment
    //
    //--------------------------------------------

    /** Set the fragment.

        Sets the fragment of the URL to the specified
        plain string:

        @li If the string is empty, the fragment is
        cleared including the leading hash mark ('#'),
        otherwise:

        @li If the string is not empty, the fragment
        is set to given string, with a leading hash
        mark added.
        Any special or reserved characters in the
        string are automatically percent-encoded.

        @par Exception Safety

        Strong guarantee.
        Calls to allocate may throw.

        @param s The string to set. This string may
        contain any characters, including nulls.

        @see set_encoded_fragment, set_fragment_part
    */
    BOOST_URL_DECL
    url&
    set_fragment(
        string_view s);

    /** Set the fragment.

        Sets the fragment of the URL to the specified
        encoded string:

        @li If the string is empty, the fragment is
        cleared including the leading hash mark ('#'),
        otherwise:

        @li If the string is not empty, the fragment
        is set to given string, with a leading hash
        mark added.
        The string must meet the syntactic requirements
        of <em>fragment</em> otherwise an exception is
        thrown.

        @par BNF
        @code
        fragment      = *( pchar / "/" / "?" )
        @endcode

        @par Exception Safety

        Strong guarantee.
        Calls to allocate may throw.

        @param s The string to set.

        @throws std::exception invalid string.

        @see set_fragment, set_fragment_part
    */
    BOOST_URL_DECL
    url&
    set_encoded_fragment(
        string_view s);

    //--------------------------------------------
    //
    // Normalization
    //
    //--------------------------------------------

    /** Normalize everything.
    */
    BOOST_URL_DECL
    url&
    normalize();

    BOOST_URL_DECL
    url&
    normalize_scheme() noexcept;

private:
    //--------------------------------------------
    //
    // implementation
    //
    //--------------------------------------------

    friend class static_url_base;

    void check_invariants() const noexcept;
    void build_tab() noexcept;

    void
    ensure_space(
        std::size_t nchar,
        std::size_t nseg,
        std::size_t nparam);

    char*
    resize_impl(
        int id,
        std::size_t new_size);

    char*
    resize_impl(
        int first,
        int last,
        std::size_t new_size);
};

//----------------------------------------------------------

/** Format the encoded url to the output stream
*/
BOOST_URL_DECL
std::ostream&
operator<<(std::ostream& os, url const& u);

} // urls
} // boost

#endif
