#pragma once    // Source encoding: UTF-8 with BOM (π is a lowercase Greek "pi").
#include <cppx-core-language/all.hpp>

#include <winapi/gui/Control.hpp>

#include <utility>

namespace winapi::gui {

    class Image_display_control:
        public Extends_<Control>
    {
    public:
        static constexpr auto& windowclass_name = "Image-display-control";

    protected:
        class Window_class:
            public Extends_<Base_::Window_class>
        {
        protected:
            void override_values( WNDCLASS& params ) const
                override
            {
                Base_::override_values( params );
                params.lpszClassName = windowclass_name;
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
        Image_display_control( const Type_<Displayable_window*> p_parent, const POINT& position, const POINT& size ):
            Base_( tag::Wrap(), Api_window_factory().new_api_window( p_parent->handle(), position, size ) )
        {}

        Image_display_control( tag::Wrap, Window_owner_handle window_handle ):
            Base_( tag::Wrap(), move( window_handle ) )
        {}
    };

}  // namespace winapi::gui
