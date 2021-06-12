#pragma once    // Source encoding: UTF-8 with BOM (π is a lowercase Greek "pi").
#include <cppx-core-language/all.hpp>

#include <winapi-header-wrappers/windows-h.hpp>
#include <winapi/gui/top-level-windows/Main_window.hpp>     // winapi::Main_window

#include <vector>

namespace winapi::gui {
    $use_std( vector );

    namespace impl {
        auto inline CALLBACK emwit_callback(
            const HWND      window,
            const LPARAM    param
            ) -> BOOL
        {
            auto& handles = *reinterpret_cast<vector<HWND>*>( param );
            handles.push_back( window );
            return true;
        }
    }  // namespace impl

    inline auto enabled_main_window_in_thread()
        -> HWND
    {
        vector<HWND> handles;
        ::EnumThreadWindows(
            GetCurrentThreadId(),
            &impl::emwit_callback,
            reinterpret_cast<LPARAM>( &handles )
            );
        if( handles.size() == 1 and ::IsWindowEnabled( handles.front() ) ) {
            return handles.front();
        }
        for( const HWND window : handles ) {
            if( ::IsWindowEnabled( window ) ) {
                char name[128];
                const int n_chars = ::GetClassName( window, name, sizeof( name ) );
                if( n_chars > 0 ) {
                    if( strcmp( name, gui::Main_window::windowclass_name ) == 0 ) {
                        return window;
                    }
                }
            }
        }
        return HWND();
    }

}  // namespace winapi::gui
