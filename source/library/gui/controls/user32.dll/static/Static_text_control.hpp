#pragma once    // Source encoding: UTF-8 with BOM (π is a lowercase Greek "pi").

#include <winapi/gui/controls/user32.dll/Static_control.hpp>

namespace winapi::gui {
    $use_cppx( Bitset_, is_in, Truth, Type_ );

    class Static_text_control:
        public Extends_<Static_control>
    {
    public:
        struct Kind{ enum Enum{
            left_text           = SS_LEFT,
            center_text         = SS_CENTER,
            right_text          = SS_RIGHT,
            left_text_nowrap    = SS_LEFTNOWORDWRAP,
            no_graying          = SS_SIMPLE
        }; };

        struct Styles{ enum Enum{
            notify          , // = SS_NOTIFY,
            underlining     , // = ~SS_NOPREFIX
            word_ellipsis   , // = SS_WORDELLIPSIS,           // Note: = SS_PATHELLIPSIS|SS_ENDELLIPSIS
            end_ellipsis    , // = SS_ENDELLIPSIS,
            path_ellipsis   , // = SS_PATHELLIPSIS
        _ }; };
        static constexpr int n_styles = Styles::_;

    private:
        static auto creation_style_bits_from( const Bitset_<Styles::Enum> styleset )
            -> WORD
        {
            static_assert( SS_WORDELLIPSIS == (SS_PATHELLIPSIS | SS_ENDELLIPSIS) );

            const bool  tw      = is_in( styleset, Styles::word_ellipsis );
            const bool  te      = is_in( styleset, Styles::end_ellipsis );
            const bool  tp      = is_in( styleset, Styles::path_ellipsis );

            hopefully( tw + te + tp <= 1 )
                or $fail( "Only one ellipsis truncation mode can be specified." );

            WORD result = 0;
            result |= (SS_NOTIFY         * +is_in( styleset, Styles::notify ));
            result |= (SS_NOPREFIX       * not is_in( styleset, Styles::underlining ));
            result |= (SS_WORDELLIPSIS   * +is_in( styleset, Styles::word_ellipsis ));
            result |= (SS_ENDELLIPSIS    * +is_in( styleset, Styles::end_ellipsis ));
            result |= (SS_PATHELLIPSIS   * +is_in( styleset, Styles::path_ellipsis ));
            return result;
        }

    public:
        Static_text_control(
            const Kind::Enum                    kind,
            const C_str                         text,
            const Type_<Displayable_window*>    p_parent,
            const POINT&                        position,
            const SIZE&                         size,
            const Bitset_<Styles::Enum>         styles = {}
            ):
            Base_( tag::Wrap(), Api_window_factory().new_api_window(
                p_parent->handle(), position, size, kind | creation_style_bits_from( styles )
            ) )
        {
            this->set_text( text );
        }

        Static_text_control( tag::Wrap, Window_owner_handle window_handle ):
            Base_( tag::Wrap(), move( window_handle ) )
        {}

        static auto left(
            const C_str                         text,
            const Type_<Displayable_window*>    p_parent,
            const POINT&                        position,
            const SIZE&                         size,
            const Bitset_<Styles::Enum>         styles = {}
            ) -> Static_text_control*
        {  return new Static_text_control( Kind::left_text, text, p_parent, position, size, styles ); }

        static auto center(
            const C_str                         text,
            const Type_<Displayable_window*>    p_parent,
            const POINT&                        position,
            const SIZE&                         size,
            const Bitset_<Styles::Enum>         styles = {}
            ) -> Static_text_control*
        {  return new Static_text_control( Kind::center_text, text, p_parent, position, size, styles ); }

        static auto right(
            const C_str                         text,
            const Type_<Displayable_window*>    p_parent,
            const POINT&                        position,
            const SIZE&                         size,
            const Bitset_<Styles::Enum>         styles = {}
            ) -> Static_text_control*
        {  return new Static_text_control( Kind::right_text, text, p_parent, position, size, styles ); }

        static auto left_nowrap(
            const C_str                         text,
            const Type_<Displayable_window*>    p_parent,
            const POINT&                        position,
            const SIZE&                         size,
            const Bitset_<Styles::Enum>         styles = {}
            ) -> Static_text_control*
        {  return new Static_text_control( Kind::left_text_nowrap, text, p_parent, position, size, styles ); }

        static auto no_graying(
            const C_str                         text,
            const Type_<Displayable_window*>    p_parent,
            const POINT&                        position,
            const SIZE&                         size,
            const Bitset_<Styles::Enum>         styles = {}
            ) -> Static_text_control*
        {  return new Static_text_control( Kind::no_graying, text, p_parent, position, size, styles ); }
    };

}  // namespace winapi::gui
