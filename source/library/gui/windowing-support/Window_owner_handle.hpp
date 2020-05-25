#pragma once    // Source encoding: UTF-8 with BOM (π is a lowercase Greek "pi").
#include <winapi/Owning_handle_.hpp>

#include <winapi-header-wrappers/windows-h.hpp>

namespace winapi {

    class Window_owner_handle:
        public winapi::Owning_handle_<Window_owner_handle, HWND>
    {
        friend class Owning_handle_;

        static void destroy( const Api_handle handle )
        {
            ::DestroyWindow( handle );
        }

    public:
        using Owning_handle_::Owning_handle_;
    };

}  // namespace winapi
