#ifndef ARGS_SIZE_HELPER
#define ARGS_SIZE_HELPER

namespace helpers
{

namespace tuple
{

template< typename... Args >
constexpr size_t args_size()
{
    return sizeof...( Args );
}

}

}

#endif
