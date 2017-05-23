# Helpers
A small set of helper functions, classes, tiny frameworks and type traits useful in various situations

## benchmarking
```
template< duration_result_type > 
auto measure_exec_time( Func&&, Args&&... ) -> duration_result_type
```
A wrapper around called function, returns elapsed time in specified std::duration. Uses std::chrono::high_resolution_clock under the hood.

```
template< duration_result_type >
auto measure_exec_time( Func&&, Args&&... ) 
-> std::pair< duration_result_type, std::result_of< Func(Args...) > >
``` 
Same as above, but for functions with non-void return values.

#### class scope_time_handle
```
template< duration_type, clock_type >
class scope_time_handle
```
A non-copyable and non-movable RAII class used to call ```execute_on_destroy``` callback of type ```std::function< void( const duration_type& ) >``` upon it's destruction. IMPORTANT: callback must not throw any exceptions, as it's called in destructor

```
explicit scope_time_handle::scope_time_handle( const pred_type& execute_on_destroy )
```
Basic constructor. ```execute_on_destroy``` will be called upon the object's destruction.

The following predefined typedefs are provided:

* ```nanosec_scope_time_handle``` 
* ```microsec_scope_time_handle``` 
* ```millisec_scope_time_handle``` 
* ```sec_scope_time_handle``` 
* ```min_scope_time_handle``` 
* ```hour_scope_time_handle``` 

#### class sample_storage
```
template< typename key,
          typename sample_duration = std::chrono::nanoseconds,
          typename clock = std::chrono::high_resolution_clock >
class sample_storage
``` 
A thread safe time duration sample storage class, capable of storing time data sorted by keys. Can calculate average time for the specified key.

```
explicit sample_storage( size_t max_samples = 0 )
```
If ```max_samples``` is 0, the number of samples is unlimited, otherwise the circular buffer approach will be used.

```
void add_timestamp( key_type key )
```
Adds new timestamp. The initial timestamp of the pair (start time, end time) is placed in temporary storage, and upon the addition of the second timestamp the duration(end time - start time) is added to the main storage. 

Throws:
* ```std::overflow_error``` if the sum of all durations related to the key may overflow the variable used to store it.

```
bool remove_key_data( key_type key ) noexcept
```
Removes time data related to key. Returns true if any data was removed.

```
sample_type average_time( key_type key ) const
```
Returns average time for the key. 

Throws:
* ```std::out_of_range``` if key is not found.

```
const std::deque< sample_type >& samples( key_type key ) const
```
Returns timestamps related to key.

Throws:
* ```std::out_of_range``` if key is not found.

```
bool key_present( key_type key ) const noexcept
```
Checks if key is present.

```
static sample_storage< key_type, sample_type, clock_type >* instance() noexcept
```
Returns pointer to static instance of the storage with the given template parameters.

## concurrency

#### class thread_pool
```
class thread_pool
``` 
A non-copyable, non movable thread pool class with basic functionality.

```
explicit thread_pool( size_t workers_number = std::thread::hardware_concurrency() )
``` 
Basic constructor.

```
template< typename Func, typename... Args >
auto add_task( Func&& func, Args&&... args  ) -> std::future< std::result_of< Func( Args... ) > >
```
Adds new task to queue, returns it's future. 

Throws:
* ```std::logic_error``` if obejct is being destroyed.

```
void clean_pending_tasks()
```
Removes all pending tasks, those being already executed are not affected.

```
size_t total_tasks_number() const noexcept
```
Returns total number of tasks, both pending and being executed.

```
size_t queue_size() const noexcept
```
Returns number of pending tasks.

```
 void add_workers( uint32_t number )
```
Adds threads to the pool. 

Throws:
* ```std::logic_error``` if obejct is being destroyed.

```
void schedule_remove_workers( uint32_t number )
```
Schedules threads to be removed from the pool. Threads will be stopped only after finishing their current job. The actual stopped threads' objects will be removed after adding a new job. 

Throws:
* ```std::logic_error``` if obejct is being destroyed.
* ```std::invalid_argument``` if number of workers to be removed is greater than the present number of workers.

```
template< typename TimeoutType = std::chrono::milliseconds >
bool wait_until_finished( const TimeoutType& timeout = TimeoutType{ 0 } ) const
```
Blocks until all tasks are finished. On timeout returns false

```
size_t workers_to_remove() const noexcept
```
Returns number of threads scheduled to be removed.

```
static thread_pool& get_instance()
```
Static thread_pool created with default constructor.

## polymorph

