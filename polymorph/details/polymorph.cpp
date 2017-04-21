#include "../polymorph.h"

namespace helpers
{

polymorph::polymorph( const polymorph& other ) noexcept : m_data( other.m_data )
{

}

polymorph::polymorph( polymorph&& other ) noexcept : m_data( std::move( other.m_data ) )
{

}

polymorph& polymorph::operator=( const polymorph& other ) noexcept
{
    m_data = other.m_data;
    return *this;
}

polymorph& polymorph::operator=( polymorph&& other ) noexcept
{
    m_data = std::move( other.m_data );
    return *this;
}

const std::type_info &polymorph::type_info() const noexcept
{
  return empty()? typeid( void ) : m_data->type_info();
}

bool polymorph::empty() const noexcept
{
    return ( m_data == nullptr );
}

}// helpers
