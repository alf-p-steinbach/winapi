#pragma once
#include <cppx/text/C_string_ref.hpp>
#include <cppx/diagnostics/CPPX_FAIL.hpp>
#include <stdlib/extension/type_builders.hpp>
#include <stdlib/extension/hopefully_and_fail.hpp>
#include <winapi/gui/Window.hpp>

namespace winapi::gui::control {
    using namespace stdlib::ext::hopefully_and_fail;
    using namespace stdlib::ext::type_builders;         // ptr_

    namespace impl {
        inline auto register_window_class()
            -> Api_windowclass_id
        {
            WNDCLASS params = {};
            params.style            = CS_DBLCLKS;
            params.lpfnWndProc      = &DefWindowProc;
            params.hInstance        = GetModuleHandle( nullptr );
            params.hIcon            = LoadIcon( 0, IDI_APPLICATION );
            params.hCursor          = LoadCursor( 0, IDC_ARROW );
            params.hbrBackground    = reinterpret_cast<HBRUSH>( COLOR_BTNFACE + 1 );
            params.lpszClassName    = L"Control";

            ATOM const result = RegisterClass( &params );
            hopefully( result != 0 )
                or CPPX_FAIL( "RegisterClass failed" );
            return result;
        }
    }  // namespace impl

    inline auto window_class()
        -> Api_windowclass_id
    {
        static Api_windowclass_id const the_id = impl::register_window_class();
        return the_id;
    }
}  // namespace winapi::gui::top_level_window

namespace winapi::gui {
    using cppx::C_widestring_ref;
    using std::make_tuple;
    using std::tie;

    using Control_base = winapi::gui::Window;
    class Control
        : public Control_base
    {
        using Base = Control_base;

    protected:
        class Api_window_factory;       // Defined out-of-class below.

    public:
        void set_text( ref_<const C_widestring_ref> text )
        {
            SetWindowText( *this, text )
                or CPPX_FAIL( "SetWindowText failed" );
        }

        ~Control() override {}

        inline Control( const HWND parent );
        inline Control( ref_<const Api_window_factory> factory );
    };

    class Control::Api_window_factory
        : public Base::Api_window_factory
    {
    private:
        const HWND m_parent;

    public:
        auto parent() const -> HWND { return m_parent; }

        virtual auto window_class() const
            -> Api_windowclass_id
        { return control::window_class(); }

        void set( ref_<CREATESTRUCT> _ ) const
            override
        {
            Base::Api_window_factory::set( _ );
            _.hwndParent = parent();
            tie( _.cx, _.cy, _.x, _.y ) = make_tuple( 10, 10, 80, 30 );
            _.style = WS_CHILD | WS_VISIBLE | WS_BORDER;
            _.lpszClass = window_class().pseudo_ptr();
        }

        Api_window_factory( const HWND parent )
            : m_parent{ parent }
        {}
    };

    Control::Control( const HWND parent )
        : Base{ Api_window_factory{ parent } }
    {}

    Control::Control( ref_<const Api_window_factory> factory )
        : Base{ factory }
    {}

}  // namespace winapi::gui::control
