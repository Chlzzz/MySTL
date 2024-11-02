#ifndef MYSTL_ITERATOR_H_
#define MYSTL_ITERATOR_H_

#include <cstddef>

#include "type_traits.h"


namespace mystl {

// 五种迭代器类型
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};

// iterator 模板
template <class Category, class T, class Distance = ptrdiff_t,
    class Pointer = T*, class Reference = T&>
struct iterator {
    typedef Category        iterator_category;
    typedef T               value_type;
    typedef Distance        difference_type;
    typedef Pointer         pointer;
    typedef Reference       reference;
};

// iterator traits
template <class T>
struct has_iterator_cat {
private:
    struct two_char {char a; char b;};
    template <class U> static two_char test(...);
    template <class U> static char test(typename U::iterator_category* = 0);
public:
    static const bool value = sizeof(test<T>(0)) == sizeof(char);
};

template <class Iterator, bool>
struct iterator_traits_impl {};

template <class Iterator>
struct iterator_traits_impl<Iterator, true> {
    typedef typename Iterator::iterator_category    iterator_category;
    typedef typename Iterator::value_type           value_type;
    typedef typename Iterator::pointer              pointer;
    typedef typename Iterator::reference            reference;
    typedef typename Iterator::difference_type      difference_type;
};

template <class Iterator, bool>
struct iterator_traits_helper {};

template <class Iterator>
struct iterator_traits_helper<Iterator, true>
    : public iterator_traits_impl<Iterator,
        std::is_convertible_v<typename Iterator::iterator_category, input_iterator_tag> ||
        std::is_convertible_v<typename Iterator::iterator_category, output_iterator_tag> > {};

// 获得迭代器的特性
template <class Iterator>
struct iterator_traits 
    : public iterator_traits_helper<Iterator, has_iterator_cat<Iterator>::value > {};

// 偏特化版本
template <class T>
struct iterator_traits <T*> {
    typedef random_access_iterator_tag  iterator_category;
    typedef T                           value_type;
    typedef T*                          pointer;
    typedef T&                          reference;
    typedef ptrdiff_t                   difference_type;
};

template <class T>
struct iterator_traits <const T*> {
    typedef random_access_iterator_tag  iterator_category;
    typedef T                           value_type;
    typedef T*                          pointer;
    typedef T&                          reference;
    typedef ptrdiff_t                   difference_type;
};

template <class T, class U,  bool = has_iterator_cat<iterator_traits<T> >::value>
struct has_iterator_cat_of
    : public m_bool_constant<std::is_convertible_v<
      typename iterator_traits<T>::iterator_category, U> > {};

// 提取某种迭代器
template <class T, class U>
struct has_iterator_cat_of<T, U, false> : m_false_type {};

template <class Iter>
struct is_input_iterator : public has_iterator_cat_of<Iter, input_iterator_tag> {};

template <class Iter>
struct is_output_iterator : public has_iterator_cat_of<Iter, output_iterator_tag> {};

template <class Iter>
struct is_forward_iterator : public has_iterator_cat_of<Iter, forward_iterator_tag> {};

template <class Iter>
struct is_bidireactional_iterator : public has_iterator_cat_of<Iter, bidirectional_iterator_tag> {};

template <class Iter>
struct is_random_access_iterator : public has_iterator_cat_of<Iter, random_access_iterator_tag> {};

template <class Iterator>
struct is_iterator :
  public m_bool_constant<is_input_iterator<Iterator>::value ||
    is_output_iterator<Iterator>::value> {};

// 提取迭代器的元素
// category
template <class Iterator>
typename iterator_traits<Iterator>::iterator_category 
    iterator_category(const Iterator&) {
        typedef typename iterator_traits<Iterator>::iterator_category Category;
        return Category();
}

//distance_type
template <class Iterator>
typename iterator_traits<Iterator>::difference_type* 
    distance_type(const Iterator&) {
        return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
}

//value_type
template <class Iterator>
typename iterator_traits<Iterator>::value_type* 
    vlaue_type(const Iterator&) {
        return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
}

//计算迭代器之间的距离
// input
template <class InputIterator>
typename iterator_traits<InputIterator>::difference_type
    distance_dispatch(InputIterator first, InputIterator last, input_iterator_tag) {
        typename iterator_traits<InputIterator>::difference_type n = 0;
        while(first != last) {
            first++;
            n++;
        }
        return n;
}

// random
template <class RandomIter>
typename iterator_traits<RandomIter>::difference_type
    distance_dispatch(RandomIter first, RandomIter last, random_access_iterator_tag) {
        return last - first;
}

