﻿#pragma once    // Source encoding: UTF-8 with BOM (π is a lowercase Greek "pi").
#include <winapi/gui/Top_level_window.hpp>

namespace winapi::gui {

    class Main_window
        : public Extends_<Top_level_window>
    {
    public:
        static constexpr auto& windowclass_name = "Main-window";

    protected:
        class Window_class:
            public Extends_<Base_::Window_class>
        {
        protected:
            void override_values( WNDCLASS& params ) const
                override
            { params.lpszClassName = windowclass_name; }
        };

        class Api_window_factory
            : public Extends_<Base_::Api_window_factory>
        {
        public:
            auto windowclass() const
                -> Windowclass_id override
            { return Window_class().id(); }
        };  // class Api_window_factory

        void on_wm_destroy()
        {
            ::PostQuitMessage( 0 );
        }

        auto on_message( const Message& m )
            -> LRESULT override
        {
            switch( m.message_id ) {
                case WM_DESTROY:    return WINAPI_HANDLE_WM( DESTROY, m, on_wm_destroy );
            }
            return Base_::on_message( m );
        }

    public:
        Main_window( const C_str title = "<untitled>" ):
            Base_( tag::Wrap(), Api_window_factory().new_api_window( title ) )
        {}

        Main_window( tag::Wrap, Window_owner_handle window_handle ):
            Base_( tag::Wrap(), move( window_handle ) )
        {}
    };

}  // namespace winapi::gui
