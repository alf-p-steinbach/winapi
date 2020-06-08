#pragma once    // Source encoding: UTF-8 with BOM (π is a lowercase Greek "pi").
#include <cppx-core-language/all.hpp>

#include <cv-win/Image_displayer.hpp>
#include <winapi/gui/Control.hpp>

#include <memory>       // std::shared_ptr

namespace winapi::gui {
    $use_std( shared_ptr );
    using cv_win::Abstract_image_displayer;

    class Image_display_control:
        public Extends_<Control>,
        protected Wm_paint_processing
    {
    public:
        static constexpr auto& windowclass_name = "Image-display-control";

    private:
        shared_ptr<const Abstract_image_displayer>  m_image_displayer;

        void paint( const PAINTSTRUCT& paint_info )
            override
        {
            const HDC dc = paint_info.hdc;
            m_image_displayer->display_on( dc );
        }

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

        auto on_message( const Message& m )
            -> LRESULT override
        {
            switch( m.message_id ) {
                case WM_PAINT: return process_wm_paint( handle(), m.word_param, m.long_param );
            }
            return Base_::on_message( m );
        }

    public:
        Image_display_control(
            const Type_<Displayable_window*>                    parent,
            const shared_ptr<const Abstract_image_displayer>    displayer,
            const POINT&                                        position,
            const SIZE&                                         size
            ):
            Base_( tag::Wrap(), Api_window_factory().new_api_window(
                parent->handle(),
                position,
                size
                ) ),
            m_image_displayer( displayer )
        {}

        Image_display_control(
            const Type_<Displayable_window*>                    parent,
            const shared_ptr<const Abstract_image_displayer>    displayer,
            const POINT&                                        position    = {}
            ):
            Image_display_control(
                parent, displayer, position, SIZE{ displayer->width(), displayer->height() }
                )
        {}

        Image_display_control(
            tag::Wrap, Window_owner_handle                      window_handle,
            const shared_ptr<const Abstract_image_displayer>    displayer
            ):
            Base_( tag::Wrap(), move( window_handle ) ),
            m_image_displayer( displayer )
        {}
    };

}  // namespace winapi::gui
