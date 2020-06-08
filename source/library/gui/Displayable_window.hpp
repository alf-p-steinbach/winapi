#pragma once    // Source encoding: UTF-8 with BOM (π is a lowercase Greek "pi").

#include <c/stdint.hpp>         // intptr_t

#include <winapi/graphics/the_default_gui_font.hpp>
#include <winapi/gui/Subclassed_window.hpp>
#include <winapi/gui/windowing-support/Windowclass_id.hpp>
#include <winapi/gui/windowing-support/Windowclass-util.hpp>        // windoclass_name_of

#include <optional>             // std::optional
#include <typeindex>            // std::typeindex

namespace winapi::gui {
    $use_cppx( Extends_, Map_ );
    $use_std( optional, type_index );

    class Displayable_window;

    class Reflected_notification_handler
    {
        friend class Displayable_window;

        using R = optional<LRESULT>;        // Empty means the message has not been handled.

        virtual auto on_nm_char( const NMCHAR& ) -> R                                   { return {}; }
        virtual auto on_nm_customdraw( const NMHDR& ) -> R                              { return {}; }
        virtual auto on_nm_customtext( const NMCUSTOMTEXT& ) -> R                       { return {}; }
        virtual auto on_nm_fontchanged( const NMHDR& ) -> R                             { return {}; }
        virtual auto on_nm_getcustomsplitrect( const NMCUSTOMSPLITRECTINFO& ) -> R      { return {}; }
        virtual auto on_nm_hover( const NMHDR& ) -> R                                   { return {}; }
        virtual auto on_nm_keydown( const NMKEY& ) -> R                                 { return {}; }
        virtual auto on_nm_killfocus( const NMHDR& ) -> R                               { return {}; }
        virtual auto on_nm_ldown( const NMHDR& ) -> R                                   { return {}; }
        virtual auto on_nm_nchittest( const NMMOUSE& ) -> R                             { return {}; }
        virtual auto on_nm_outofmemory( const NMHDR& ) -> R                             { return {}; }
        virtual auto on_nm_rdown( const NMHDR& ) -> R                                   { return {}; }
        virtual auto on_nm_releasedcapture( const NMHDR& ) -> R                         { return {}; }
        virtual auto on_nm_return( const NMHDR& ) -> R                                  { return {}; }
        virtual auto on_nm_setcursor( const NMMOUSE& ) -> R                             { return {}; }
        virtual auto on_nm_setfocus( const NMHDR& ) -> R                                { return {}; }
        virtual auto on_nm_themechanged( const NMHDR& ) -> R                            { return {}; }
        virtual auto on_nm_tooltipscreated( const NMHDR& ) -> R                         { return {}; }
        virtual auto on_nm_tvstateimagechanging( const NMTVSTATEIMAGECHANGING& ) -> R   { return {}; }
        virtual auto on_custom_nm( const NMHDR& ) -> R                                  { return {}; }

    public:
        virtual ~Reflected_notification_handler() = default;
    };

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

        static auto dispatch_to_specific_func_in(
            Reflected_notification_handler&     handler,
            const NMHDR&                        params
            ) -> optional<LRESULT>
        {
            #define INVOKE( func, Type ) \
                handler.func( reinterpret_cast<const Type&>( params ) )
            using Nm_csri = NMCUSTOMSPLITRECTINFO;

            switch( params.code ) {
                case NM_CHAR:                   { return INVOKE( on_nm_char, NMCHAR ); }
                case NM_CUSTOMDRAW:             { return INVOKE( on_nm_customdraw, NMHDR ); }
                case NM_FIRST-24:               { // Ambiguity introduced in Windows Vista.
                    static_assert( NM_FIRST-24 == NM_CUSTOMTEXT );              // SysLink control.
                    static_assert( NM_FIRST-24 == NM_TVSTATEIMAGECHANGING );    // TreeView control.

                    // TODO: This logic is not tested per early June 2020.
                    const string control_windowclass_name = windowclass_name_of( params.hwndFrom );
                    if( control_windowclass_name == "SysLink" ) {               // WC_LINK is wide literal only.
                        return handler.on_nm_customtext( reinterpret_cast<const NMCUSTOMTEXT&>( params ) );
                    } else if( control_windowclass_name == WC_TREEVIEWA ) {
                        return handler.on_nm_tvstateimagechanging(
                            reinterpret_cast<const NMTVSTATEIMAGECHANGING&>( params )
                        );
                    } else {
                        return {};
                    }
                }
                case NM_FONTCHANGED:            { return INVOKE( on_nm_fontchanged, NMHDR ); }
                case NM_GETCUSTOMSPLITRECT:     { return INVOKE( on_nm_getcustomsplitrect, Nm_csri ); }
                case NM_HOVER:                  { return INVOKE( on_nm_hover, NMHDR ); }
                case NM_KEYDOWN:                { return INVOKE( on_nm_keydown, NMKEY ); }
                case NM_KILLFOCUS:              { return INVOKE( on_nm_killfocus, NMHDR ); }
                case NM_LDOWN:                  { return INVOKE( on_nm_ldown, NMHDR ); }
                case NM_NCHITTEST:              { return INVOKE( on_nm_nchittest, NMMOUSE ); }
                case NM_OUTOFMEMORY:            { return INVOKE( on_nm_outofmemory, NMHDR ); }
                case NM_RDOWN:                  { return INVOKE( on_nm_rdown, NMHDR ); }
                case NM_RELEASEDCAPTURE:        { return INVOKE( on_nm_releasedcapture, NMHDR ); }
                case NM_RETURN:                 { return INVOKE( on_nm_return, NMHDR ); }
                case NM_SETCURSOR:              { return INVOKE( on_nm_setcursor, NMMOUSE ); }
                case NM_SETFOCUS:               { return INVOKE( on_nm_setfocus, NMHDR ); }
                case NM_THEMECHANGED:           { return INVOKE( on_nm_themechanged, NMHDR ); }
                case NM_TOOLTIPSCREATED:        { return INVOKE( on_nm_tooltipscreated, NMHDR ); }

                default:                        {
                    return handler.on_custom_nm( params );
                }
            }
            #undef INVOKE
        }

        virtual void on_command( const int id )
        {
            $is_unused( id );
        }

        virtual void on_notification( const HWND control, const int id )
        {
            $is_unused( control );   $is_unused( id );
        }

        void on_wm_close()
        {
            close();
        }

        void on_wm_command( const int id, const HWND control, const UINT notification_code )
        {
            if( control != 0 ) {
                on_notification( control, notification_code );
            } else {
                on_command( id );
            }
        }

        auto on_wm_notify( const int control_id, const Type_<const NMHDR*> p_params )
            -> LRESULT
        {
            const Type_<Subclassed_window*> p_wrapper = cpp_wrapper_for( p_params->hwndFrom );
            if( const auto p_handler = dynamic_cast<Reflected_notification_handler*>( p_wrapper ) ) {
                if( const auto optional_result = dispatch_to_specific_func_in( *p_handler, *p_params ) ) {
                    return optional_result.value();
                }
            }
            return FORWARD_WM_NOTIFY( this, control_id, p_params, apply_original_handling_of );
        }

        auto on_message( const Message& m )
            -> LRESULT override
        {
            switch( m.message_id ) {
                #if 0
                    case WM_PAINT: return process_wm_paint( handle(), m.word_param, m.long_param );
                #endif
                WINAPI_CASE_WM( COMMAND, m, on_wm_command );
                WINAPI_CASE_WM( CLOSE, m, on_wm_close );
                WINAPI_CASE_WM( NOTIFY, m, on_wm_notify );
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
