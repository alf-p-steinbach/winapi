#pragma once    // Source encoding: UTF-8 with BOM (π is a lowercase Greek "pi").
#include <cppx-core-language/all.hpp>

#include <functional>       // std::invoke
#include <utility>          // std::(forward, move)

#include <winapi/gui/windowing-support/Window_subclassing.hpp>
#include <winapi/gui/windowing-support/Window_owner_handle.hpp>

namespace winapi::gui {
    $use_cppx( No_copy_or_move );
    $use_std( forward, move );

    class Subclassed_window
        : protected Abstract_message_handler
        , public No_copy_or_move
    {
        Window_subclassing      m_subclassing;

    protected:
        Subclassed_window( Window_owner_handle window_handle )
            : m_subclassing( move( window_handle ), this )
        {}

        auto original_handling_of( const Message& m )
            -> LRESULT
        { return m_subclassing.original_handling_of( m ); }

        auto on_message( const Message& m )
            -> LRESULT override
        {
            if( m.message_id == WM_DESTROY ) {
                LRESULT result = 0;
                try {
                    result = original_handling_of( m );
                } catch( ... ) {
                    // TODO: How to deal with failure to clean up? Hm. For now intentionally ignored.
                }
                m_subclassing.release();
                delete this;
                return result;
            }
            return original_handling_of( m );
        }

        auto process_message(
            const UINT id,
            const WPARAM word_param = 0,
            const LPARAM long_param = 0
            ) -> LRESULT
        {
            // The indirection via ::SendMessage supports e.g. message hooks.
            return ::SendMessage( handle(), id, word_param, long_param );
        }

    public:
        auto handle() const
            -> HWND
        { return m_subclassing.handle(); }

        operator HWND() const { return handle(); }
    };

}  // namespace winapi::gui
