#ifndef MYSTL_CONSTRUCT_H_
#define MYSTL_CONSTRUCT_H_

#include <new>

#include "type_traits.h"
#include "iterator.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4100)  // unused parameter
#endif // _MSC_VER


namespace  mystl
{

// construct 构造函数

template <class Ty>
void construct(Ty* ptr) {
    ::new ((void*)ptr) Ty();
    // new (place_address) type或者new (place_address) type(initializer-list)
    // place_address必须是一个指针
}

template <class Ty1, class Ty2>
void construct(Ty1* ptr, const Ty2& value) {
    ::new((void*)ptr) Ty(value);
}

template <class Ty1, class... Args>
void construct(Ty1* ptr, Args&&... args) {
    ::new((void*)ptr) Ty(mystl::forward<Args>(args)...);
}

// destory 析构
template <class Ty>
void destory_one(Ty*, std::true_type) {}

template <class Ty>
void destory_one(Ty* pointer, std::false_type) {
    if(pointer != nullptr)
        pointer -> ~Ty();
}

template <class ForwardIter>
void destory_cat(ForwardIter first, ForwardIter last, std::true_type) {}

template <class ForwardIter>
void destory_cat(ForwardIter first, ForwardIter last, std::false_type) {
    for(; first != last; ++first)
        destory(&*first);
}

template <class Ty>
void destory_one(Ty* pointer) {
    destory_one(pointer, std::is_trivially_destructible<Ty>{});
}

template <class ForwardIter>
void destory(ForwardIter first, ForwardIter last) {
    destory_cat(first, last, std::is_trivially_destructible<
        typename iterator_traits<ForwardIter>::value_type>{});
}
    
} // namespace  mystl

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

#endif // MYSTL_CONSTRUCT_H_