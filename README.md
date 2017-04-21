# Helpers
A small set of helper functions, tiny frameworks and type traits useful in various situations

## benchmarking

* ```measure_exec_time( func, args.. ) -> Duration``` - a wrapper around called function, returns elapsed time.
* ```measure_exec_time( func, args... ) -> < Duration, RESULT OF FUNC >``` - same as above, but for functions with non-void return values.

## chrono

### type_traits
  * ```is_duration< T >``` - checks if type is std::chrono::duration
  
## classes

  * ```non_copyable``` - base class for non-copyable classes
  * ```non_movable``` - base class for non-movable classes
  * ```singleton``` - base class for singletons
  
## functional

### type_traits
  * ```returns_void< Func, Args... >``` - checks if function return type is void

## tuple
  * ```args_size< Args... >``` - returns size of args
  * ```first_of_type< Target, startindex >( Tuple )``` - returns position of first arg of type ```Target```, starting from ```start_index```
  * ```for_each_of_type< Target, Func, start_index >( Tuple )``` - applies Func for each member of tuple of type ```Target```, starting from ```start_index```
  * ```tuple_find< start_index, Target >( target, Tuple )``` - finds position of first member of tuple of type ```Target``` equal to ```target```, starting from ```start_index```
  * ```type_present< Target >( Tuple )``` - checks if element of type ```Target``` is present in ```Tuple```
  

## test
Lightweight and simple test framework

### Classes
* ```TEST_RESULT``` - Contains test name, result and possible error message

* ```TEST_RESULTS_STORAGE``` - Stores ```TEST_RESULT``` objects during testing.
* * ```TEST_RESULTS_STORAGE::get()``` - Returns ```TEST_RESULTS``` object ref containing ```TEST_RESULT``` objects .
* * ```TEST_RESULTS_STORAGE::has_fails()``` - Returns if any of tests failed.
* *```TEST_RESULTS_STORAGE::reset()``` - Clears all stored results.

### Enums
* ```SHOULD_THROW``` - used in ```TEST_EXEC_FUNC```, defines expected tested func behaviour.
* * ```YES``` - tested func should throw an exception during it's call.
* * ```NO``` - tested func should not throw an exception during it's call.

### Asserts and wrappers
* ```TEST_DYNAMIC_ASSERT( condition, error )``` - Evaluates condition, throws ```test_error``` on fail

* ```TEST_EXEC_FUNC( testname, THROW, func, args.. )``` - a wrapper around tested function, checks it's throw behaviour. Throws ```test_error``` on fail.

* ```TEST_EXEC_FUNC( testname, THROW, func, args... ) -> < RESULT OF FUNC >``` - same as above, but for functions with non-void return values.

* ```TEST_CASE( testname, test_func, test_args... )``` - a wrapper around actual test func, puts ```TEST_RESULT``` into ```TEST_RESULTS_STORAGE``` on test completion. All tests should be executed vie this wrapper.
* ```TEST_CASE( testname, on_fail_func, test_func, test_args... )``` - same as above, but calls on_fail_func if test fails.

### Mock
* ```mock_ref< T >``` - mocks reference of type ```T```
* ```mock_const_ref< T >``` - mocks const reference of type ```T```
* ```mock_valid_ptr< T >``` - mocks non-null pointer to type ```T```
* ```mock_valid_const_ptr< T >``` - mocks non-null const pointer to type ```T```

## polymorph
A header-only type-erasure class similar to boost::any. Employs implicit sharing of data unless ```deep_copy``` is explicitly called.

* ```polymorph& deep_copy<T>( const polymorph& source )``` - perfoms deep copy of the source's underlying data. Throws ```std::invalid _argument``` if source is empty, or ```std::bad_cast``` if template paramerer is not correct.
* ```(const) T& get<T>()``` - Returns a reference to the underlying data. Throws ```std::bad_cast``` if template paramerer is not correct.
* ```(const) T& get_unsafe<T>()``` - Returns a reference to the underlying data. No runtime checks are performed, ```static_cast``` is used under the hood.
* ```bool check_type<T>()``` - Returns ```true``` if the underlying type is of type ```T```, otherwise returns ```false```.
* ```const type_info& type_info()``` - Returns ```std::type_info``` of the underlying type, or ```typeid(void)``` if empty.
* ```bool empty()``` - Returns ```true``` if no data is stored, otherwise returns ```false```.
