#pragma once    // Source encoding: UTF-8 with BOM (π is a lowercase Greek "pi").
#include <cppx-core-language/all.hpp>

#include <winapi-header-wrappers/commctrl-h.hpp>
#include <winapi/text/encoding-conversions.hpp>                             // winapi::text::to_wide
#include <winapi/gui/windowing-support/enabled_main_window_in_thread.hpp>   // winapi::gui::enabled_main_window_in_thread

#include <string_view>
#include <variant>
#include <vector>

namespace winapi::gui {
    $alias_cppx_namespaces( ascii );
    using namespace cppx::x_throwing;

    $use_cppx( is_same_type_, Type_ );
    $use_std( holds_alternative, get, string_view, variant, vector, wstring );

    namespace td {
        struct Icon{ enum Enum{ warning, error, information, shield, question }; };

        struct Icon_resource_spec
        {
            ATOM            resource_id;
            HINSTANCE       module  = ::GetModuleHandle( 0 );
        };

        using Icon_spec = variant<Icon::Enum, Icon_resource_spec>;
    }  // namespace td

    static_assert( is_same_type_<TASKDIALOG_COMMON_BUTTON_FLAGS, int> );

    inline auto task_dialog(
        const HWND                              desired_owner_window,
        const string_view&                      title,
        const string_view&                      main_text,
        const string_view&                      details_text,
        const td::Icon_spec                     icon        = td::Icon::information,
        const TASKDIALOG_COMMON_BUTTON_FLAGS    buttons     = TDCBF_OK_BUTTON
        ) -> int        // const int *pnButton
    {
        const bool icon_from_rsrc   = holds_alternative<td::Icon_resource_spec>( icon );
        const bool is_question      = (not icon_from_rsrc) and (get<td::Icon::Enum>( icon ) == td::Icon::question);

        constexpr auto resource_id_of = []( auto& var )
            -> wchar_t*
        { return MAKEINTRESOURCEW( get<td::Icon_resource_spec>( var ).resource_id ); };

        static const Type_<wchar_t*> icon_id[] =
        {
            TD_WARNING_ICON, TD_ERROR_ICON, TD_INFORMATION_ICON, TD_SHIELD_ICON
        };

        const bool  owner_specified = (desired_owner_window != 0);
        const HWND  owner_window    = (owner_specified? desired_owner_window: enabled_main_window_in_thread());

        //const HRESULT hr = TaskDialog(
        //    owner_window,
        //    icon_from_rsrc? get<td::Icon_resource_spec>( icon ).module : nullptr,
        //    ascii::to_wide( title ).c_str(),
        //    ascii::to_wide( main_text ).c_str(),
        //    ascii::to_wide( details_text ).c_str(),
        //    buttons,
        //    icon_from_rsrc? resource_id_of( icon ) : icon_id[get<td::Icon::Enum>( icon )],
        //    &button_id
        //    );

        TASKDIALOGCONFIG params = {sizeof( params )};

        const auto wide_title       = text::to_wide( title );
        const auto wide_main_text   = text::to_wide( main_text );
        const auto wide_details     = text::to_wide( details_text );
        $with( params ) {
            _.hwndParent                = owner_window;
            _.hInstance                 = icon_from_rsrc? get<td::Icon_resource_spec>( icon ).module : 0;
            _.dwFlags                   = TDF_EXPANDED_BY_DEFAULT | TDF_SIZE_TO_CONTENT | is_question*TDF_USE_HICON_MAIN;
            _.dwCommonButtons           = buttons;
            _.pszWindowTitle            = wide_title.c_str();
            _.hMainIcon                 = (is_question? ::LoadIcon( 0, IDI_QUESTION) : 0);
            if( not is_question ) {     // .hMainIcon and .pszMainIcon are in a `union`, same memory.
                _.pszMainIcon               = (icon_from_rsrc? resource_id_of( icon ) : icon_id[get<td::Icon::Enum>( icon )]);
            }
            _.pszMainInstruction        = wide_main_text.c_str();
            _.pszContent                = wide_details.c_str();
            _.cButtons                  = 0;
            _.pButtons                  = nullptr;
            _.nDefaultButton            = 0;
            _.cRadioButtons             = 0;
            _.pRadioButtons             = nullptr;
            _.nDefaultRadioButton       = 0;
            _.pszVerificationText       = nullptr;
            _.pszExpandedInformation    = nullptr;      // Text
            _.pszExpandedControlText    = nullptr;
            _.hFooterIcon               = 0;
            _.pszFooterIcon             = nullptr;
            _.pszFooter                 = nullptr;
        }
        int button_id;
        const HRESULT hr = ::TaskDialogIndirect( &params, &button_id, nullptr, nullptr );
        hopefully( SUCCEEDED( hr ) )
            or $fail( "TaskDialog failed" );
        return button_id;
    }
}  // namespace winapi::gui
