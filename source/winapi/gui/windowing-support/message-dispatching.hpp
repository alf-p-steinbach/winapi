#pragma once    // Source encoding: UTF-8 with BOM (π is a lowercase Greek "pi").
#include <cppx-core-language/all.hpp>

#include <cppx-core/collections/Map_.hpp>           // cppx::Map_
#include <cppx-core/collections/is_in.hpp>          // cppx::is_in(raw array)

#include <exception>                                // std::(current_exception, exception_ptr)
#include <utility>                                  // std::(exchange, move)

#include <winapi/gui/windowing-support/message-structs.hpp>     // winapi::gui::Queued_message
#include <winapi/gui/windowing-support/message_name.hpp>        // winapi::gui::message_name
#include <winapi-header-wrappers/windows-h.hpp>

namespace winapi::gui {
    using namespace cppx::x_throwing;
    using namespace cppx::basic_string_assembly;    // "<<" notation.
    $use_cppx( is_in, Map_ );
    $use_std( current_exception, exception_ptr, rethrow_exception, exchange, move );

    #if 0
        inline auto accelerators()
            -> Map_<HWND, HACCEL>&
        {
            static Map_<HWND, HACCEL> the_accelerators;
            return the_accelerators;
        }

        inline auto accelerators_for( const HWND window )
            -> HACCEL
        {
            auto const& a = accelerators();
            return (is_in( a, window )? a[window] : 0);
        }

        inline auto handle_accelerators( const Queued_message& m )
            -> bool
        {
            static const UINT keypress_messages[] =
            {
                WM_KEYDOWN, WM_SYSKEYDOWN, WM_KEYUP, WM_SYSKEYUP
            };

            if( is_in( keypress_messages, m.message_id ) ) {
                const HWND top_window = GetAncestor( m.window, GA_ROOT );
                const HACCEL accelerators = accelerators_for( top_window );
                if( accelerators == 0 ) {
                    return false;
                }
                MSG mutable_m = as_api_msg( m );
                return TranslateAccelerator( top_window, accelerators, &mutable_m );
            }
            return false;
        }
    #endif

    namespace impl
    {
        inline auto propagated_exception_ref()
            -> exception_ptr&
        {
            static exception_ptr the_exception_ptr;
            return the_exception_ptr;
        }
    }

    inline void propagate( exception_ptr p )
    {
        impl::propagated_exception_ref() = move( p );

        // Best effort to have it processed in timely fashion:
        ::PostMessage( HWND{}, WM_NULL, WPARAM{}, LPARAM{} );
    }

    inline auto get_propagated_exception()
        -> exception_ptr
    { return exchange( impl::propagated_exception_ref(), nullptr ); }

    inline auto get_window_message()
        -> gui::Queued_message
    {
        gui::Queued_message result;
        hopefully( ::GetMessage( &as_api_msg( result ), 0, 0, 0 ) >= 0 )
            or $fail( "GetMessage failed" );
        return result;
    }

    inline auto generate_char_messages_from( const gui::Queued_message& m )
        -> bool
    { return ::TranslateMessage( &as_api_msg( m ) ); }

    inline auto dispatch_to_window( const gui::Queued_message& m )
        -> LRESULT
    {
        const LRESULT result = ::DispatchMessage( &as_api_msg( m ) );
        if( impl::propagated_exception_ref() != nullptr ) {
            try {
                rethrow_exception( get_propagated_exception() );
            } catch( ... ) {
                $fail( "Processing of "s << message_name( m.message_id ) << " failed" );
            }
        }
        return result;
    }

    inline auto dispatch_window_messages()
        -> int
    {
        for( ;; ) {
            const gui::Queued_message m = get_window_message();
            if( m.message_id == WM_QUIT )
            {
                return static_cast<int>( m.word_param );
            }
            // TODO: handle accelerators?
            // TODO: suppress keydown repeats
            generate_char_messages_from( m );
            dispatch_to_window( m );
        }
    }

    inline void empty_the_message_queue()
    {
        MSG m;
        while( ::PeekMessage( &m, 0, 0, 0, false ) ) {
            ::GetMessage( &m, 0, 0, 0 );
        }
    }
}  // namespace winapi::gui
