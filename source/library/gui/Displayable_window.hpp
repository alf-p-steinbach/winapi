#pragma once    // Source encoding: UTF-8 with BOM (π is a lowercase Greek "pi").

#include <c/stdint.hpp>         // intptr_t

#include <winapi/graphics/the_default_gui_font.hpp>
#include <winapi/gui/Subclassed_window.hpp>
#include <winapi/gui/windowing-support/Windowclass_id.hpp>

#include <typeindex>

namespace winapi::gui {
    $use_cppx( Map_ );
    $use_std( type_index );

    class Displayable_window
        : public Subclassed_window
    {
    public:
        using Base = Subclassed_window;

    protected:
        virtual void paint( const PAINTSTRUCT& paint_info )
        {
            $is_unused( paint_info );
        }

        class Window_class
        {
        protected:
            static auto sys_color_brush( const intptr_t system_color_id )
                -> HBRUSH
            { return reinterpret_cast<HBRUSH>( system_color_id + 1 ); }

            virtual void override_values( WNDCLASS& params ) const
            {
                $is_unused( params );
            }

            auto create() const
                -> Windowclass_id
            {
                WNDCLASS params = {};
                params.style            = CS_DBLCLKS;
                params.lpfnWndProc      = &::DefWindowProc;
                params.hInstance        = ::GetModuleHandle( nullptr );
                params.hIcon            = ::LoadIcon( 0, IDI_APPLICATION );
                params.hCursor          = ::LoadCursor( 0, IDC_ARROW );
                params.hbrBackground    = sys_color_brush( COLOR_WINDOW );
                params.lpszClassName    = "";       // Derived class responsibility.

                override_values( params );

                ATOM const result = ::RegisterClass( &params );
                hopefully( result != 0 )
                    or $fail( "RegisterClass failed" );
                return result;
            }

        public:
            auto id() const
                -> Windowclass_id
            {
                struct Workaround
                {
                    Windowclass_id  value;

                    Workaround(): value( "" ) {}        // Needed for unordered_map item type.
                    Workaround( Windowclass_id&& v ): value( v ) {}
                };

                static Map_<type_index, Workaround> the_ids;
                
                const auto cpp_id = type_index( typeid( *this ) );
                if( the_ids.count( cpp_id ) == 0 ) {
                    the_ids.insert( {cpp_id, Workaround( create() )} );
                }
                return the_ids[cpp_id].value;
            }
        };

        class Api_window_factory
        {
        public:
            using Window_style = decltype( CREATESTRUCT::style );               // E.g. LONG
            using Window_extended_style = decltype( CREATESTRUCT::dwExStyle );  // E.g. DWORD

            virtual auto windowclass() const
                -> Windowclass_id = 0;
            // return Window_class().id();

            virtual auto fixed_window_style() const
                -> Window_style = 0;

            virtual auto fixed_window_extended_style() const
                -> Window_extended_style
            { return 0; }

            virtual auto fixed_creation_params() const
                -> CREATESTRUCT
            {
                CREATESTRUCT params = {};
                params.hInstance    = GetModuleHandle( nullptr );
                params.cy           = CW_USEDEFAULT;
                params.cx           = CW_USEDEFAULT;
                params.y            = CW_USEDEFAULT;
                params.x            = CW_USEDEFAULT;
                params.style        = fixed_window_style();
                params.lpszName     = "";
                params.lpszClass    = windowclass().pseudo_ptr();
                params.dwExStyle    = fixed_window_extended_style();
                return params;
            }

            virtual void fail_if_obviously_wrong( const CREATESTRUCT& params ) const
            { $is_unused( params ); }  // Signals ungood params by throwing.
        };

        virtual void close()
        {
            DestroyWindow( *this );
        }

        void on_wm_close()
        {
            close();
        }

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
            switch( m.message_id ) {
                #if 0
                    WINAPI_CASE_WM( PAINT, m, on_wm_paint );
                #endif
                WINAPI_CASE_WM( CLOSE, m, on_wm_close );
            }

            return Base::on_message( m );
        }

        Displayable_window( tag::Wrap, Window_owner_handle window_handle )
            : Base( tag::Wrap(), move( window_handle ) )
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
