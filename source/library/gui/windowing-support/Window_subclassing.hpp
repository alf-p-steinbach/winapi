#pragma once    // Source encoding: UTF-8 with BOM (π is a lowercase Greek "pi").
#include <cppx-core-language/all.hpp>

#include <winapi-header-wrappers/commctrl-h.hpp>        // SetWindowSubclass etc.

#include <winapi/gui/windowing-support/Abstract_message_handler.hpp>
#include <winapi/gui/windowing-support/message-dispatching.hpp>         // winapi::gui::propagate
#include <winapi/gui/windowing-support/Window_owner_handle.hpp>         // winapi::Window_owner_handle

namespace winapi::gui {
    using namespace cppx::x_throwing;
    $use_cppx( No_copy_or_move, Type_ );

    class Window_subclassing
        : public No_copy_or_move
    {
        static constexpr int common_subclass_id = 0;

        HWND                        m_window;
        Abstract_message_handler*   m_p_handler;

        static auto CALLBACK subclass_proc(
            const HWND              window,
            const UINT              message_id,
            const WPARAM            word_param,
            const LPARAM            long_param,
            const UINT_PTR          subclass_id,
            const DWORD_PTR         data
            ) -> LRESULT
        {
            $is_unused( subclass_id );
            static_assert( sizeof( data ) == sizeof( void* ) );
            const auto p_handler = reinterpret_cast<Abstract_message_handler*>( data );
            // assert( p_handler );
            try {
                return p_handler->on_message( {window, message_id, word_param, long_param} );
            } catch( ... ) {
                propagate( current_exception() );
            }
            return ::DefWindowProc( window, message_id, word_param, long_param );
        }

        void end_subclassing()
        {
            ::RemoveWindowSubclass( m_window, &subclass_proc, common_subclass_id )
                or $fail( "RemoveWindowSubclass failed" );
            m_window = 0;
        }

    public:
        static auto message_handler_for( const HWND window ) noexcept
            -> Abstract_message_handler*
        {
            DWORD_PTR data;
            static_assert( sizeof( data ) == sizeof( void* ) );
            const bool success = ::GetWindowSubclass( window, &subclass_proc, common_subclass_id, &data );
            return (success? reinterpret_cast<Abstract_message_handler*>( data ) : nullptr);
        }

        Window_subclassing(
            Window_owner_handle                     window_handle,
            const Type_<Abstract_message_handler*>  p_handler
            ):
            m_window( window_handle ),
            m_p_handler( p_handler )
        {
            ::SetWindowSubclass(
                window_handle,
                &subclass_proc,
                common_subclass_id,
                reinterpret_cast<DWORD_PTR>( p_handler )
            )
                or $fail( "SetWindowSubclass failed" );
            window_handle.release();
        }

        ~Window_subclassing()
        {
            if( m_window != 0 ) {
                const HWND doomed = m_window;
                end_subclassing();
                ::DestroyWindow( doomed );
            }
        }

        void release() { if( m_window != 0 ) { end_subclassing(); } }

        auto handle() const -> HWND { return m_window; }

        auto apply_original_handling_of( const Message& m ) const
            -> LRESULT
        {
            //TODO: check for propagated x.
            return ::DefSubclassProc( m.window, m.message_id, m.word_param, m.long_param );
        }
    };

}  // namespace winapi::gui
