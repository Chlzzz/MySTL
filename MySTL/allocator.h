#ifndef MYSTL_ALLOCATOR_H
#define MYSTL_ALLOCATOR_H_

// 这个头文件包含一个模板类 allocator，用于管理内存的分配、释放，对象的构造、析构

#include "construct.h"
#include "util.h"

namespace mystl {

// 模板类：allocator
// 模板函数代表数据类型
template <class T>
class allocator {
public:
    typedef T               value_type;
    typedef T*              pointer;
    typedef const T*        const_pointer;
    typedef T&              reference;
    typedef const T&        const_reference;
    typedef size_t          size_type;
    typedef ptridiff_t      difference_type;

public:
    static T*   allocate();    
    static T*   allocate(size_type n);

    static void deallocate(T* ptr);
    static void deallocate(T* ptr, size_type n);

    static void construct(T* ptr);
    static void construct(T* ptr, const T& value);
    static void construct(T* ptr, T&& value);

    template <class... Args>
    static void construct(T* ptr, Args&& ...args);

    static void destory(T* ptr);
    static void destory(T* first, T* last);
};

template <class T>
T* allocator<T>::allocate() {
    return static_cast<T*>(::operator new(sizeof(T)));
}

template <class T>
T* allocator<T>::allocate(size_type n) {
    if(n == 0)
        return nullptr;
    return static_cast<T*>(::operator new(n * sizeof(T)));
}

template <class T>
void allocator<T>::deallocate(T* ptr) {
    if(ptr == nullptr)
        return;
    ::operator delete(ptr);
}

template <class T>
void allocator<T>::deallocate(T* ptr, size_type /*size*/) {
    if(ptr ==  nullptr)
        return;
    ::operator delete(ptr);
}

template <class T>
void allocator<T>::construct(T* ptr) {
    mystl::construct(ptr);
}

template <class T>
void allocator<T>::construct(T* ptr, const T& value) {
    mystl::construct(ptr, value);
}

template <class T>
void allocator<T>::construct(T* ptr, T&& value) {
    mystl::construct(ptr, mystl::move(value));
}

template <class T>
template <class ...Args>
void allocator<T>::construct(T* ptr, Args&& ...args) {
    mystl::construct(ptr, mystl::forward<Args>(args)...);
}

template <class T>
void allocator<T>::destory(T* ptr) {
    mystl::destory(ptr);
}

template <class T>
void allocator<T>::destory(T* first, T* last) {
    mystl::destory(first, last);
}

} // namespace mystl

#endif // MYSTL_ALLOCATOR_H_