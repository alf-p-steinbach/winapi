#pragma once    // Source encoding: UTF-8 with BOM (π is a lowercase Greek "pi").

#include <winapi/gui/Control.hpp>
#include <winapi-header-wrappers/commctrl-h.hpp>

#include <algorithm>        // std::swap
#include <unordered_set>    // std::unordered_set

namespace winapi::gui {
    $use_cppx( Bitset_, is_in, Truth );
    $use_std( swap, unordered_set );

    class Trackbar_control:
        public Extends_<Control>
    {
    public:
        static constexpr auto& windowclass_name = TRACKBAR_CLASS;       // "msctls_trackbar32"

        struct Styles{ enum Enum{ vertical, manual_ticks, ticks_ul, ticks_dr, _ }; };
        static constexpr int n_styles = Styles::_;

        static constexpr auto ticks_both_sides = Bitset_<Styles::Enum>( tag::Compile_time(),
            Styles::ticks_ul, Styles::ticks_dr
            );

        struct Observer
        {
            virtual void on_new_position_for( Trackbar_control& control, const int new_position ) = 0;
        };

    private:
        unordered_set<Observer*>    m_listeners;

        auto on_custom_nm( const NMHDR& params_header )
            -> optional<LRESULT> override
        {
            const auto& params = reinterpret_cast<const NMTRBTHUMBPOSCHANGING&>( params_header );

            for( const Type_<Observer*> p_listener: m_listeners ) {
                p_listener->on_new_position_for( *this, params.dwPos );
            }
            return 0;
        }

        static auto creation_style_bits_from( const Bitset_<Styles::Enum> styleset )
            -> WORD
        {
            const Truth is_vertical         = is_in( styleset, Styles::vertical );
            const Truth has_manual_ticks    = is_in( styleset, Styles::manual_ticks );
            const Truth has_ticks_ul        = is_in( styleset, Styles::ticks_ul );
            const Truth has_ticks_dr        = is_in( styleset, Styles::ticks_dr );
            const Truth has_ticks_bs        = has_ticks_ul and has_ticks_dr;

            WORD bits = (is_vertical? TBS_VERT|TBS_DOWNISLEFT : TBS_HORZ);

            if( not has_manual_ticks ) {
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

        static auto n_tick_rows( const Bitset_<Styles::Enum> styleset )
            -> int
        { return !!is_in( styleset, Styles::ticks_ul ) + !!is_in( styleset, Styles::ticks_dr ); }

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
        static auto size_for( const int length, const Bitset_<Styles::Enum> styleset )
            -> SIZE
        {
            const int thumb_length = 21;    // TODO, e.g. TBM_GETTHUMBLENGTH
            SIZE result = { thumb_length + 10*n_tick_rows( styleset ), length };
            if( not is_in( styleset, Styles::vertical ) ) {
                swap( result.cx, result.cy );
            }
            return result;
        }

        Trackbar_control(
            const Type_<Displayable_window*>    p_parent,
            const POINT&                        position,
            const int                           length,
            const Bitset_<Styles::Enum>         styleset = {}
            ):
            Base_( tag::Wrap(), Api_window_factory().new_api_window(
                p_parent->handle(),
                position,
                size_for( length, styleset ),
                creation_style_bits_from( styleset ) )
            )
        {}

        Trackbar_control( tag::Wrap, Window_owner_handle window_handle ):
            Base_( tag::Wrap(), move( window_handle ) )
        {}

        auto position() const
            -> int
        { return int( const_cast<Trackbar_control*>( this )->process_message( TBM_GETPOS ) ); }

        void set_position( const int new_pos )
        {
            process_message( TBM_SETPOS, true, new_pos );
        }
            
        void add_observer( const Type_<Observer*> p_observer )
        {
            m_listeners.insert( p_observer );
            p_observer->on_new_position_for( *this, position() );
        }

        void remove_observer( const Type_<Observer*> p_observer )
        {
            m_listeners.erase( p_observer );
        }

        auto has_autoticks() const
            -> Truth
        { return (styles() & TBS_AUTOTICKS) != 0; }

        void set_range( const int first, const int last, const int tick_interval = 0 )
        {
            constexpr WORD max_word = WORD( -1 );
            assert( 0 <= first and first < int( max_word ) );
            assert( 0 <= last and last < int( max_word ) );
            assert( 0 <= tick_interval and tick_interval < int( max_word ) );

            if( tick_interval > 0 ) {
                hopefully( has_autoticks() )
                    or $fail( "Tick interval can only be specified for autoticks." );
                process_message( TBM_SETTICFREQ, WPARAM( tick_interval ) );
            }
            process_message( TBM_SETRANGE, true, MAKELPARAM( first, last ) );
        }
    };

    inline auto operator|(
        const Trackbar_control::Styles::Enum    a,
        const Trackbar_control::Styles::Enum    b
        ) -> Trackbar_control::Styles::Enum
    {
        return static_cast<Trackbar_control::Styles::Enum>( +a | +b );
    }

}  // namespace winapi::gui
