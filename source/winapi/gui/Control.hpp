#pragma once




#pragma once    // Source encoding: UTF-8 with BOM (π is a lowercase Greek "pi").
#include <cppx-core-language/all.hpp>

#include <string_view>
#include <tuple>            // std::tie
#include <utility>          // std::move

#include <winapi/gui/Displayable_window.hpp>
#include <winapi/gui/windowing-support/create_window.hpp>

namespace winapi::gui {
    using namespace cppx::x_throwing;
    $use_cppx( C_str, Type_ );
    $use_std( move );

    class Control:
        public Child_window
    {
        using Base = Child_window;

    public:
        static constexpr auto& windowclass_name = "Control";

    protected:

        class Window_class:
            public Base::Window_class
        {
        protected:
            void override_values( WNDCLASS& params ) const
                override
            {
                params.hbrBackground    = sys_color_brush( COLOR_3DFACE );
                params.lpszClassName    = windowclass_name;
            }
        };

        class Api_window_factory
            : public Base::Api_window_factory
        {
        public:
            auto windowclass() const
                -> Windowclass_id override
            { return Window_class().id(); }
        };

    public:
        Control( const HWND parent_window ):
            Base( tag::Wrap(), Api_window_factory().new_api_window( parent_window ) )
        {}

        Control( tag::Wrap, Window_owner_handle window_handle ):
            Base( tag::Wrap(), move( window_handle ) )
        {}
    };

}  // namespace winapi::gui
