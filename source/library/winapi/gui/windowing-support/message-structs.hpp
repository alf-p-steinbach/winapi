#pragma once    // Source encoding: UTF-8 with BOM (π is a lowercase Greek "pi").
#include <winapi-header-wrappers/windows-h.hpp>

#include <c/stddef.hpp>     // offsetof


#if 0
    struct MSG
    {
        HWND        hwnd;
        UINT        message;
        WPARAM      wParam;
        LPARAM      lParam;
        DWORD       time;           // See (presumably) GetMessageTime 
        POINT       pt;             // See (presumably) GetMessagePos
    };
#endif

namespace winapi::gui
{
    struct Message
    {
        HWND     window;
        UINT     message_id;
        WPARAM   word_param;
        LPARAM   long_param;
    };

    static_assert( offsetof( Message, window )      == offsetof( MSG, hwnd ) );
    static_assert( offsetof( Message, message_id )  == offsetof( MSG, message ) );
    static_assert( offsetof( Message, word_param )  == offsetof( MSG, wParam ) );
    static_assert( offsetof( Message, long_param )  == offsetof( MSG, lParam ) );

    struct Queued_message:
        Message
    {
        DWORD    tick_count;
        POINT    mouse_pos;
    };

    static_assert( offsetof( Queued_message, tick_count )  == offsetof( MSG, time ) );
    static_assert( offsetof( Queued_message, mouse_pos )   == offsetof( MSG, pt ) );

    inline auto as_api_msg( Queued_message& m )
        -> MSG&
    { return reinterpret_cast<MSG&>( m ); }

    inline auto as_api_msg( const Queued_message& m )
        -> const MSG&
    { return reinterpret_cast<const MSG&>( m ); }

}  // namespace winapi::gui
