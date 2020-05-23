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

    class Child_window
        : public Displayable_window
    {
        using Base = Displayable_window;

    public:
        static constexpr auto& windowclass_name = "Child-window";

    protected:
        class Window_class:
            public Base::Window_class
        {
        protected:
            void override_values( WNDCLASS& params ) const
                override
            { params.lpszClassName = windowclass_name; }
        };

         class Api_window_factory
            : public Base::Api_window_factory
        {
        public:
            auto windowclass() const
                -> Windowclass_id override
            { return Window_class().id(); }

            auto fixed_window_style() const
                -> Window_style override
            { return WS_CHILD | WS_CLIPSIBLINGS; }

            auto new_api_window( const HWND parent_window )
                -> Window_owner_handle
            {
                CREATESTRUCT params = fixed_creation_params();
                params.hwndParent = parent_window;
                return create_window( params );
            }
        };  // class Api_window_factory

    public:
        void set_text( const C_str text )
        {
            ::SetWindowText( *this, text )
                or $fail( "SetWindowText failed" );
        }

        Child_window( const HWND parent_window ):
            Base( tag::Wrap(), Api_window_factory().new_api_window( parent_window ) )
        {}

        Child_window( tag::Wrap, Window_owner_handle window_handle ):
            Base( tag::Wrap(), move( window_handle ) )
        {}
    };

}  // namespace winapi::gui