#### class polymorph
```
class polymorph
``` 
A type erasure storage class similar to boost::any or QVariant, but with implicit data sharing.

```
polymorph() = default
polymorph( const polymorph& other ) noexcept
polymorph( polymorph&& other ) noexcept
polymorph& operator=( const polymorph& other ) noexcept
polymorph& operator=( polymorph&& other ) noexcept
```
Basic constructors and operatos. Copy or move only the pointer to the data.

```
template< typename T, typename = disable_if_polymorph< T > >
polymorph( T&& t )

template< typename T, typename = disable_if_polymorph< T > >
polymorph& operator=( T&& t )
```
Internal data assignment constructor and operator for types different from the class itself.

```
template< class T >
polymorph& deep_copy( const polymorph& source )
```
Creates a deep copy of source's data instead of shared pointer copy.

```
template< class T >
T& get()

template< class T >
const T& get() const
```
Return a reference to the stored data.

Throws:
* ```polymorph_empty``` if the class is empty

```
template< class T >
T& get_unsafe()

template< class T >
const T& get_unsafe() const
```
Return a reference to the stored data. Use static_cast under the hood, no runtime checks are performed(might be dangerous).

```
template< typename T >
bool check_type() const noexcept
```
Returns true if the stored type is similar to ```T```.

```
const std::type_info& type_info() const noexcept
```
Returns type_info of the stored data(or ```typeid( void )``` if empty ).

```
bool empty() const noexcept
```
Returns true if empty, otherwise returns false.

## temporal

#### enum since
* ```epoch``` - since 1970.
* ```julian``` - since julian day 0.

#### class time_ratio
```
template< time_type tick_cnt, time_type sec_period >
class time_ratio final
```
General class for time representation. ```tick_cnt``` is tick count per ```sec_period```( period in seconds ),
so for example to represent minutes ```tick_cnt``` = 1, ```sec_period``` = 60.

```
using curr_std_duration_type = std::chrono::duration<
    std::conditional< ( sec_period > tick_cnt ), int, int64_t >::type,
    std::ratio< sec_period, tick_cnt > >;    
```
Represents ```std::duration``` for the similar units.

```
constexpr time_ratio() = default
```
Default constructor

```
constexpr explicit time_ratio( time_type count ) noexcept
```
Creates time from the specified number of units.

```
constexpr explicit time_ratio( const time_moment& tm ) noexcept
```
Creates time from ```time_moment``` class(since epoch).

```
constexpr explicit time_ratio( const date_time& dt ) noexcept
```
Creates time from ```date_time``` class(since epoch).

```
constexpr explicit time_ratio( const timeval& tv ) noexcept
```
Creates time from ```timeval``` class(since epoch).

```
constexpr explicit time_ratio( const timespec& ts ) noexcept
```
Creates time from ```timespec``` class(since epoch).

```
template< std_duration_type >
constexpr explicit time_ratio( const std_duration_type& duration, const since& start_point ) noexcept
```
Creates time from ```std::duration``` class. Depending on ```start_point``` the duration may be added to julian day 0  or epoch year.

```
void from_time_t( time_t time ) noexcept
void from_timeval( const timeval& time ) noexcept
void from_timespec( const timespec& time ) noexcept

template< std_duration_type >
void from_std_duration( const std_duration_type& duration, const since& start_point ) noexcept
```
Convenience functions.

```
static time_ratio< tick_cnt, sec_period > now() noexcept
```
Returns time_ratio of the current type initialized with the current time since julian day 0.

```
constexpr time_t to_time_t() const noexcept;
constexpr timeval to_timeval() const noexcept;
constexpr timespec to_timespec() const noexcept;

template< std_duration_type = curr_std_duration_type  >
constexpr std_duration_type to_std_duration() const noexcept; // since julian
```
Conversions.

```
operator curr_std_duration_type() const noexcept
```
Implicit conversion to the corresponding ```std::duration```.

```
constexpr time_type count() const noexcept
```
Returns number of unuts for the stored time since julian day 0.

```
 time_type count_since_epoch() const noexcept
```
Returns number of unuts for the stored time since epoch.

```
constexpr date_time to_date_time() const noexcept
```
Creates date_time class initialized with the stored time.

```
static constexpr time_ratio< tick_cnt, sec_period > max() noexcept
```
Returns an instance of the class initialized with the maximum time value that doesn't cause count() overflow.

#### struct verbose_date_time
```
struct verbose_date_time
{
    time_type day{ 0 };
    date_time::dt_month month{ date_time::jan };
    int64_t year{ 0 };
    time_type hour{ 0 };
    time_type min{ 0 };
    time_type sec{ 0 };
    time_type nsec{ 0 };
}
```
A convenience struct to be able to query all date and time components at once.

