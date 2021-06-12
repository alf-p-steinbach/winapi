#pragma once    // Source encoding: UTF-8 with BOM (π is a lowercase Greek "pi").

#include <winapi-header-wrappers/windows-h.hpp>

namespace winapi::gui {

    struct Stylebit_groups { enum Enum{ basic = GWL_STYLE, extended = GWL_EXSTYLE };};

    template< Stylebit_groups::Enum group, class Derived >
    class Stylebits_
    {
    private:
        static constexpr int    index   = group;

        auto window_handle() const
            -> HWND
        { return static_cast<Derived const*>( this )->handle(); }

    public:
        auto value() const
            -> DWORD
        { return static_cast<DWORD>( ::GetWindowLongPtr( window_handle(), index ) ); }

        operator DWORD () const { return value(); }

        void add( const DWORD more_styles )
        {
            ::SetWindowLongPtr( window_handle(), index, value() | more_styles );
        }

        void remove( const DWORD styles_to_remove )
        {
            ::SetWindowLongPtr( window_handle(), index, value() & ~styles_to_remove );
        }
    };

    template< class Derived >
    using Basic_stylebits_ = Stylebits_<Stylebit_groups::basic, Derived>;

    template< class Derived >
    using Extended_stylebits_ = Stylebits_<Stylebit_groups::extended, Derived>;

}  // namespace winapi::gui
