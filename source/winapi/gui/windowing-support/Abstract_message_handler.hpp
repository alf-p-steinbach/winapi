#pragma once    // Source encoding: UTF-8 with BOM (π is a lowercase Greek "pi").

#include <cppx-core-language/all.hpp>

#include <winapi/gui/windowing-support/message-structs.hpp>
#include <winapi-header-wrappers/windowsX-h.hpp>        // Message cracker macros

#include <utility>          // std::forward

#define WINAPI_HANDLE_WM( name, msg, m_func )           \
        HANDLE_WM_##name (                              \
            this, msg.word_param, msg.long_param,       \
            [&]( auto* p_window, auto... args )         \
                -> auto                                 \
            { return (p_window->m_func)( args... ); }   \
        )

#define WINAPI_CASE_WM( name, m, func ) case WM_##name: return WINAPI_HANDLE_WM(name, m, func);

namespace winapi::gui {
    $use_cppx( Type_ );
    $use_std( forward );

    class Abstract_message_handler
    {
    friend class Window_subclassing;
    protected:
        virtual auto on_message( const Message& )
            -> LRESULT = 0;

    public:
        virtual ~Abstract_message_handler() {}
    };

}  // namespace winapi::gui