#### struct date_time
```
class date_time final
```
A general class for date and time representation. Provides the means to convert time_moment to date and time, and vice versa. Is able to serialize date and time into std::string

```
constexpr date_time() = default;
constexpr explicit date_time( const details::time_moment& tm )
date_time( int64_t year, dt_month month, time_type day,
           time_type hour, time_type min, time_type sec, time_type nsec )

explicit date_time( const verbose_date_time& vdt )
```
Basic constructors. 

Throws:
* ```invalid_argument``` if some components are invalid.

```
verbose_date_time to_verbose_date_time() const noexcept
```
Conversion to ```verbose_date_time```.

```
int64_t year() const noexcept
dt_month month() const noexcept
time_type day() const noexcept
time_type hour() const noexcept
time_type minute() const noexcept
time_type sec() const noexcept
time_type nsec() const noexcept
int day_of_week() const noexcept
```
Time and date component getters.

```
bool is_leap() const noexcept
```
Checks if the year stored in class is leap.

```
std::string to_string( const std::string& pattern = {} ) const noexcept
```
Serializes time to string. Replaces the first occurrence of pattern with the corresponding value. The default pattern used is "w mn d hr:min:sec:ms yr"

Avaliable patterns:
* ```yr``` - year
* ```mn``` - month
* ```d``` - day
* ```w``` - day of weak
* ```hr``` - hour
* ```min``` - minute
* ```sec``` - second
* ```ms``` - millisec
* ```us``` - microsec
* ```ns``` - nanosec

## type_traits
  * ```is_duration< T >``` - checks if type is std::chrono::duration.
  * ```enable_if_duration< T >``` - enable_if for ```is_duration```.
  * ```is_clock< T >``` - checks if type conforms the typedefs for clocks from std::chrono.
  * ```enable_if_clock< T >``` - enable_if for ```is_clock```.
  * ```returns_type< result_type, Func, Args... >``` - checks if function's return type is ```result_type```.
  * ```disable_if< val, type >``` - opposite to ```enable_if```.
  * ```enable_if_returns_type< result_type, Func, Args... >``` - enable_if for returns_type.
  * ```disable_if_returns_type< result_type, Func, Args... >``` - not(enable_if) for returns_type.
  
## classes
  * ```non_copyable``` - base class for non-copyable classes
  * ```non_movable``` - base class for non-movable classes
  * ```non_copyable_non_movable``` - base class for non-copyable and non-movable classes

## tuple
  * ```args_size< Args... >``` - returns size of args.
  * ```first_of_type< Target, startindex >( Tuple )``` - returns position of first arg of type ```Target```, starting from ```start_index```.
  * ```for_each_of_type< Target, Func, start_index >( Tuple )``` - applies Func to each member of tuple of type ```Target```, starting from ```start_index```.
  * ```tuple_find< start_index, Target >( target, Tuple )``` - finds position of thr first member of tuple of type ```Target``` equal to ```target```, starting from ```start_index```.
  * ```type_present< Target >( Tuple )``` - checks if element of type ```Target``` is present in ```Tuple```.
  

## test
Lightweight and simple test framework

### Asserts and wrappers
* ```DYNAMIC_ASSERT( val )``` - Evaluates condition, throws ```test_error``` with assert info on fail
* ```DYNAMIC_ASSERT( val, error )``` - Same as above, but uses ```error``` as error text.

* ```CHECK_NOTHROW( Func )``` - a wrapper around tested function, checks it's exception behaviour. Throws ```test_error``` on fail.
* ```CHECK_THROW( Func )``` - a wrapper around tested function, checks it's exception behaviour. Throws ```test_error``` on fail.

* ```TEST_CASE( test_name )``` - Should be used as a wrapper around tests, automatically launches them.
* ```TEST_CASE_FIXTURE( test_name, fuxture_name )``` - Same as above, but with fixtures support. ```fixture_name``` should be the name of an existing struct with a default constructor. All public variables and functions of the fixture are directly available in the scope of the test case.

* ```HELPERS_MAIN``` - A define expanding to main() function. Should be defined before the test_helpers.h include and only in one translation unit.

### Mock
* ```mock_ref< T >``` - mocks reference of type ```T```
* ```mock_const_ref< T >``` - mocks const reference of type ```T```
* ```mock_valid_ptr< T >``` - mocks non-null pointer to type ```T```
* ```mock_valid_const_ptr< T >``` - mocks non-null const pointer to type ```T```
