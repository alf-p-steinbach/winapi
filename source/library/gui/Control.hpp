#pragma once    // Source encoding: UTF-8 with BOM (π is a lowercase Greek "pi").
#include <cppx-core-language/all.hpp>

#include <winapi/gui/Child_window.hpp>

namespace winapi::gui {

    class Control:
        public Extends_<Child_window>
    {
    public:
        static constexpr auto& windowclass_name = "Control";

    protected:
        Control( tag::Wrap, Window_owner_handle window_handle ):
            Base_( tag::Wrap(), move( window_handle ) )
        {}

        class Window_class:
            public Extends_<Base_::Window_class>
        {
        protected:
            void override_values( WNDCLASS& params ) const
                override
            {
                Base_::override_values( params );

                params.hbrBackground    = sys_color_brush( COLOR_3DFACE );
                params.lpszClassName    = windowclass_name;
            }
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
            { return Base_::fixed_window_style() & ~WS_CLIPSIBLINGS; }

            void fail_if_obviously_wrong( const CREATESTRUCT& params ) const
                override
            {
                Base_::fail_if_obviously_wrong( params );

                const auto ws_title = WS_CAPTION & ~WS_BORDER;  // A.k.a. WS_DLGFRAME.
                hopefully( (params.style & ws_title) == 0 )
                    or $fail( "A control should not have a title bar (WS_CAPTION style)." );
            }
        };

    public:
        Control( const Type_<Displayable_window*> p_parent, const POINT& position, const SIZE& size ):
            Base_( tag::Wrap(), Api_window_factory().new_api_window( p_parent->handle(), position, size ) )
        {}
    };

}  // namespace winapi::gui
