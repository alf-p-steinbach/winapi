#pragma once    // Source encoding: UTF-8 with BOM (π is a lowercase Greek "pi").
#include <cppx-core-language/all.hpp>

#include <winapi/gui/windowing-support/Window_owner_handle.hpp>
#include <winapi/gui/windowing-support/Windowclass_id.hpp>

namespace winapi::gui {

    class Api_window_factory
    {
    public:
        using Window_style = decltype( CREATESTRUCT::style );               // E.g. LONG
        using Window_extended_style = decltype( CREATESTRUCT::dwExStyle );  // E.g. DWORD

        virtual auto windowclass() const
            -> Windowclass_id = 0;

        virtual auto fixed_window_style() const
            -> Window_style
        { return 0; }

        virtual auto fixed_window_extended_style() const
            -> Window_extended_style
        { return 0; }

        virtual auto fixed_creation_params() const
            -> CREATESTRUCT
        {
            CREATESTRUCT params = {};
            params.hInstance    = GetModuleHandle( nullptr );
            params.cy           = CW_USEDEFAULT;
            params.cx           = CW_USEDEFAULT;
            params.y            = CW_USEDEFAULT;
            params.x            = CW_USEDEFAULT;
            params.style        = fixed_window_style();
            params.lpszName     = "";
            params.lpszClass    = windowclass().pseudo_ptr();
            params.dwExStyle    = fixed_window_extended_style();
            return params;
        }
    };

    inline auto new_api_window_with( const CREATESTRUCT& params )
        -> Window_owner_handle
    {
        const HWND result = CreateWindowEx(
            params.dwExStyle, params.lpszClass, params.lpszName, params.style,
            params.x, params.y, params.cx, params.cy,
            params.hwndParent, params.hMenu, params.hInstance, params.lpCreateParams
        );
        hopefully( result != 0 )
            or $fail( "CreateWindowEx failed" );
        return Window_owner_handle( result );
    }

}  // namespace winapi::gui
