#ifndef MOCK_HELPERS
#define MOCK_HELPERS

#include <type_traits>
#define unused( x ) ( void )( x )

namespace test
{

namespace mock
{

namespace details
{

template< class T >
T mock_type()
{
    static_assert( std::is_reference< T >::value || std::is_pointer< T >::value,
                   "Only pointers or references are allowed to be mocked" );

    unused( static size_t i{ 1 } );
    return reinterpret_cast< T >( i );
}

}

template< class T >
T& mock_ref()
{
    return details::mock_type< T& >();
}

template< class T >
const T& mock_const_ref()
{
    return details::mock_type< const T& >();
}

template< class T >
T* mock_valid_ptr()
{
    return details::mock_type< T* >();
}

template< class T >
const T* mock_valid_const_ptr()
{
    return details::mock_type< const T* >();
}

}

}

#endif
