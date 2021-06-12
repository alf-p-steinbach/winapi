#pragma once    // Source encoding: UTF-8 with BOM (π is a lowercase Greek "pi").
#include <winapi/Hybrid_little_id.hpp>

namespace winapi::gui
{
    class Windowclass_id:
        public Hybrid_little_id
    {
    public:
        using Hybrid_little_id::Hybrid_little_id;
    };

}  // namespace winapi::gui
