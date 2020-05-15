#pragma once    // Source encoding: UTF-8 with BOM (π is a lowercase Greek "pi").

#ifndef     _MSC_VER
#   error   "This header is for the Visual C++ compiler only."
#endif

#include <cppx-core-language/all.hpp>
#include <winapi-header-wrappers/windows-h.hpp>
#include <winapi/files/standard-streams.hpp>            // winapi::files::*

#include <process.h>            // Microsoft - _set_app_type

namespace winapi::msvc::runtime_library {

    inline void fix_assertion_reporting()
    {
        // Prevent assertion text being sent to The Big Bit Bucket In The Sky™.
        $use_from_namespace( winapi::files,
            is_connected, stream, Std_stream
        );
        _set_app_type( _crt_app_type( 0?0
            : is_connected( stream( Std_stream::error ) )?  _crt_console_app
            :                                               _crt_gui_app
        ) );
    }

    class  Envelope
    {
        Envelope()
        {
            fix_assertion_reporting();
        }

    public:
        static void global_instantiation()
        {
            static Envelope the_envelope;
        };
    };

}  // namespace winapi::msvc::runtime_library
