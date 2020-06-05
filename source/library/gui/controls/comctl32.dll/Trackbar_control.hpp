#pragma once    // Source encoding: UTF-8 with BOM (π is a lowercase Greek "pi").

#include <winapi/gui/Control.hpp>
#include <winapi-header-wrappers/commctrl-h.hpp>

#include <bitset>               // std::bitset
#include <initializer_list>     // std::initializer_list

namespace winapi::gui {
    $use_cppx( Truth );
    $use_std( bitset, initializer_list );

    class Trackbar_control:
        public Extends_<Control>
    {
    protected:
        class Api_window_factory:
            public Extends_<Base_::Api_window_factory>
        {
        public:
            auto windowclass() const
                -> Windowclass_id override
            { return TRACKBAR_CLASS; }
        };

    public:
        struct Styles{ enum Enum{ vertical, autoticks, ticks_ul, ticks_dr, ticks_both_sides, _ }; };
        static constexpr int n_styles = Styles::_;
        using Styles_spec = initializer_list<Styles::Enum>;

        static auto creation_style_bits_from( const Styles_spec& spec )
            -> WORD
        {
            std::bitset<n_styles>   mutable_styles;
            for( const Styles::Enum style: spec ) {
                mutable_styles.set( style );
            }
            const auto styles = mutable_styles;
            $is_unused( mutable_styles );

            const Truth is_vertical     = styles.test( Styles::vertical );
            const Truth has_ticks_ul    = styles.test( Styles::ticks_ul );
            const Truth has_ticks_dr    = styles.test( Styles::ticks_dr );
            const Truth has_ticks_bs    = false
                or styles.test( Styles::ticks_both_sides )
                or (has_ticks_ul and has_ticks_dr);

            WORD bits = (is_vertical? TBS_VERT|TBS_DOWNISLEFT : TBS_HORZ);
            if( styles.test( Styles::autoticks ) ) {
                bits |= TBS_AUTOTICKS;
            }
            if( has_ticks_bs ) {
                bits |= TBS_BOTH;
            } else {
                if( has_ticks_ul ) {
                    bits |= (is_vertical? TBS_TOP : TBS_LEFT);
                }
                if( has_ticks_dr ) {
                    bits |= (is_vertical? TBS_BOTTOM : TBS_RIGHT);
                }
            }
            return bits;
        }

        Trackbar_control(
            const Type_<Displayable_window*>    p_parent,
            const POINT&                        position,
            const SIZE&                         size,
            const Styles_spec                   styles  = {}
            ):
            Base_( tag::Wrap(), Api_window_factory().new_api_window(
                p_parent->handle(), position, size, creation_style_bits_from( styles ) )
                )
        {}

        Trackbar_control( tag::Wrap, Window_owner_handle window_handle ):
            Base_( tag::Wrap(), move( window_handle ) )
        {}

    };

}  // namespace winapi::gui
