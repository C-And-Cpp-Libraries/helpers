#ifndef _HELPERS_ARGS_SIZE_H_
#define _HELPERS_ARGS_SIZE_H_

namespace helpers
{

namespace tuple
{

template< typename... Args >
constexpr size_t args_size()
{
    return sizeof...( Args );
}

}// tuple

}// helpers

#endif
