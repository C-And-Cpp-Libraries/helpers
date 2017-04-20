#ifndef HELPERS_VARIANT
#define HELPERS_VARIANT

#include <memory>
#include <typeinfo>

namespace helpers
{

namespace utility
{

namespace details
{

class base_type_storage
{
private:
    struct access{};

protected:
    explicit base_type_storage( const access& ){}

public:
    virtual const std::type_info&  type_info() const noexcept = 0;

public:
    virtual ~base_type_storage() = default;
};

template< typename T >
class type_storage;

}// details

class variant
{
public:
    variant() = default;

    template< typename T,
              typename = typename std::enable_if< !std::is_same< T, variant& >::value >::type >
    explicit variant( T&& t ) : m_data( details::type_storage< typename std::decay< T >::type >::
                                          create( std::forward< T >( t ) ) ){}

    explicit variant( const variant& other ) : m_data( other.m_data ){}
    variant& operator=( const variant& other ){ m_data = other.m_data; }
    explicit variant( variant&& other ) : m_data( std::move( other.m_data ) ){}
    variant& operator=( variant&& other ){ m_data = std::move( other.m_data ); }

    template< class T >
    T& get()
    {
        auto data_casted = try_cast< T >();
        return data_casted->get();
    }

    template< class T >
    const T& get() const
    {
        auto data_casted = try_cast< T >();
        return data_casted->get();
    }

    template< typename T >
    bool check_type() const noexcept
    {
        return typeid( T ) == m_data->type_info();
    }

    bool empty() const noexcept{ return ( m_data == nullptr ); }

private:
    template< typename T >
    std::shared_ptr< details::type_storage< T > > try_cast() const
    {
        auto data_casted = std::dynamic_pointer_cast< details::type_storage< T > >( m_data );
        if( !data_casted )
        {
            throw std::bad_cast{};
        }

        return data_casted;
    }


private:
    std::shared_ptr< details::base_type_storage > m_data;
};

namespace details
{

template< typename T >
class type_storage final : public base_type_storage,
                           public std::enable_shared_from_this< type_storage< T > >
{
    using base_type = T;
    using base_unref_type = typename std::remove_reference< T >::type;
    friend class helpers::variant;

public:
    type_storage() = delete;

private:
    explicit type_storage( const base_unref_type& data ) :
        base_type_storage( base_type_storage::access{} ), m_data( data ){}

    explicit type_storage( base_unref_type&& data ) :
        base_type_storage( base_type_storage::access{} ), m_data( std::move( data ) ){}

    static std::unique_ptr< type_storage< T > > create( const base_unref_type& data )
    {
        return std::unique_ptr< type_storage< T > >( new type_storage< T >{ data } );
    }

    static std::unique_ptr< type_storage< T > > create( base_unref_type&& data )
    {
        return std::unique_ptr< type_storage< T > >( new type_storage< T >{ std::move( data ) } );
    }

    const std::type_info& type_info() const noexcept{ return typeid( base_type ); }

public:
    void set( const base_unref_type& data ){ m_data = data; }
    void set( base_unref_type&& data ){ m_data = std::move( data ); }

    const base_type& get() const{ return m_data; }
    base_type& get(){ return m_data; }

private:
    base_type m_data;
};

}// details

}// utility

}// helpers