template <class InputIterator>
typename iterator_traits<InputIterator>::difference_type
    distance_dispatch(InputIterator first, InputIterator last) {
        return distance_dispatch(first, last, iterator_category(first));
}

// 前进n步
// input_iterator_tag 
template <class InputIterator, class Distance>
void advance_dispatch(InputIterator& iter, Distance n, input_iterator_tag) {
    while(n--)
        ++iter;
}

// bidirectional_iterator_tag
template <class BiDirectionalIter, class Distance>
void advance_dispatch(BiDirectionalIter& iter, Distance n, bidirectional_iterator_tag) {
    if(n >= 0)
        while(n--) ++iter;
    else
        while(n++) --iter;
}

// random_access_iterator_tag
template <class RandomIterator, class Distance>
void advance_dispatch(RandomIterator& iter, Distance n,random_access_iterator_tag) {
    iter += n;
}

template <class InputIterator, class Distance>
void advance_dispatch(InputIterator& iter, Distance n) {
    distance_dispatch(iter, n, iterator_category(iter));
}

// <<<<< 反向迭代器 <<<<< //
template <class Iterator>
class reverse_iterator {
private:
    Iterator current;       // 记录对应的正向迭代器

public:
    typedef typename iterator_traits<Iterator>::iterator_category   iterator_category;
    typedef typename iterator_traits<Iterator>::value_type          value_type;
    typedef typename iterator_traits<Iterator>::difference_type     difference_type;
    typedef typename iterator_traits<Iterator>::pointer             pointer;
    typedef typename iterator_traits<Iterator>::reference           reference;

    typedef Iterator                                                iterator_type;
    typedef reverse_iterator<Iterator>                              self;

public:
    //constructor
    reverse_iterator() {};
    explicit reverse_iterator(iterator_type iter) : current(iter) {}
    reverse_iterator(const self& rhs) : current(rhs.current) {}

public:
    //取对应正向迭代器
    iterator_type base() const {
        return current;
    }

    //重载操作符
    reference operator*() const {
        auto tmp = current;
        return *--tmp;
    } //返回一个值or引用， * 解引用

    pointer operator->() {
        return &(operator*());
    } // 返回一个引用， & 获取返回值的地址
    
    //重载运算符
    // ++ => --
    self& operator++() {
        --current;
        return *this;
    }// 前缀递减
    self operator++(int) {
        self tmp = *this;
        --current;
        return tmp;
    }// 后缀递减

    // -- => ++
    self& operator--() {
        ++current;
        return *this;
    }
    self operator--(int) {
        self tmp = *this;
        ++current;
        return tmp;
    }

    self& operator+=(difference_type n) {
        current -= n;
        return *this;
    }
    
    self operator+(difference_type n)const {
        return self(current - n);
    }

    self& operator-=(difference_type n) {
        current += n;
        return *this;
    }
    
    self operator-(difference_type n)const {
        return self(current + n);
    }

    reference operator[](difference_type n) const {
        return *(*this + n);
    }

};

//operator -, 计算迭代器之间的距离
template <class Iterator>
typename reverse_iterator<Iterator>::difference_type
    operator-(const reverse_iterator<Iterator>& lhs,
              const reverse_iterator<Iterator>& rhs) {
        return rhs.base() - lhs.base();
}

//重载比较运算符
template <class Iterator>
bool operator==(const reverse_iterator<Iterator>& lhs,
                const reverse_iterator<Iterator>& rhs) {
        return rhs.base() == lhs.base();
}

template <class Iterator>
bool operator<(const reverse_iterator<Iterator>& lhs,
               const reverse_iterator<Iterator>& rhs) {
        return rhs.base() < lhs.base();
}

template <class Iterator>
bool operator!=(const reverse_iterator<Iterator>& lhs,
                const reverse_iterator<Iterator>& rhs) {
        return !(rhs == lhs);
}

template <class Iterator>
bool operator>(const reverse_iterator<Iterator>& lhs,
               const reverse_iterator<Iterator>& rhs) {
        return rhs < lhs;
}

template <class Iterator>
bool operator<=(const reverse_iterator<Iterator>& lhs,
                const reverse_iterator<Iterator>& rhs) {
        return !(rhs < lhs);
}

template <class Iterator>
bool operator>=(const reverse_iterator<Iterator>& lhs,
                const reverse_iterator<Iterator>& rhs) {
        return !(lhs < rhs);
}

} // namespace mystl

#endif // MYSTL_ITERATOR_H_
