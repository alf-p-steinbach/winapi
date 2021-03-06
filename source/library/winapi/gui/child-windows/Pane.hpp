﻿#pragma once    // Source encoding: UTF-8 with BOM (π is a lowercase Greek "pi").
#include <cppx-core-language/all.hpp>

#include <winapi/gui/Child_window.hpp>

namespace winapi::gui {
    using namespace cppx::x_throwing;

    class Pane:
        public Extends_<Child_window>
    {
    protected:
        Pane( tag::Wrap, Window_owner_handle window_handle ):
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
        };

    public:
        Pane(
            const Type_<Displayable_window*>    parent,
            const POINT&                        position,
            const SIZE&                         size
            ):
            Base_( tag::Wrap(), Api_window_factory().new_api_window( parent->handle(), position, size ) )
        {}
    };

}  // namespace winapi::gui
