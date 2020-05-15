#pragma once
#include <winapi/Owning_handle_.hpp>

#include <winapi-header-wrappers/windows-h.hpp>

namespace winapi {

    class Font_owner_handle:
        public winapi::Owning_handle_<Font_owner_handle, HFONT>
    {
        friend class Owning_handle_;

        static void destroy( const Api_handle handle )
        {
            ::DeleteObject( handle );
        }

    public:
        using Owning_handle_::Owning_handle_;
    };

}  // namespace winapi
