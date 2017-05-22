#ifndef _HELPERS_NONCOPYABLE_H_
#define _HELPERS_NONCOPYABLE_H_

namespace helpers
{

namespace classes
{

class non_copyable
{
public:
    non_copyable() = default;
    non_copyable( const non_copyable& ) = delete;
    non_copyable& operator=( const non_copyable& ) = default;
    non_copyable( non_copyable&& ) = default;
    non_copyable& operator=( non_copyable&& ) = default;
};

class non_movable
{
public:
    non_movable() = default;
    non_movable( const non_movable& ) = default;
    non_movable& operator=( const non_movable& ) = default;
    non_movable( non_movable&& ) = delete;
    non_movable& operator=( non_movable&& ) = delete;
};

class non_copyable_non_movable : public non_copyable, public non_movable
{
public:
    non_copyable_non_movable() = default;
};

}// classes

}// helpers

#endif
