#pragma once    // Source encoding: UTF-8 with BOM (π is a lowercase Greek "pi").
#include <cppx-core-language/all.hpp>

#include <winapi/Hybrid_little_id.hpp>

namespace winapi::resources::rc {
    using namespace cppx::x_throwing;
    $use_cppx( Byte, Array_span_, span_of );

    auto access(
        const Hybrid_little_id      resource_id,
        const Hybrid_little_id      kind,
        const HMODULE               module      = 0
        ) -> Array_span_<const Byte>
    {
        const HRSRC location = ::FindResource( module, resource_id.pseudo_ptr(), kind.pseudo_ptr() );
        hopefully( location != 0 )
            or $fail( "FindResource failed" );

        const Size size = SizeofResource( module, location );
        hopefully( size > 0 )
            or $fail( "SizeofResource failed" );

        const HGLOBAL handle = ::LoadResource( module, location );
        hopefully( handle != 0 )
            or $fail( "LoadResource failed" );

        const Type_<const void*> pointer = ::LockResource( handle );
        hopefully( pointer != 0 )
            or $fail( "LockResource failed" );

        return span_of( reinterpret_cast<const Byte*>( pointer ), size );
    }
}  // namespace winapi::resources::rc
