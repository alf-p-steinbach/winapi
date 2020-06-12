#pragma once    // Source encoding: UTF-8 with BOM (π is a lowercase Greek "pi").

#include <c/stdint.hpp>         // intptr_t

#include <winapi/graphics/the_default_gui_font.hpp>
#include <winapi/gui/Subclassed_window.hpp>
#include <winapi/gui/windowing-support/Windowclass_id.hpp>
#include <winapi/gui/windowing-support/Windowclass-util.hpp>        // windoclass_name_of

#include <optional>             // std::optional
#include <typeindex>            // std::typeindex

namespace winapi::gui {
    $use_cppx( Extends_, Map_, is_zero );
    $use_std( optional, type_index );

    class Wm_paint_handler
    {
        virtual void paint( const PAINTSTRUCT& paint_info ) = 0;

        void on_classic_wm_paint( const HWND window_handle )
        {
            struct Envelope
            {
                HWND            m_window_handle;
                PAINTSTRUCT     m_info;

                ~Envelope() { ::EndPaint( m_window_handle, &m_info ); }

                explicit Envelope( const HWND w ):
                    m_window_handle( w )
                { ::BeginPaint( m_window_handle, &m_info ) or $fail( "BeginPaint failed" ); }
            };

            Envelope envelope( window_handle );
            paint( envelope.m_info );
        }

    protected:
        auto process_wm_paint( const HWND window_handle, const WPARAM word_param, const LPARAM long_param )
            -> LRESULT
        {
            $is_unused( word_param );  $is_unused( long_param );    // word_param can indicate custom painting.
            on_classic_wm_paint( window_handle );
            return 0;
        }
    };

    struct Scroll_direction{ enum Enum{ none, horizontal, vertical }; };

    class Scroll_event_handler
    {
        friend class Displayable_window;    // Caller.

        virtual void on_scroll(
            const Scroll_direction::Enum    direction,
            const UINT                      code,
            const int                       pos
            ) = 0;
    };

    class Displayable_window:
        public Extends_<Subclassed_window>
    {
    protected:
        Displayable_window( tag::Wrap, Window_owner_handle window_handle )
            : Base_( tag::Wrap(), move( window_handle ) )
        {
            set_default_font( the_default_gui_font() );
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

                const ATOM result = ::RegisterClass( &params );
                hopefully( result != 0 )
                    or $fail( "RegisterClass failed" );
                return result;
            }

        public:
            virtual ~Window_class() = default;

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
            { $is_unused( params ); }  // Should signal ungood params by throwing.
        };

        virtual void close()
        {
            DestroyWindow( *this );
        }

        virtual void on_command( const int id )
        {
            $is_unused( id );
        }

        void dispatch_wm_scroll(
            const HWND                      control,
            const Scroll_direction::Enum    dir,
            const UINT                      code,
            const int                       pos
            )
        {
            if( is_zero( control )  or control == handle() ) {
                if( const auto p = dynamic_cast<Scroll_event_handler*>( this ) ) {
                    p->on_scroll( dir, code, pos );
                }
            } else {
                assert( is_zero( control ) or control == handle() );    // Will fire.
            }
        }

        void on_wm_close()
        {
            close();
        }

        void on_wm_command( const int id, const HWND control, const UINT notification_code )
        {
            if( is_zero( control ) ) {
                on_command( id );
            } else {
                $is_unused( notification_code );
                // TODO: reflect notification
            }
        }

        void on_wm_hscroll( const HWND control, const UINT code, const int pos )
        {
            dispatch_wm_scroll( control, Scroll_direction::horizontal, code, pos );
        }

        void on_wm_vscroll( const HWND control, const UINT code, const int pos )
        {
            dispatch_wm_scroll( control, Scroll_direction::vertical, code, pos );
        }

        auto on_message( const Message& m )
            -> LRESULT override
        {
            static const auto reflect_to = []( const HWND control, const Message& m )
                -> LRESULT
            { return ::SendMessage( control, m.message_id, m.word_param, m.long_param ); };

            switch( m.message_id ) {
                #if 0
                    WINAPI_CASE_WM( PAINT, m, on_wm_paint );
                #endif

                WINAPI_CASE_WM( COMMAND, m, on_wm_command );
                WINAPI_CASE_WM( CLOSE, m, on_wm_close );

                case WM_NOTIFY: {
                    const HWND control = reinterpret_cast<NMHDR*>( m.long_param )->hwndFrom;
                    if( control != handle() ) { return reflect_to( control, m ); }
                }

                case WM_HSCROLL:
                case WM_VSCROLL:
                {
                    const HWND control = reinterpret_cast<HWND>( m.long_param );
                    if( is_zero( control ) or control == handle() ) {
                        switch( m.message_id ) {
                            WINAPI_CASE_WM( HSCROLL, m, on_wm_hscroll );
                            WINAPI_CASE_WM( VSCROLL, m, on_wm_vscroll );
                        }
                    } else {
                        return reflect_to( control, m );
                    }
                }
            }

            return Base_::on_message( m );
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

        auto client_rect() const
            -> RECT
        {
            RECT r;
            ::GetClientRect( *this, &r ) or $fail( "::GetClientRect failed" );
            return r;
        }

        auto window_rect() const
            -> RECT
        {
            RECT r;
            ::GetWindowRect( *this, &r ) or $fail( "::GetWindowRect failed" );
            return r;
        }
    };
}  // namespace winapi::gui
