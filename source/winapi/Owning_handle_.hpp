#pragma once    // Source encoding: UTF-8 with BOM (π is a lowercase Greek "pi").
#include <cppx-core-language/all.hpp>

#include <utility>      // std::(exchange, move)

namespace winapi {
    $use_cppx( No_copy );
    $use_std( exchange, move );

    template<
        class Derived,
        class Api_handle_,
        Api_handle_ invalid_ = Api_handle_()
    >
        class Owning_handle_:
        public No_copy
    {
    public:
        using Api_handle = Api_handle_;
        static constexpr Api_handle invalid = invalid_;

    private:
        Api_handle      m_value;

    protected:
        ~Owning_handle_()
        {
            if( m_value != invalid ) {
                Derived::destroy( m_value );
            }
        }

    public:
        explicit Owning_handle_( const Api_handle value = invalid ) noexcept:
            m_value( value )
        {}

        Owning_handle_( Owning_handle_&& other ) noexcept:
            m_value( exchange( other.m_value, invalid ) )
        {}

        auto operator=( Owning_handle_&& other ) noexcept
            -> Owning_handle_&
        {
            m_value = exchange( other.m_value, invalid );
            return *this;
        }

        auto is_invalid() const noexcept
            -> bool
        { return m_value == invalid; }

        auto release() noexcept
            -> Api_handle
        { return exchange( m_value, invalid ); }

        auto value() const noexcept
            -> Api_handle
        { return m_value; }

        operator Api_handle() const noexcept
        {
            return m_value;
        }
    };

}  // namespace winapi
