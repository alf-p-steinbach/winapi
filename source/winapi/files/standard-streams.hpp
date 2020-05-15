#pragma once    // Source encoding: UTF-8 with BOM (π is a lowercase Greek "pi").
#include <cppx-core-language/all.hpp>

#include <winapi-header-wrappers/windows-h.hpp>

#include <array>

namespace winapi::files {
    $use_std( array );

    struct Std_stream { enum Enum{ input, output, error, _ }; };
    constexpr int n_std_streams = Std_stream::_;

    inline auto stream( const Std_stream::Enum id )
        -> HANDLE
    {
        static const array<HANDLE, n_std_streams> the_streams =
        {
            ::GetStdHandle( STD_INPUT_HANDLE ),
            ::GetStdHandle( STD_OUTPUT_HANDLE ),
            ::GetStdHandle( STD_ERROR_HANDLE )
        };

        return the_streams.at( id );
    }

    inline auto is_connected( const HANDLE stream )
        -> bool
    {
        DWORD dummy;
        if( ::GetConsoleMode( stream, &dummy ) ) {
            return true;
        }
        switch( ::GetFileType( stream ) ) {
            case FILE_TYPE_DISK:
            case FILE_TYPE_PIPE: {
                return true;
            }
        }
        return false;       // E.g. stream is redirected to `nul`.
    }
}  // namespace winapi::files
