#ifndef _HELPERS_TEMPORAL_H_
#define _HELPERS_TEMPORAL_H

#include "impl/time_ratio.h"

namespace helpers
{

namespace temporal
{

using nanosec_t  = details::time_ratio< ratio::ns_ratio_sec,    1 >;
using microsec_t = details::time_ratio< ratio::micro_ratio_sec, 1 >;
using millisec_t = details::time_ratio< ratio::milli_ratio_sec, 1 >;
using seconds_t  = details::time_ratio< ratio::sec_ratio_sec,   1 >;
using minutes_t  = details::time_ratio< 1, ratio::min_ratio_sec >;
using hours_t    = details::time_ratio< 1, ratio::hour_ratio_sec >;
using days_t     = details::time_ratio< 1, ratio::day_ratio_sec >;

}// temporal

}// helpers

#endif
