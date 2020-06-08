#pragma once    // Source encoding: UTF-8 with BOM (π is a lowercase Greek "pi").
#include <cppx-core-language/all.hpp>

#include <string_view>
#include <tuple>            // std::tie
#include <utility>          // std::move

#include <winapi/gui/Displayable_window.hpp>
#include <winapi/gui/windowing-support/create_window.hpp>
#include <winapi-header-wrappers/commctrl-h.hpp>        // HANDLE_WM_NOTIFY

namespace winapi::gui {
    using namespace cppx::x_throwing;
    $use_cppx( C_str, Extends_, Type_ );
    $use_std( move );

    class Top_level_window:
        public Extends_<Displayable_window>
    {
    public:
        static constexpr auto& windowclass_name = "Top-level-window";

    protected:
        Top_level_window( tag::Wrap, Window_owner_handle window_handle ):
            Base_( tag::Wrap(), move( window_handle ) )
        {}

        class Window_class:
            public Extends_<Base_::Window_class>
        {
        protected:
            void override_values( WNDCLASS& params ) const
                override
            { params.lpszClassName = windowclass_name; }
        };

        class Api_window_factory:
            public Extends_<Base_::Api_window_factory>
        {
        public:
            auto windowclass() const
                -> Windowclass_id override
            { return Window_class().id(); }

            auto fixed_window_style() const
                -> Window_style override
            { return WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN; }

            void fail_if_obviously_wrong( const CREATESTRUCT& params ) const
                override
            {
                Base_::fail_if_obviously_wrong( params );
                hopefully( (params.style & WS_CHILD) == 0 )
                    or $fail( "A top level window can't have the WS_CHILD style." );
                hopefully( (params.style & WS_CLIPSIBLINGS) == 0 )
                    or $fail( "The WS_CLIPSIBLINGS style is not meaningful for a top level window." );
                static_assert( WS_TABSTOP == WS_MAXIMIZEBOX );  // I.e. don't err on use of that style.
                static_assert( WS_GROUP == WS_MINIMIZEBOX );    // I.e. don't err on use of that style.
            }

            auto new_api_window( const C_str title )
                -> Window_owner_handle
            {
                CREATESTRUCT params = fixed_creation_params();
                params.lpszName = title;
                fail_if_obviously_wrong( params );
                return create_window( params );
            }
        };  // class Api_window_factory

    public:
        Top_level_window( const C_str title = "<untitled>" ):
            Base_( tag::Wrap(), Api_window_factory().new_api_window( title ) )
        {}

        void set_title( const C_str title )
        {
            ::SetWindowText( *this, title )
                or $fail( "SetWindowText failed" );
        }
    };

}  // namespace winapi::gui
