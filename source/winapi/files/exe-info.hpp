#pragma once    // Source encoding: UTF-8 with BOM (π is a lowercase Greek "pi").
#include <cppx-core-language/all.hpp>

#include <winapi-header-wrappers/windows-h.hpp>

namespace winapi::files {
    using namespace cppx::x_throwing;
    $use_cppx( hopefully, Byte, Size, Type_ );

    namespace impl {
        template< class New, class Current >
        inline auto advance_to_( const Type_<const Current*> p, const Size offset )
            -> const New*
        {
            return reinterpret_cast<const New*>(
                offset + reinterpret_cast<const Byte*>( p )
                );
        }
    }  // namespace impl

    inline auto header_from_pe_executable( const Type_<const void*> p_image )
        -> const IMAGE_DOS_HEADER*
    {
        const auto p_header = reinterpret_cast<const IMAGE_DOS_HEADER*>( p_image );
        hopefully( p_header->e_magic == IMAGE_DOS_SIGNATURE )
            or $fail( "The image doesn't start with \"MZ\" as magic number." );
        return p_header;
    }

    inline auto this_executable_image()
        -> const IMAGE_DOS_HEADER*
    { return header_from_pe_executable( GetModuleHandle( 0 ) ); }

    inline auto nt64_header_from( const Type_<const IMAGE_DOS_HEADER*> p_dos_header )
        -> const IMAGE_NT_HEADERS64*
    {
        const auto p_nt_64 = impl::advance_to_<IMAGE_NT_HEADERS64>(
            p_dos_header, p_dos_header->e_lfanew
            );
        hopefully( p_nt_64->Signature  == IMAGE_NT_SIGNATURE )
            or $fail( "Alleged PE header doesn't start with \"PE\" as magic number." );
        hopefully( p_nt_64->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC )
            or $fail( "The image is not a 64-bit executable" );
        return p_nt_64;
    }

    inline auto subsystem_of( const Type_<const IMAGE_DOS_HEADER*> exec_image )
        -> WORD
    { return nt64_header_from( exec_image )->OptionalHeader.Subsystem; }

    inline auto is_gui_app(
        const Type_<const IMAGE_DOS_HEADER*> exec_image = this_executable_image()
        ) -> bool
    { return (subsystem_of( exec_image ) == IMAGE_SUBSYSTEM_WINDOWS_GUI); }

}  // namespace winapi::files
