#pragma once    // Source encoding: UTF-8 with BOM (π is a lowercase Greek "pi").

#include <winapi/gui/Control.hpp>
#include <winapi-header-wrappers/commctrl-h.hpp>

#include <c/stdint.hpp>     // int16_t

#include <algorithm>        // std::swap
#include <unordered_set>    // std::unordered_set

namespace winapi::gui {
    $use_cppx( Bitset_, is_in, max_, Sequence_, Truth );
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
            virtual void on_new_position( const int new_position ) = 0;
        };

    private:
        unordered_set<Observer*>    m_observers;
        Truth                       m_is_reversed;
        Sequence_<int>              m_range;

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

        //auto on_custom_nm( const NMHDR& params_header )
        //    -> optional<LRESULT> override
        //{
        //    if( params_header.code ==  TRBN_THUMBPOSCHANGING ) {
        //        const auto& params = reinterpret_cast<const NMTRBTHUMBPOSCHANGING&>( params_header );

        //        const int pos = logical_pos_from_raw( params.dwPos );
        //        for( const Type_<Observer*> p_listener: m_observers ) {
        //            p_listener->on_new_position( pos );
        //        }
        //        return 0;
        //    }
        //    return {};
        //}

        static auto creation_style_bits_from( const Bitset_<Styles::Enum> styleset )
            -> WORD
        {
            const Truth is_vertical         = is_in( styleset, Styles::vertical );
            const Truth has_manual_ticks    = is_in( styleset, Styles::manual_ticks );
            const Truth has_ticks_ul        = is_in( styleset, Styles::ticks_ul );
            const Truth has_ticks_dr        = is_in( styleset, Styles::ticks_dr );
            const Truth has_ticks_bs        = has_ticks_ul and has_ticks_dr;

            // TBS_DOWNISLEFT has no effect, it's an MS bug. TBS_REVERSED is just a flag.
            WORD bits = TBS_NOTIFYBEFOREMOVE | WORD( +is_vertical*TBS_REVERSED );
            
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
            for( const auto p_observer: m_observers ) {
                p_observer->on_new_position( new_position );
            }
        }

        void on_scroll( const UINT code, const int raw_pos )
        {
            switch( code ) {
                case TB_THUMBPOSITION:
                case TB_THUMBTRACK: {
                    on_position_change( logical_pos_from_raw( raw_pos ) );
                    break;
                }
                default: {
                    assert( raw_pos == 0 );     // Per the documentation.
                    //const int new_pos = (is_zero( raw_pos )? position() : logical_pos_from_raw( raw_pos ));
                    on_position_change( position() );
                }
            }
        }

        void on_wm_hscroll( const HWND control, const UINT code, const int pos )
        {
            assert( control == handle() );
            on_scroll( code, pos );
        }

        void on_wm_vscroll( const HWND control, const UINT code, const int pos )
        {
            assert( control == handle() );
            on_scroll( code, pos );
        }

        auto on_message( const Message& m )
            -> LRESULT override
        {
            switch( m.message_id ) {
                WINAPI_CASE_WM( HSCROLL, m, on_wm_hscroll );
                WINAPI_CASE_WM( VSCROLL, m, on_wm_vscroll );
            }
            return Base_::on_message( m );
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

        void set_position( const int new_pos )
        {
            process_message( TBM_SETPOS, true, new_pos );
        }
            
        void add_observer( const Type_<Observer*> p_observer )
        {
            m_observers.insert( p_observer );
            p_observer->on_new_position( position() );
        }

        void remove_observer( const Type_<Observer*> p_observer )
        {
            m_observers.erase( p_observer );
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
        }
    };

}  // namespace winapi::gui
