#pragma once
#include <cppx-core-language/all.hpp>
#include <winapi/graphics/Font_owner_handle.hpp>
#include <winapi-header-wrappers/windows-h.hpp>

#include <utility>      // std::move

namespace winapi
{
    using namespace cppx::x_throwing;
    $use_std( move );

    inline auto create_default_font()
        -> Font_owner_handle
    {
        // See <url: https://stackoverflow.com/a/6057761/464581>

        // Get the system message box font information.
        NONCLIENTMETRICS ncm = { sizeof( ncm ) };
        SystemParametersInfo( SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0 );

        // Create a corresponding font.
        auto result = Font_owner_handle( CreateFontIndirect( &ncm.lfMessageFont ) );
        hopefully( not result.is_invalid() )
            or $fail( "CreateFontIndirect failed" );
        return result;
    }

    inline auto the_default_gui_font()
        -> HFONT
    {
        static const Font_owner_handle the_font = create_default_font();
        return the_font;
    }
}  // namespace winapi
