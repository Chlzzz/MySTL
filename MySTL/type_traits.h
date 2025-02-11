#ifndef MYSTL_TYPE_TRAITS_H_
#define MYSTL_TYPE_TRAITS_H_

//用于提取类型信息

// use standard header for type_traits
#include <type_traits>

namespace mystl {

// helper struct
template <class T, T v>
struct m_integral_constant {
    static constexpr T value = v;
};

template <bool b>
using m_bool_constant = m_integral_constant<bool, b>;

typedef m_bool_constant<true> m_true_type;
typedef m_bool_constant<false> m_false_type;

// type traits
template <class T1, class T2>
struct pair;  // forward declaration

template <class T>
struct is_pair : mystl::m_false_type{};

template <class T1, class T2>
struct is_pair<mystl::pair<T1, T2> > : mystl::m_true_type{};

// *** general code ends *** //
} // namespace ystl
#endif // MYSTL_TYPE_TRAITS_H_
