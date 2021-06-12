#pragma once    // Source encoding: UTF_8 with BOM (π is a lowercase Greek "pi").

#ifndef     _MSC_VER
#   error   "This header is for the Visual C++ compiler only."
#endif

#include <winapi/compiler-support/msvc/runtime_library/Envelope.hpp>

namespace uuiid_355137b9_6408_4803_9dcf_5e5f120ff290 {
    using namespace winapi::msvc;

    inline const bool dummy = (
        runtime_library::Envelope::global_instantiation(),
        true
        );

}  // namespace <uuid>
