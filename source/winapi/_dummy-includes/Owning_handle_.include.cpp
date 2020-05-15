#include <winapi/Owning_handle_.hpp>

namespace {
    class Hi:
        winapi::Owning_handle_<Hi, int>
    {
        friend class Owning_handle_;
        static void destroy( const int ) {}
    public:
        Hi( int x ): Owning_handle_( x ) {}
    };
    const Hi dummy = 42;
}
