#pragma once    // Source encoding: UTF-8 with BOM (π is a lowercase Greek "pi").

#include <winapi/gui/Control.hpp>
#include <winapi-header-wrappers/commctrl-h.hpp>

namespace winapi::gui {

    class Trackbar_control:
        public Extends_<Control>
    {
    protected:
        class Api_window_factory:
            public Extends_<Base_::Api_window_factory>
        {
        public:
            auto windowclass() const
                -> Windowclass_id override
            { return TRACKBAR_CLASS; }
        };

    public:
        Trackbar_control( const Type_<Displayable_window*> p_parent, const POINT& position, const SIZE& size ):
            Base_( tag::Wrap(), Api_window_factory().new_api_window( p_parent->handle(), position, size ) )
        {}

        Trackbar_control( tag::Wrap, Window_owner_handle window_handle ):
            Base_( tag::Wrap(), move( window_handle ) )
        {}

    };

}  // namespace winapi::gui
