#include <dv/sys-interface/gui/graphics-size-util.hpp>

#include <stdlib/c/math.hpp>        // abs, round
#include <winapi-header-wrappers/windows-h.hpp>

namespace ns = sys::gui;

extern auto ns::pixel_size_from_points( const double n_points )
    -> int
{
    const HDC dc = GetDC( 0 );
    const double result = round( n_points )*GetDeviceCaps( dc, LOGPIXELSY )/72.0;
    ReleaseDC( 0, dc );
    return static_cast<int>( result );
}

extern auto ns::point_size_from_pixels( const int n_pixels )
    -> double
{
    const HDC dc = GetDC( 0 );
    const double result = 72.0*n_pixels/GetDeviceCaps( dc, LOGPIXELSY );
    ReleaseDC( 0, dc );
    return result;
}
