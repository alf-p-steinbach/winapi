#pragma once    // Source encoding: UTF-8 with BOM (π is a lowercase Greek "pi").
#include <cppx-core-language/all.hpp>

#include <winapi/gui/windowing-support/Window_owner_handle.hpp>

namespace winapi::gui {
    using namespace cppx::x_throwing;

    inline auto create_window( const CREATESTRUCT& params )
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
