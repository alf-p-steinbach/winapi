#pragma once    // Source encoding: UTF-8 with BOM (π is a lowercase Greek "pi").

#include <winapi/graphics/the_default_gui_font.hpp>
#include <winapi/gui/Subclassed_window.hpp>

namespace winapi::gui {

    class Displayable_window
        : public Subclassed_window
    {
        using Base = Subclassed_window;

        virtual void paint( const PAINTSTRUCT& paint_info )
        {
            $is_unused( paint_info );
        }

    protected:
        void on_wm_paint( )
        {
            struct Envelope
            {
                HWND            window;
                PAINTSTRUCT     info;

                ~Envelope() { ::EndPaint( window, &info ); }

                explicit Envelope( const HWND w ):
                    window( w )
                { ::BeginPaint( window, &info ) or $fail( "BeginPaint failed" ); }
            };

            Envelope envelope( handle() );
            paint( envelope.info );
        }

        auto on_message( const Message& m )
            -> LRESULT override
        {
            #if 0
            switch( m.message_id ) {
                WINAPI_CASE_WM( PAINT, m, on_wm_paint );
            }
            #endif

            return Base::on_message( m );
        }

        Displayable_window( Window_owner_handle window_handle )
            : Base( move( window_handle ) )
        {
            set_default_font( the_default_gui_font() );
        }

    public:
        auto font() const
            -> HFONT
        {
            return reinterpret_cast<HFONT>(
                SendMessage( *this, WM_GETFONT, WPARAM(), LPARAM() )
                );
        }

        void set_default_font( const HFONT font )
        {
            process_message( WM_SETFONT, reinterpret_cast<WPARAM>( font ) );
        }
    };
}  // namespace winapi::gui
