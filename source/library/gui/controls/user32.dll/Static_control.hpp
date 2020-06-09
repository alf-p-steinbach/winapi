#pragma once    // Source encoding: UTF-8 with BOM (π is a lowercase Greek "pi").

#include <winapi/gui/Control.hpp>

namespace winapi::gui {
    $use_cppx( Bitset_ );

    class Static_control:
        public Extends_<Control>
    {
    public:
        static constexpr auto& windowclass_name     = "STATIC";

        struct Styles{ enum Enum{ _ }; };
        static constexpr int n_styles = Styles::_;

    private:
        static auto creation_style_bits_from( const Bitset_<Styles::Enum> styleset )
            -> WORD
        {
            $is_unused( styleset );
            return 0;
        }

    protected:
        class Api_window_factory:
            public Extends_<Base_::Api_window_factory>
        {
        public:
            auto windowclass() const
                -> Windowclass_id override
            { return windowclass_name; }
        };

    public:
        Static_control(
            const Type_<Displayable_window*>    p_parent,
            const POINT&                        position,
            const SIZE&                         size,
            const DWORD                         stylebits = {}
            ):
            Base_( tag::Wrap(), Api_window_factory().new_api_window(
                p_parent->handle(), position, size, stylebits
                ) )
        {}

        Static_control( tag::Wrap, Window_owner_handle window_handle ):
            Base_( tag::Wrap(), move( window_handle ) )
        {}
    };

}  // namespace winapi::gui
