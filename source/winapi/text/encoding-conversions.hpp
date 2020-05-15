#pragma once    // Source encoding: UTF-8 with BOM (π is a lowercase Greek "pi").
#include <cppx-core-language/all.hpp>

#include <winapi-header-wrappers/windows-h.hpp>

#include <c/assert.hpp>

#include <string>
#include <string_view>

namespace winapi::text {
    using namespace cppx::x_throwing;
    $use_std( string, string_view, wstring, wstring_view );

    inline auto to_wide( const string_view& s )
        -> wstring
    {
        const int n_bytes = int( s.length() );
        if( n_bytes == 0 ) {
            return L"";
        }
        const DWORD flags = 0;
        const int buffer_size = ::MultiByteToWideChar( CP_UTF8, flags, s.data(), n_bytes, nullptr, 0 );
        hopefully( buffer_size > 0 )
            or $fail( "MultiByteToWideChar failed" );
        wstring result( buffer_size, L'\0' );
        const int wide_length = ::MultiByteToWideChar( CP_UTF8, flags, s.data(), n_bytes, result.data(), buffer_size );
        assert( wide_length > 0 );
        result.resize( wide_length );
        return result;
    }
    
}  // namespace winapi::text
