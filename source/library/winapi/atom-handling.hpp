#pragma once    // Source encoding: UTF-8 with BOM (π is a lowercase Greek "pi").
#include <winapi-header-wrappers/windows-h.hpp>
#include <cppx-core-language/all.hpp>

#include <c/assert.hpp>     // assert
#include <c/stdint.hpp>     // intptr_t

#include <stdexcept>        // std::logic_error
#include <string>           // std::(string, to_string)

namespace winapi {
    using namespace cppx::x_throwing;       // hopefully, fail
    $use_cppx( Type_, C_str );
    $use_std( logic_error, to_string, string );

    inline auto pseudo_ptr_from( const ATOM a )
        -> C_str
    { return MAKEINTATOM( a ); }

    inline auto string_from_atom( const ATOM a )
        -> string
    { return "#" + to_string( a ); }

    // A pseudo-pointer is an atom if it's zero except for the lowest 16 bits.
    inline auto is_atom( const C_str pseudo_ptr )
        -> bool
    { return IS_INTRESOURCE( pseudo_ptr ); }

    inline auto as_atom( const C_str pseudo_ptr )
        -> ATOM
    {
        assert( is_atom( pseudo_ptr ) );
        hopefully( is_atom( pseudo_ptr ) )
            or fail_<logic_error>( "winapi::atom_value_of" );
        return static_cast<ATOM>( reinterpret_cast<uintptr_t>( pseudo_ptr ) );
    }
}  // namespace winapi

