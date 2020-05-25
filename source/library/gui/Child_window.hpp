#pragma once    // Source encoding: UTF-8 with BOM (π is a lowercase Greek "pi").
#include <cppx-core-language/all.hpp>

#include <string_view>
#include <tuple>            // std::tie
#include <utility>          // std::move

#include <winapi/gui/Displayable_window.hpp>
#include <winapi/gui/windowing-support/create_window.hpp>

namespace winapi::gui {
    using namespace cppx::x_throwing;
    $use_cppx( C_str, Type_ );
    $use_std( move );

    class Child_window
        : public Extends_<Displayable_window>
    {
    public:
        static constexpr auto& windowclass_name = "Child-window";

    protected:
        Displayable_window*     m_parent;

        class Window_class:
            public Extends_<Base_::Window_class>
        {
        protected:
            void override_values( WNDCLASS& params ) const
                override
            { params.lpszClassName = windowclass_name; }
        };

        class Api_window_factory:
            public Extends_<Base_::Api_window_factory>
        {
        public:
            auto windowclass() const
                -> Windowclass_id override
            { return Window_class().id(); }

            auto fixed_window_style() const
                -> Window_style override
            { return WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE; }

            void fail_if_obviously_wrong( const CREATESTRUCT& params ) const
                override
            {
                Base_::fail_if_obviously_wrong( params );

                hopefully( (params.style & WS_CHILD) != 0 )
                    or $fail( "A child window must have the WS_CHILD style." );
                hopefully( params.hwndParent != 0 )
                    or $fail( "A child window must have a parent window." );
                hopefully( (params.style & WS_POPUP) == 0 )
                    or $fail( "The WS_POPUP style is not meaningful for a child window." );
            }

            auto new_api_window( const HWND parent_window, const POINT& position, const POINT& size )
                -> Window_owner_handle
            {
                CREATESTRUCT params = fixed_creation_params();
                $with( params ) {
                    _.x             = position.x;
                    _.y             = position.y;
                    _.cx            = size.x;
                    _.cy            = size.y;
                    _.hwndParent    = parent_window;
                }
                fail_if_obviously_wrong( params );
                return create_window( params );
            }
        };  // class Api_window_factory

    public:
        auto parent() const
            -> Displayable_window*
        { return m_parent; }

        void set_text( const C_str text )
        {
            ::SetWindowText( *this, text )
                or $fail( "SetWindowText failed" );
        }

        Child_window( const Type_<Displayable_window*> parent, const POINT& position, const POINT& size ):
            Base_( tag::Wrap(), Api_window_factory().new_api_window( parent->handle(), position, size ) ),
            m_parent( parent )
        {}

        Child_window( tag::Wrap, Window_owner_handle window_handle ):
            Base_( tag::Wrap(), move( window_handle ) ),
            m_parent( nullptr )     // TODO: consider looking up the C++ object.
        {}
    };

}  // namespace winapi::gui
