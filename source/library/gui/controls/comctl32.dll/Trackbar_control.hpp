#pragma once    // Source encoding: UTF-8 with BOM (π is a lowercase Greek "pi").

#include <winapi/gui/Control.hpp>
#include <winapi-header-wrappers/commctrl-h.hpp>

#include <event-handling-support/Observable_.hpp>

#include <c/stdint.hpp>     // int16_t

#include <algorithm>        // std::swap
#include <utility>          // std::exchange

namespace winapi::gui {
    $use_cppx( Bitset_, is_in, max_, No_copy, Sequence_, Truth );
    $use_std( exchange, swap );
    $use_from_namespace( event_handling_support, Observable_ );

    namespace ns_trackbar_control {
        struct Observer_interface
        {
            virtual void on_new_position( const int new_position ) = 0;
        };
    }  // namespace ns_trackbar_control 

    class Trackbar_control:
        public Extends_<Control>,
        public Scroll_event_handler,
        public Observable_<Trackbar_control, ns_trackbar_control::Observer_interface>
    {
    public:
        static constexpr auto& windowclass_name = TRACKBAR_CLASS;       // "msctls_trackbar32"

        struct Styles{ enum Enum{ vertical, manual_ticks, ticks_ul, ticks_dr, _ }; };
        static constexpr int n_styles = Styles::_;

        static constexpr auto ticks_both_sides = Bitset_<Styles::Enum>( tag::Compile_time(),
            Styles::ticks_ul, Styles::ticks_dr
            );

        using Observer_interface = ns_trackbar_control::Observer_interface;

    private:
        Truth                               m_is_reversed;
        Sequence_<int>                      m_range;

        auto logical_pos_from_raw( int raw_pos ) const
            -> int
        {
            const int   first       = m_range.first();
            const int   last        = m_range.last();

            const int clamped = (0?0
                : raw_pos < first?       first
                : raw_pos > last?        last
                :                        raw_pos
                );
            return (m_is_reversed? last - (clamped - first) : clamped);
        }

        static auto creation_style_bits_from( const Bitset_<Styles::Enum> styleset )
            -> WORD
        {
            const Truth is_vertical         = is_in( styleset, Styles::vertical );
            const Truth has_manual_ticks    = is_in( styleset, Styles::manual_ticks );
            const Truth has_ticks_ul        = is_in( styleset, Styles::ticks_ul );
            const Truth has_ticks_dr        = is_in( styleset, Styles::ticks_dr );
            const Truth has_ticks_bs        = has_ticks_ul and has_ticks_dr;

            // TBS_NOTIFYBEFOREMOVE doesn't produce notifications for scrollwheel, so, WM_xSCROLL.
            // TBS_REVERSED is just a flag.
            WORD bits = WORD( +is_vertical )*(TBS_REVERSED | TBS_DOWNISLEFT);
            
            bits |= (is_vertical? TBS_VERT : TBS_HORZ);
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

        auto get_range() const
            -> Sequence_<int>
        {
            auto& self = *const_cast<Trackbar_control*>( this );
            return Sequence_<int>(
                int( self.process_message( TBM_GETRANGEMIN ) ),
                int( self.process_message( TBM_GETRANGEMAX ) )
                );
        }

    protected:
        class Api_window_factory:
            public Extends_<Base_::Api_window_factory>
        {
        public:
            auto fixed_window_style() const
                -> Window_style override
            { return Base_::fixed_window_style() | 0; }

            auto windowclass() const
                -> Windowclass_id override
            { return windowclass_name; }
        };

        virtual void on_position_change( const int new_position )
        {
            for_each_observer( [=]( const auto p_observer ) {
                p_observer->on_new_position( new_position );
                } );
        }

        void on_scroll(
            const Scroll_direction::Enum    direction,
            const UINT                      code,
            const int                       raw_pos
            ) override
        {
            $is_unused( direction );
            switch( code ) {
                case TB_THUMBPOSITION:
                case TB_THUMBTRACK: {
                    on_position_change( logical_pos_from_raw( raw_pos ) );
                    break;
                }
                default: {
                    assert( raw_pos == 0 );     // Per the documentation.
                    on_position_change( position() );
                }
            }
        }

    public:
        static auto control_size_for( const int length, const Bitset_<Styles::Enum> styleset )
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
                control_size_for( length, styleset ),
                creation_style_bits_from( styleset ) )
                ),
            m_is_reversed( (styles() & TBS_REVERSED) != 0 ),
            m_range( get_range() )
        {}

        Trackbar_control( tag::Wrap, Window_owner_handle window_handle ):
            Base_( tag::Wrap(), move( window_handle ) ),
            m_is_reversed( (styles() & TBS_REVERSED) != 0 ),
            m_range( get_range() )
        {}

        auto position() const
            -> int
        {
            return logical_pos_from_raw(
                int( const_cast<Trackbar_control*>( this )->process_message( TBM_GETPOS ) )
                );
        }

        void set_position( const int logical_new_pos )
        {
            const int new_pos = (0?0
                : m_is_reversed?    m_range.first() + (m_range.last() - logical_new_pos)
                :                   logical_new_pos
                ); 
            process_message( TBM_SETPOS, true, new_pos );
            on_position_change( logical_new_pos );
        }
            
        auto has_autoticks() const
            -> Truth
        { return (styles() & TBS_AUTOTICKS) != 0; }

        auto is_vertical() const
            -> Truth
        { return (styles() & TBS_VERT) != 0; }

        void set_range( const int first, const int last, const int tick_interval = 0 )
        {
            constexpr int max_value = max_<int16_t>;        // Signed, = 32767. Documentation ungood.
            constexpr auto valid_range = Sequence_<int>( 0, max_value );

            assert( is_in( valid_range, first ) );
            assert( is_in( valid_range, last ) );
            assert( first < last );
            assert( is_in( valid_range, tick_interval ) );

            if( tick_interval > 0 ) {
                hopefully( has_autoticks() )
                    or $fail( "Tick interval can only be specified for autoticks." );
                process_message( TBM_SETTICFREQ, WPARAM( tick_interval ) );
            }

            process_message( TBM_SETRANGE, true, MAKELPARAM( first, last ) );
            m_range = Sequence_<int>( first, last );
            set_position( first );
        }
    };

}  // namespace winapi::gui
