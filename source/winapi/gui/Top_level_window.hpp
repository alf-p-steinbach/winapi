﻿#pragma once    // Source encoding: UTF-8 with BOM (π is a lowercase Greek "pi").
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

    class Top_level_window
        : public Displayable_window
    {
        using Base = Displayable_window;

    public:
        static constexpr auto& windowclass_name = "Top-level-window";

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
            { return WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN; }

            auto new_api_window( const C_str title )
                -> Window_owner_handle
            {
                CREATESTRUCT params = fixed_creation_params();
                params.lpszName = title;
                return create_window( params );
            }
        };  // class Api_window_factory

    public:
        void set_title( const C_str title )
        {
            ::SetWindowText( *this, title )
                or $fail( "SetWindowText failed" );
        }

        Top_level_window( const C_str title = "<untitled>" ):
            Base( tag::Wrap(), Api_window_factory().new_api_window( title ) )
        {}

        Top_level_window( tag::Wrap, Window_owner_handle window_handle ):
            Base( tag::Wrap(), move( window_handle ) )
        {}
    };

}  // namespace winapi::gui
