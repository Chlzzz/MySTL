#ifndef MYSTL_UTIL_H_
#define MYSTL_UTIL_H_

#include <cstddef>

#include "type_traits.h"


namespace mystl {

//move
template <class T>
typename std::remove_reference_t<T>&& move(T&& arg) noexcept {
    return static_cast<typename std::remove_reference_t<T>&&>(arg);
}
// typename:告诉编译器返回值是一个类型

//forward
template <class T>
T&& forward(typename std::remove_reference_t<T>& arg) noexcept {
    return static_cast<T&&>(arg);
}

template <class T>
T&& forward(typename std::remove_reference_t<T>&& arg) noexcept {
    static_assert(!std::is_lvalue_reference_v<T>, "bad forward");
    return static_cast<T&&>(arg);
}

//swap
template <class Tp>
void swap(Tp& lhs, Tp& rhs) {
    auto tmp(mystl::move(lhs));
    lhs = mystl::move(rhs);
    rhs = mystl::move(tmp);
}

template <class ForwardIter1, class ForwardIter2>
ForwardIter2 swap_range(ForwardIter1& iter1, ForwardIter1& last1, ForwardIter2& iter2) {
    for(; iter1 != last1; ++iter1, (void) ++iter2) 
        mystl::swap(iter1, iter2);
    return iter2;
}

template <class Tp, size_t N>
void swap(Tp(&a)[N], Tp(&b)[N]) {
    mystl::swap_range(a, a + N, b);
}

// *** pair *** //
template <class Ty1, class Ty2>
struct pair {
    typedef Ty1 first_type;
    typedef Ty2 second_type;

    first_type first;
    second_type second;

    // c++ 17 standards, using enable_if_t, is_default_constructible_v
    // instead enable_if< , >::type, is_default_constructible< >::value
    // default constructible, 
    template <class Other1 = Ty1, class Other2 = Ty2,
        typename = typename std::enable_if_t<          
        std::is_default_constructible_v<Other1> &&
        std::is_default_constructible_v<Other2>, void> >
        constexpr pair(): first(), second() {}        
    
    //implicit constructor
    template <class U1 = Ty1, class U2 = Ty2,
        typename std::enable_if_t<          
        std::is_copy_constructible_v<Other1> &&
        std::is_copy_constructible_v<Other2> &&
        std::is_convertible_v<const U1&, Ty1> &&
        std::is_convertible_v<const U2&, Ty2>, int> = 0>
        constexpr pair(const U1& a, const U2& b): first(a), second(b) {}        
       
    //explicit constructor
    template <class U1 = Ty1, class U2 = Ty2,
        typename std::enable_if_t<          
        std::is_copy_constructible_v<Other1> &&
        std::is_copy_constructible_v<Other2> &&
        (!std::is_convertible_v<const U1&, Ty1> ||
        !std::is_convertible_v<const U2&, Ty2>), int> = 0>
        explicit constexpr pair(const U1& a, const U2& b): first(a), second(b) {}        
   
    //copy,move constructor
    pair(const pair& rhs) = default;
    pair(pair&& rhs) = default;

    //constructor for other types or pair
    //implicit, other types
    template <class Other1, class Other2, 
        typename std::enable_if_t<
        std::is_constructible_v<Ty1, Other1> &&
        std::is_constructible_v<Ty2, Other2> &&
        std::is_convertible_v<Other1&&, Ty1> &&
        std::is_convertible_v<Other2&&, Ty2>, int> = 0>
        constexpr pair(Other1&& a, Other2&& b):
        first(mystl::forward<Other1>(a)), second(mystl::forward<Other2>(b)) {}
        // is_constructible_v<Ty1, Other1>：是否可以使用Other1构造类型Ty1的对象
   
    //explicit, types
    template <class Other1, class Other2, 
        typename std::enable_if_t<
        std::is_constructible_v<Ty1, Other1> &&
        std::is_constructible_v<Ty2, Other2> &&
        (!std::is_convertible_v<Other1, Ty1> &&
        !std::is_convertible_v<Other2, Ty2>), int> = 0>
        explicit constexpr pair(Other1&& a, Other2&& b):
        first(mystl::forward<Other1>(a)), second(mystl::forward<Other2>(b)) {}

