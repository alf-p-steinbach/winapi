#pragma once    // Source encoding: UTF-8 with BOM (π is a lowercase Greek "pi").

#include <winapi-header-wrappers/windows-h.hpp>

namespace winapi::gui {

    class Stylebits
    {
    public:
        struct Kind{ enum Enum{ classic, extended }; };

    private:
        HWND            m_window;
        int             m_index;

    public:
        Stylebits( const HWND window, const Kind::Enum kind = Kind::classic ):
            m_window( window ),
            m_index( kind == Kind::classic? GWL_STYLE : GWL_EXSTYLE )
        {}

        auto value() const
            -> DWORD
        { return static_cast<DWORD>( ::GetWindowLongPtr( m_window, m_index ) ); }

        operator DWORD () const { return value(); }

        void set( const DWORD more_styles )
        {
            ::SetWindowLongPtr( m_window, m_index, value() | more_styles );
        }

        void unset( const DWORD styles_to_remove )
        {
            ::SetWindowLongPtr( m_window, m_index, value() & ~styles_to_remove );
        }
    };

}  // namespace winapi::gui
