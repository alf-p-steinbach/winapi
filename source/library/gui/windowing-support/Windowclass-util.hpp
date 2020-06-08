#pragma once    // Source encoding: UTF-8 with BOM (π is a lowercase Greek "pi").

#include <cppx-core-language/all.hpp>

#include <winapi-header-wrappers/windows-h.hpp>

#include <c/limits.hpp>     // MAXINT
#include <string>           // std::string

namespace winapi::gui {
    $use_cppx( hopefully );
    $use_std( string );

    inline auto windowclass_name_of( const HWND window )
        -> string
    {
        int bufsize = 32;
        for( ;; ) {
            auto result = string( bufsize, '\0' );
            const int n_bytes = ::GetClassName( window, result.data(), bufsize );
            hopefully( n_bytes > 0 )
                or $fail( "::GetClassName failed" );
            if( n_bytes < bufsize ) {
                result.resize( n_bytes - 1 );
                return result;
            }
            hopefully( bufsize <= MAXINT/2 )
                or $fail( "Unreasonably long windowclass-name." );
            bufsize *= 2;
        } 
    }

}  // namespace winapi::gui