    //implicit, pair
    template <class Other1, class Other2, 
        typename std::enable_if_t<
        std::is_constructible_v<Ty1, const Other1&> &&
        std::is_constructible_v<Ty2, const Other2&> &&
        std::is_convertible_v<const Other1&, Ty1> &&
        std::is_convertible_v<const Other2&, Ty2>, int> = 0>
        constexpr pair(const pair<Other1, Other2>& other):
        :first(other.fisrt), second(other.second) {}
    
    template <class Other1, class Other2, 
        typename std::enable_if_t<
        std::is_constructible_v<Ty1, Other1> &&
        std::is_constructible_v<Ty2, Other2> &&
        std::is_convertible_v<Other1, Ty1> &&
        std::is_convertible_v<Other2, Ty2>, int> = 0>
        constexpr pair(pair<Other1, Other2>&& other):
        :first(mystl::forward<Other1>(other.first)),
        second(mystl::forward<Other2>(other.second)) {}
    
    //explicit, pair
    template <class Other1, class Other2, 
        typename std::enable_if_t<
        std::is_constructible_v<Ty1, const Other1&> &&
        std::is_constructible_v<Ty2, const Other2&> &&
        (!std::is_convertible_v<const Other1&, Ty1> &&
        !std::is_convertible_v<const Other2&, Ty2>), int> = 0>
        explicit constexpr pair(const pair<Other1, Other2>& other):
        :first(other.fisrt), second(other.second) {}
    
    template <class Other1, class Other2, 
        typename std::enable_if_t<
        std::is_constructible_v<Ty1, Other1> &&
        std::is_constructible_v<Ty2, Other2> &&
        (!std::is_convertible_v<Other1, Ty1> &&
        !std::is_convertible_v<Other2, Ty2>), int> = 0>
        explicit constexpr pair(const pair<Other1, Other2>&& other):
        :first(mystl::forward<Other1>(other.first)),
        second(mystl::forward<Other2>(other.second)) {}

    pair& operator=(const pair& rhs) {
        if(this != &rhs) {
            first = rhs.first;
            second = rhs.second;
        }
        return *this;
    }
    
    pair& operator=(pair&& rhs) {
        if(this != &rhs) {
            first = mystl::move(rhs.first);
            second = mystl::move(rhs.second);
        }
        return *this;
    }

    template <class Other1, class Other2>
    pair& operator=(const pair<Other1, Other2>& other) {
        first = other.first;
        second = other.second;
        return *this;
    }
    
    template <class Other1, class Other2>
    pair& operator=(const pair<Other1, Other2>&& other) {
        first = mystl::forward<Other1>(other.first);
        second = mystl::forward<Other2>(other.second);
        return *this;
    }

    ~pair() = default;

    void swap(pair& other) {
        if(this != &other) {
            mystl::swap(first, other.first);
            mystl::swap(second, other.second);
         }
    }

}; // struct pair ends

template <class Ty1, class Ty2>
bool operator==(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs) {
    return (lhs.first == rhs.first) && (lhs.second == rhs.second); 
}

template <class Ty1, class Ty2>
bool operator<(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs) {
    return (lhs.first < rhs.first) || (lhs.first == rhs.first && lhs.second < rhs.second); 
}

template <class Ty1, class Ty2>
bool operator!=(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs) {
    return !(lhs == rhs);
}

template <class Ty1, class Ty2>
bool operator>(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs) {
    return rhs < lhs;
}

template <class Ty1, class Ty2>
bool operator<=(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs) {
    return !(rhs < lhs);
}

template <class Ty1, class Ty2>
bool operator>=(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs) {
    return !(lhs < rhs);
}

template <class Ty1, class Ty2>
    void swap(pair<Ty1, Ty2>& lhs,  pair<Ty1, Ty2>& rhs) {
        lhs.swap(rhs);
}

template <class Ty1, class Ty2>
pair<Ty1, Ty2> make_pair(Ty1&& first, Ty2&& second) {
    return pair<Ty1, Ty2>(mystl::forward<Ty1>(first), mystl::forward<Ty2>(second));
}


} //namespace mystl
#endif //MYSTL_UTILS_H_

