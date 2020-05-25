#pragma once    // Source encoding: UTF-8 with BOM (π is a lowercase Greek "pi").
#include <cppx-core-language/all.hpp>

#include <stdexcept>

#include <winapi/atom-handling.hpp>     // winapi::(is_atom, as_atom, string_from_atom, pseudo_ptr_from)

namespace winapi
{
    $use_cppx( Raw_array_of_, Size );
    $use_std( logic_error );

    class Hybrid_little_id
    {
        const char*     m_pseudo_ptr;

    public:
        auto pseudo_ptr() const
            -> const char*
        { return m_pseudo_ptr; }

        auto is_atom() const
            -> bool
        { return winapi::is_atom( m_pseudo_ptr ); }

        auto to_atom() const
            -> ATOM
        { return as_atom( m_pseudo_ptr ); }

        auto str() const
            -> string
        { return (is_atom()? string_from_atom( to_atom() ) : m_pseudo_ptr); }

        template< Size n >
        Hybrid_little_id( const Raw_array_of_<n, const char>& a )
            : m_pseudo_ptr( a )
        {}

        Hybrid_little_id( const ATOM a )
            : m_pseudo_ptr( pseudo_ptr_from( a ) )
        {}
    };

}  // namespace winapi
