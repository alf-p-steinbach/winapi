#pragma once    // Source encoding: UTF-8 with BOM (π is a lowercase Greek "pi").
#include <cppx-core-language/all.hpp>

#include <winapi/gui/Child_window.hpp>

// This is a comment.
namespace winapi::gui {

    class Control:
        public Child_window
    {
    public: 
        using Base = Child_window;
        static constexpr auto& windowclass_name = "Control";

    protected:

        class Window_class:
            public Base::Window_class
        {
        protected:
            void override_values( WNDCLASS& params ) const
                override
            {
                const auto bg_color = ::GetSysColor( COLOR_3DFACE );
                params.hbrBackground    = sys_color_brush( COLOR_3DFACE );
                params.lpszClassName    = windowclass_name;
            }

        public:
            Window_class() { m_outer = "Control"; }
        };

        class Api_window_factory:
            public Base::Api_window_factory
        {
        public:
            using Base = Control::Base::Api_window_factory;

            auto windowclass() const
                -> Windowclass_id override
            { return Window_class().id(); }

            void fail_if_not_ok( const CREATESTRUCT& params ) const
                override
            {
                Base::fail_if_not_ok( params );

                const auto ws_title = WS_CAPTION & ~WS_BORDER;  // A.k.a. WS_DLGFRAME.
                hopefully( (params.style & ws_title) == 0 )
                    or $fail( "A control should not have a title bar (WS_CAPTION style)." );
            }
        };

    public:
        Control( const Type_<Displayable_window*> p_parent, const POINT& position, const POINT& size ):
            Base( tag::Wrap(), Api_window_factory().new_api_window( p_parent->handle(), position, size ) )
        {}

        Control( tag::Wrap, Window_owner_handle window_handle ):
            Base( tag::Wrap(), move( window_handle ) )
        {}
    };

}  // namespace winapi::gui
