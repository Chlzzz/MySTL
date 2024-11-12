#ifndef MYSTL_HEAP_ALGO_H_
#define MYSTL_HEAP_ALGO_H_

// 这个头文件包含 heap 的四个算法 : push_heap, pop_heap, sort_heap, make_heap

#include "iterator.h"

namespace mystl {

/*****************************************************************************************/
// push_heap
// 该函数接受两个迭代器，表示一个 heap 容器的首尾，并且新元素已经插入到底部容器的最尾端，调整 heap
/*****************************************************************************************/
template <class RandomIter, class Distance, class T>
void push_heap_aux(RandomIter first, Distance holeIndex, Distance topIndex, T value) {
    auto parent = (holeIndex / 2) - 1;
    while(holeIndex > topIndex && *(first + parent) < value) {
        *(first + holeIndex) = *(first + parent);
        holeIndex = parent;
        parent = (holeIndex / 2) - 1;
    }
    *(first + holeIndex) = value;
}

template <class RandomIter, class Distance>
void push_heap_d(RandomIter first, RandomIter last, Distance*) {
    // <Distance>(0), （0）在这里无意义
    mystl::push_heap_aux(first, (last - first) - 1, static_cast<Distance>(0), *(last - 1));
}

template <class RandomIter>
void push_heap(RandomIter first, RandomIter last) {
    // 新元素应该已置于底部容器的最尾端， 即仅做堆调整
    // distance_type 返回类型名
    mystl::push_heap_d(first, last, distance_type(first));
    
}

// 重载版本使用函数对象 comp 代替比较操作
template <class RandomIter, class Distance, class T, class Compared>
void push_heap_aux(RandomIter first, Distance holeIndex, Distance topIndex, 
                    T value, Compared comp) {
    auto parent = (holeIndex / 2) - 1;
    while(holeIndex > topIndex && comp(*(first + parent), value)) {
        *(first + holeIndex) = *(first + parent);
        holeIndex = parent;
        parent = (holeIndex / 2) - 1;
    }
    *(first + holeIndex) = value;
}

template <class RandomIter, class Distance, class Compared>
void push_heap_d(RandomIter first, RandomIter last, Distance*, Compared comp) {
    mystl::push_heap_aux(first, (last - first) - 1, static_cast<Distance>(0), *(last - 1), comp);
}

template <class RandomIter, class Compared>
void push_heap(RandomIter first, RandomIter last, Compared comp) {
    mystl::push_heap_d(first, last, distance_type(first), comp);
    
}


/*****************************************************************************************/
// pop_heap
// 该函数接受两个迭代器，表示 heap 容器的首尾，将 heap 的根节点取出放到容器尾部，调整 heap
/*****************************************************************************************/
template <class RandomIter, class T, class Distance>
void adjust_heap(RandomIter first, Distance holeIndex, Distance len, T value) {
    // 先进行下溯(percolate down)过程
    auto topIndex = holeIndex;
    auto child = 2 * holeIndex + 2;         // 右节点
    while(child < len) {
        if(*(first + child) < *(first + child - 1))
            --child;                        // 找到较大的子节点 
        *(first + holeIndex) = *(first + child);
        holeIndex = child;
        child = 2 * child + 2;
    }
    if(child == len) {
        // 没有右节点
        *(first + holeIndex) = *(first + child - 1);
        holeIndex = child - 1;
    }
    // 此时，有一个空位
    // 再执行一次上溯(percolate up)过程
    mystl::push_heap_aux(first, holeIndex, topIndex, value);
}

template <class RandomIter, class T, class Distance>
void pop_heap_aux(RandomIter first, RandomIter last, RandomIter result,
    T value, Distance*) {
    *result = *first;
    mystl::adjust_heap(first, static_cast<Distance>(0), last - first, value);
}

template <class RandomIter>
void pop_heap(RandomIter first, RandomIter last) {
    mystl::pop_heap_aux(first, last - 1, last - 1, *(last - 1), distance_type(first));
}

// 重载版本使用函数对象 comp 代替比较操作
template <class RandomIter, class T, class Distance, class Compared>
void adjust_heap(RandomIter first, Distance holeIndex, Distance len, 
    T value, Compared comp) {
    auto topIndex = holeIndex;
    auto child = 2 * holeIndex + 2;         // 右节点
    while(child < len) {
        if(comp(*(first + child), *(first + child - 1)))
            --child;                        // 找到较大的子节点 
        *(first + holeIndex) = *(first + child);
        holeIndex = child;
        child = 2 * child + 2;
    }
    if(child == len) {
        // 没有右节点
        *(first + holeIndex) = *(first + child - 1);
        holeIndex = child - 1;
    }
    mystl::push_heap_aux(first, holeIndex, topIndex, value, comp);
}

template <class RandomIter, class T, class Distance, class Compared>
void pop_heap_aux(RandomIter first, RandomIter last, RandomIter result,
    T value, Distance*, Compared comp) {
    *result = *first;
    mystl::adjust_heap(first, static_cast<Distance>(0), last - first, value, comp);
}

template <class RandomIter, class Compared>
void pop_heap(RandomIter first, RandomIter last, Compared comp) {
    mystl::pop_heap_aux(first, last - 1, last - 1, *(last - 1), distance_type(first), comp);
}


/*****************************************************************************************/
// sort_heap
// 该函数接受两个迭代器，表示 heap 容器的首尾，不断执行 pop_heap 操作，直到首尾最多相差1
/*****************************************************************************************/
template <class RandomIter>
void sort_heap(RandomIter first, RandomIter last) {
    // 每执行一次 pop_heap，最大的元素都被放到尾部，直到容器最多只有一个元素，完成排序
    while(last - first > 1)
        mystl::pop_heap(first, last--);
}

// 重载版本， 使用自定义compare
template <class RandomIter, class Compared>
void sort_heap(RandomIter first, RandomIter last, Compared comp) {
    while(last - first > 1)
        mystl::pop_heap(first, last--, comp);
}


/*****************************************************************************************/
// make_heap
// 该函数接受两个迭代器，表示 heap 容器的首尾，把容器内的数据变为一个 heap
/*****************************************************************************************/
template <class RandomIter, class Distance>
void make_heap_aux(RandomIter first, RandomIter last, Distance*) {
    if(last - first < 2)
        return;             // 为空，或只有一个元素
    auto len = last - first;
    auto holeIndex = (len - 2) / 2;
    // 从倒数第二层开始，重排以 holeIndex 为根的子树 
    while(true) {
        mystl::adjust_heap(first, holeIndex, len, *(first + holeIndex));
        if(holeIndex == 0)
            return;
        --holeIndex;
    }
}

// 重载版本使用函数对象 comp 代替比较操作
template <class RandomIter, class Distance, class Compared>
void make_heap_aux(RandomIter first, RandomIter last, Distance*, Compared comp) {
    if(last - first < 2)
        return;             // 为空，或只有一个元素
    auto len = last - first;
    auto holeIndex = (len - 2) / 2;
    // 从倒数第二层开始，重排以 holeIndex 为根的子树 
    while(true) {
        mystl::adjust_heap(first, holeIndex, len, *(first + holeIndex), comp);
        if(holeIndex == 0)
            return;
        --holeIndex;
    }
}

template <class RandomIter, class Compared>
void make_heap(RandomIter first, RandomIter last, Compared comp) {
    mystl::make_heap_aux(first, last, distance_type(first), comp);
}


} // namespace mystl

#endif // MYSTL_HEAP_ALGO_H_