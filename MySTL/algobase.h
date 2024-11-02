#ifndef MYSTL_ALGOBASE_H_
#define MYSTL_ALGOBASE_H_

#include <cstring>

#include "iterator.h"
#include "util.h"

namespace mystl {

#ifndef max
#pragma message("#undefing macro max")
#undef max
#endif // max

#ifndef min
#pragma message("#undefing macro min")
#undef min
#endif // min

/****************************************************************/
// max
//取二者中的较大值，语义相等时保证返回第一个参数
/****************************************************************/
template <class T>
const T& max(const T& lhs, const T& rhs) {
    return lhs < rhs ? rhs : lhs;
}
//重载版本使用自定义函数对象 comp 代替比较操作
template <class T, class Compare>
const T& max(const T& lhs, const T& rhs, Compare comp) {
    return comp(lhs, rhs) ? lhs : rhs;
}

/****************************************************************/
// min
//取二者中的较小值，语义相等时保证返回第一个参数
/****************************************************************/
template <class T>
const T& min(const T& lhs, const T& rhs) {
    return rhs < lhs ? rhs : lhs;
}
//重载版本使用自定义函数对象 comp 代替比较操作
template <class T, class Compare>
const T& min(const T& lhs, const T& rhs, Compare comp) {
    return comp(rhs, lhs) ? rhs : lhs;
}

/****************************************************************/
// iter_swap， 交换两个迭代器所指对象
/****************************************************************/
template <class Iter1, class Iter2>
void iter_swap(Iter1 lhs, Iter2 rhs) {
    mystl::swap(*lhs, *rhs);
}

/******************************************************************/
// copy
// 把 [first, last)区间内的元素拷贝到 [result, result + (last - first))内
/*****************************************************************/
// input_iterator_tag 版本
template <class InputIter, class OutputIter>
OutputIter unchecked_copy_cat(InputIter first, InputIter last, OutputIter result,
        mystl::input_iterator_tag) {
    for(; first != last; ++first, ++result) {
        *result = *first;
    }
    return result;
}

// random_access_iterator_tag 版本
template <class RandomIter, class OutputIter>
OutputIter unchecked_copy_cat(RandomIter first, RandomIter last, OutputIter result,
        mystl::random_access_iterator_tag) {
    for(auto n = last - first; n > 0; --n, ++first, ++result) {
        *result = *first;
    }
    return result;
}

template <class InputIter, class OutputIter>
OutputIter unchecked_copy(InputIter first, InputIter last, OutputIter result) {
    return unchecked_copy_cat(first, last, result, iterator_category(first));
}

// 为 trivially_copy_assignable 类型提供特化版本
template <class Tp, class Up>
typename std::enable_if_t< 
    std::is_same<typename std::remove_const_t<Tp>, Up>::value && 
    std::is_trivially_copy_assignable<Up>::value, Up*>
    unchecked_copy(Tp* first, Tp* last, Up* result) {
        const auto n = static_cast<size_t>(last - first);
        if(n != 0)
            std::memmove(result, first, n*sizeof(Up));
        return result + n;
}

template <class InputIter, class OutputIter>
OutputIter copy(InputIter first, InputIter last, OutputIter result) {
    return unchecked_copy(first, last, result);
}

/*****************************************************************************************/
// copy_backward
// 将 [first, last)区间内的元素拷贝到 [result - (last - first), result)内
/*****************************************************************************************/
// unchecked_copy_backward_cat 的 bidirectional_iterator_tag 版本
template <class BiIter1, class BiIter2>
BiIter2 unchecked_copy_backward_cat(BiIter1 first, BiIter1 last, BiIter2 result,
        mystl::bidirectional_iterator_tag) {
    while(first != last)
        *--result = *--last;
    return result;
}

// unchecked_copy_backward_cat 的 random_access_iterator_tag 版本
template <class RandomIter1, class BiIter2>
BiIter2 unchecked_copy_backward_cat(RandomIter1 first, RandomIter1 last,
            BiIter2 result, mystl::random_access_iterator_tag) {
    for(auto n = last - first; n > 0; --n)
        *--result = *--last;
    return result;
}

template <class BiIter1, class BiIter2>
BiIter2 unchecked_copy_backward(BiIter1 first, BiIter1 last, BiIter2 result) {
    return unchecked_copy_backward_cat(first, last, result, iterator_category(first));
}


// 为 trivially_copy_assignable 类型提供特化版本
template <class Tp, class Up>
typename std::enable_if_t< 
    std::is_same<typename std::remove_const_t<Tp>, Up>::value && 
    std::is_trivially_copy_assignable<Up>::value, Up*>
    unchecked_copy_backward(Tp* first, Tp* last, Up* result) {
        const szie_t n = static_cast<size_t>(last - first);
        if(n != 0)
            result -= n;
            std::memmove(result, first, n*sizeof(Up));
        return result;
}

template <class InputIter, class OutputIter>
OutputIter copy_backward(InputIter first, InputIter last, OutputIter result) {
    return unchecked_copy_backward(first, last, result);
}

/*****************************************************************************************/
// move
// 把 [first, last)区间内的元素移动到 [result, result + (last - first))内
/*****************************************************************************************/
// input_iterator_tag 版本
// 把[first, last)内满足一元操作 unary_pred 的元素拷贝到以 result 为起始的位置上
/*****************************************************************************************/
template <class InIter, class OutIter, class UnaryPredicate>
OutIter copy_if(InIter first, InIter last, OutIter result, UnaryPredicate unary_pred) {
    for(; first != last; ++first) {
        if(unary_pred(*first)) 
            *result++ = *first;
    }
    return result;
}

/*****************************************************************************************/
// copy_n
// 把 [first, first + n)区间上的元素拷贝到 [result, result + n)上
// 返回一个 pair 分别指向拷贝结束的尾部
/*****************************************************************************************/
template <class InputIter, class Size, class OutputIter>
mystl::pair<InputIter, OutputIter> unchecked_copy_n
    (InputIter first, Size n, OutputIter result, mystl::input_iterator_tag) {
        for(; n > 0; --n, ++first, ++result)
            *result = *first;
        return mystl::pair<InputIter, OutputIter>(first, result);
}

template <class RandomIter, class Size, class OutputIter>
mystl::pair<RandomIter, OutputIter> unchecked_copy_n
    (RandomIter first, Size n, OutputIter result, mystl::random_access_iterator_tag) {
        auto last = first + n;
        return mystl::pair<InputIter, OutputIter>(last, mystl::copy(first, last, result));
}

template <class RandomIter, class Size, class OutputIter>
mystl::pair<RandomIter, OutputIter> unchecked_copy_n
    (RandomIter first, Size n, OutputIter result) {
        return unchecked_copy_n(first, n, result, iterator_category(first));
}

/*****************************************************************************************/
// move
// 把 [first, last)区间内的元素移动到 [result, result + (last - first))内
/*****************************************************************************************/
// input_iterator_tag 版本
template <class InputIter, class OutputIter>
OutputIter unchecked_move_cat(InputIter first, InputIter last, OutputIter result,
    mystl::input_iterator_tag) {
        for(; first != last; ++first, ++result)
            *result = mystl::move(*first);    // move: 转换为右值引用， 便于移动赋值
        return result;
}

// random_access_ietrator_tag
template <class RandomIter, class OutputIter>
OutputIter unchecked_move_cat(RandomIter first, RandomIter last, OutputIter result,
    mystl::random_access_iterator_tag) {
        for(auto n = last - first; n > 0; --n, ++first, ++result)
            *result = mystl::move(*first);    // move: 转换为右值引用， 便于移动赋值
        return result;
}

template <class InputIter, class OutputIter>
OutputIter unchecked_move(InputIter first, InputIter last, OutputIter result) {
       return unchecked_move_cat(first, last, result, iterator_category(first));
}

// 为 trivially_copy_assignable 类型提供特化版本
template <class Up, class Tp>
typename std::enable_if_t<
    std::is_same<typename std::remove_const_t<Tp>, Up>::value &&
    std::is_trivially_move_assignable<Up>::value, Up*>
    unchecked_move(Tp* first, Tp* last, Up* result) {
        const size_t n = static_cast<size_t>(last - first);
        if(n != 0)
            std::memmove(result, first, n * sizeof(Up));
        return result + n;
}

template <class InputIter, class OutputIter>
OutputIter move(InputIter first, InputIter last, OutputIter result) {
       return unchecked_move(first, last, result);
}

/*****************************************************************************************/
// move_backward
// 将 [first, last)区间内的元素移动到 [result - (last - first), result)内
/*****************************************************************************************/
// bidirectional_iterator_tag 版本
template <class BiIter1, class BiIter2>
BiIter2 unchecked_move_backward_cat(BiIter1 first, BiIter1 last, BiIter2 result,
        mystl::bidirectional_iterator_tag) {
    while(first != last)
        *--result = mystl::move(*--last);
    return result;
}

// random_access_iterator_tag 版本
template <class RandomIter1, class BiIter2>
BiIter2 unchecked_move_backward_cat(RandomIter1 first, RandomIter1 last,
            BiIter2 result, mystl::random_access_iterator_tag) {
    for(auto n = last - first; n > 0; --n)
        *--result = mystl::move(*--last);
    return result;
}

template <class BiIter1, class BiIter2>
BiIter2 unchecked_move_backward(BiIter1 first, BiIter1 last, BiIter2 result) {
    return unchecked_move_backward_cat(first, last, result, iterator_category(first));
}

// 为 trivially_move_assignable 类型提供特化版本
template <class Tp, class Up>
typename std::enable_if_t< 
    std::is_same<typename std::remove_const_t<Tp>, Up>::value && 
    std::is_trivially_copy_assignable<Up>::value, Up*>
    unchecked_move_backward(Tp* first, Tp* last, Up* result) {
        const size_t n = static_cast<size_t>(last - first);
        if(n != 0)
            result -= n;
            std::memmove(result, first, n*sizeof(Up));
        return result;
}

template <class InputIter, class OutputIter>
OutputIter move_backward(InputIter first, InputIter last, OutputIter result) {
    return unchecked_move_backward(first, last, result);
}

/*****************************************************************************************/
// equal
// 比较第一序列在 [first, last)区间上的元素值是否和第二序列相等
/*****************************************************************************************/
template <class InputIter1, class InputIter2>
bool equal(InputIter1 first1, InputIter1 last1, InputIter2 first2) {
    for(; first1 != last1; ++first1, ++first2) {
        if(*first1 != *first2)
            return false;
    }
    return true;
}

// 使用自定义compare
template <class InputIter1, class InputIter2, class Compare>
bool equal(InputIter1 first1, InputIter1 last1, InputIter2 first2, Compare comp) {
    for(; first1 != last1; ++first1, ++first2) {
        if(!comp(*first1, *first2))
            return false;
    }
    return true;
}

/*****************************************************************************************/
// fill_n
// 从 first 位置开始填充 n 个值
/*****************************************************************************************/
template <class OutputIter, class Size, class T>
OutputIter unchecked_fill_n(OutputIter first, Size n, const T& value) {
    for(; n > 0; --n, ++first)
        *first = value;
    return first;
}

// 为 one-byte 类型提供特化版本
template <class Tp, class Size, class Up>
typename std::enable_if_t<
    std::is_integral<Tp>::value && sizeof(Tp) == 1 &&
    !std::is_same<Tp, bool>::value &&
    std::is_integral<Up>::value && sizeof(Up) == 1, Tp*>
    unchecked_fill_n(Tp* first, Size n, Up value) {
        if(n > 0)
            std::memset(first, (unsigned char) value, (size_t) n);
        return first + n;
}

template <class OutputIter, class Size, class T>
OutputIter fill_n(OutputIter first, Size n, const T& value) {
    return unchecked_fill_n(first, n, value);
}

/*****************************************************************************************/
// fill
// 为 [first, last)区间内的所有元素填充新值
/*****************************************************************************************/
} // end namespace

#endif // MYSTL_ALGOBASE_H_
