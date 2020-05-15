#pragma once    // Source encoding: UTF-8 with BOM (π is a lowercase Greek "pi").
#include <cppx-core-language/all.hpp>

#include <string_view>
#include <tuple>            // std::tie
#include <utility>          // std::move

#include <winapi/gui/Api_window_factory.hpp>
#include <winapi/gui/Displayable_window.hpp>

namespace winapi::gui {
    using namespace cppx::x_throwing;
    $use_cppx( C_str, Type_ );
    $use_std( move );

    class Top_level_window
        : public Displayable_window
    {
        using Base = Displayable_window;

    public:
        static constexpr auto& class_name = "Top-level-window";

    protected:
        static auto register_window_class( const C_str name, const HICON icon )
            -> Windowclass_id
        {
            WNDCLASS params = {};
            params.style            = CS_DBLCLKS;
            params.lpfnWndProc      = &::DefWindowProc;
            params.hInstance        = ::GetModuleHandle( nullptr );
            params.hIcon            = icon;
            params.hCursor          = ::LoadCursor( 0, IDC_ARROW );
            params.hbrBackground    = reinterpret_cast<HBRUSH>( COLOR_WINDOW + 1 );
            params.lpszClassName    = name;

            ATOM const result = ::RegisterClass( &params );
            hopefully( result != 0 )
                or $fail( "RegisterClass failed" );
            return result;
        }

        class Api_window_factory
            : public gui::Api_window_factory
        {
        public:
            virtual auto windowclass_name() const
                -> C_str
            { return "Top-level-window"; }

            virtual auto windowclass_icon() const
                -> HICON
            { return ::LoadIcon( 0, IDI_APPLICATION ); }

            auto windowclass() const
                -> Windowclass_id
                override
            {
                static Windowclass_id const the_id = register_window_class(
                    windowclass_name(),
                    windowclass_icon()
                    );
                return the_id;
            }

            auto fixed_window_style() const
                -> Window_style
                override
            { return WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN; }

            auto new_api_window( const C_str title )
                -> Window_owner_handle
            {
                CREATESTRUCT params = fixed_creation_params();
                params.lpszName = title;
                return new_api_window_with( params );
            }
        };  // class Api_window_factory

        void on_wm_close()
        {
            DestroyWindow( *this );
        }

        auto on_message( const Message& m )
            -> LRESULT override
        {
            switch( m.message_id ) {
                WINAPI_CASE_WM( CLOSE, m, on_wm_close );
            }
            return Base::on_message( m );
        }

    public:
        void set_title( const C_str title )
        {
            ::SetWindowText( *this, title )
                or $fail( "SetWindowText failed" );
        }

        Top_level_window( const C_str title = "<untitled>" ):
            Base( Api_window_factory().new_api_window( title ) )
        {}

        Top_level_window( Window_owner_handle window_handle ):
            Base( move( window_handle ) )
        {}
    };

}  // namespace winapi::gui
