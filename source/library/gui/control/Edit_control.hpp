#pragma once
#include <cppx/text/C_string_ref.hpp>
#include <cppx/Option_value_.hpp>
#include <stdlib/extension/type_builders.hpp>
#include <stdlib/extension/hopefully_and_fail.hpp>
#include <stdlib/initializer_list.hpp>
#include <stdlib/string_view.hpp>
#include <stdlib/tuple.hpp>
#include <winapi/gui/window/Control.hpp>

namespace name
{
     CPPX_DEFINE_NAMETYPE( Features );
     CPPX_DEFINE_NAMETYPE( Alignment );
     CPPX_DEFINE_NAMETYPE( Restriction );
}  // namespace name

namespace winapi::gui {
    using cppx::C_widestring_ref;
    using cppx::Option_value_;
    using cppx::item;
    using std::initializer_list;
    using std::tuple;
    using std::wstring_view;

    using namespace stdlib::ext::type_builders;

    using Edit_control_base = winapi::gui::Control;
    class Edit_control
        : public Edit_control_base
    {
        using Base = Edit_control_base;
        class Api_window_factory;           // Defined out-of-class below.

    public:
        enum Features       // multi
        {
            multiline               = ES_MULTILINE,
            no_hide_selection       = ES_NOHIDESEL,
            read_only               = ES_READONLY,
            want_return_key_message = ES_WANTRETURN,
            auto_hscroll            = ES_AUTOHSCROLL,
            auto_vscroll            = ES_AUTOVSCROLL,
        };
        enum Alignment      // single
        {
            align_left              = ES_LEFT,
            align_center            = ES_CENTER,
            align_right             = ES_RIGHT,
        };
        enum Restriction    // single
        {
            unrestricted            = 0,
            lowercase               = ES_LOWERCASE,
            uppercase               = ES_UPPERCASE,
            digits_only             = ES_NUMBER,
            password                = ES_PASSWORD,
        };

        class Options
            : public Option_value_< Features,       name::Features >
            , public Option_value_< Alignment,      name::Alignment >
            , public Option_value_< Restriction,    name::Restriction >
        {
        public:
            auto features() const       -> Features     { return item<name::Features>( *this ); }
            auto alignment() const      -> Alignment    { return item<name::Alignment>( *this ); }
            auto restriction() const    -> Restriction  { return item<name::Restriction>( *this ); }

            auto style_bits() const
                -> DWORD
            { return features() | alignment() | restriction(); }
        };

        ~Edit_control() override {}

        inline Edit_control( const HWND parent, ref_<const Options> options = {} );
        inline Edit_control( ref_<const Api_window_factory> factory );
    };

    inline auto operator|(
        const Edit_control::Features a,
        const Edit_control::Features b
        )
        -> Edit_control::Features
    { return Edit_control::Features( a | b ); }

    class Edit_control::Api_window_factory
        : public Base::Api_window_factory
    {
    private:
        Options     m_options;

    public:
        virtual auto window_class() const
            -> Api_windowclass_id
        { return Api_windowclass_id{ L"Edit" }; }

        auto options() const
            -> Options
        { return m_options; }

        void set( ref_<CREATESTRUCT> _ ) const
            override
        {
            Base::Api_window_factory::set( _ );
            _.style |= options().style_bits();
        }

        Api_window_factory(
            const HWND              parent,
            ref_<const Options>     options = Options{}
            )
            : Base::Api_window_factory{ parent }
            , m_options{ options }
        {}

        template< class... Items >
        Api_window_factory(
            const HWND                      parent,
            ref_<const tuple<Items...>>     options = {}
            )
            : Base::Api_window_factory{ parent }
            , m_options{}
        {
            // TODO:
        }
    };

    Edit_control::Edit_control(
        const HWND              parent,
        ref_<const Options>     options
        )
        : Base{ Api_window_factory{ parent, options } }
    {}

    Edit_control::Edit_control( ref_<const Api_window_factory> factory )
        : Base{ factory }
    {}

}  // namespace winapi::gui::control
