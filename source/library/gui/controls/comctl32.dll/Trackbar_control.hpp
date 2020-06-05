#pragma once    // Source encoding: UTF-8 with BOM (π is a lowercase Greek "pi").

#include <winapi/gui/Control.hpp>
#include <winapi-header-wrappers/commctrl-h.hpp>

namespace winapi::gui {
    $use_cppx( Truth );

    class Trackbar_control:
        public Extends_<Control>
    {
    public:
        struct Styles{ enum Enum{
            vertical            = 0x01,
            autoticks           = 0x02,
            ticks_ul            = 0x04,
            ticks_dr            = 0x08,
            ticks_both_sides    = ticks_ul | ticks_dr,
            _ }; };
        static constexpr int n_styles = Styles::_;

    private:
        static auto creation_style_bits_from( const Styles::Enum styles )
            -> WORD
        {
            const Truth is_vertical     = (styles & Styles::vertical) != 0;
            const Truth has_ticks_ul    = (styles & Styles::ticks_ul) != 0;
            const Truth has_ticks_dr    = (styles & Styles::ticks_dr) != 0;
            const Truth has_ticks_bs    = has_ticks_ul and has_ticks_dr;

            WORD bits = (is_vertical? TBS_VERT|TBS_DOWNISLEFT : TBS_HORZ);
            if( (styles & Styles::autoticks) != 0 ) {
                bits |= TBS_AUTOTICKS;
            }
            if( has_ticks_bs ) {
                bits |= TBS_BOTH;
            } else if( has_ticks_ul ) {
                bits |= (is_vertical? TBS_TOP : TBS_LEFT);
            } else if( has_ticks_dr ) {
                bits |= (is_vertical? TBS_BOTTOM : TBS_RIGHT);
            } else {
                bits |= TBS_NOTICKS;
            }
            return bits;
        }

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
        Trackbar_control(
            const Type_<Displayable_window*>    p_parent,
            const POINT&                        position,
            const SIZE&                         size,
            const Styles::Enum                  styles  = {}
        ):
            Base_( tag::Wrap(), Api_window_factory().new_api_window(
                p_parent->handle(), position, size, creation_style_bits_from( styles ) )
            )
        {}

        Trackbar_control( tag::Wrap, Window_owner_handle window_handle ):
            Base_( tag::Wrap(), move( window_handle ) )
        {}
    };

    inline auto operator|(
        const Trackbar_control::Styles::Enum    a,
        const Trackbar_control::Styles::Enum    b
        ) -> Trackbar_control::Styles::Enum
    {
        return static_cast<Trackbar_control::Styles::Enum>( +a | +b );
    }

}  // namespace winapi::gui
