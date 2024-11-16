#pragma once

#include "Containers/Array.h"
#include <concepts>

template <typename Fn, typename Elem>
concept G1CompareFn = requires(const Elem &a, const Elem &b) {
    { std::declval<Fn>()(a, b) } -> std::convertible_to<bool>;
};

template <typename Fn, typename Elem>
concept G1EvalFn = requires(const Elem &a) {
    { std::declval<Fn>()(a) } -> std::convertible_to<double>;
};

template <typename T> struct FX1FindResult
{
    bool bFound = false;
    int Index = 0;
    T Item = {};
};

class X1_API UX1CollectionUtil
{
    template <typename Elem, G1CompareFn<Elem> Compare>
    static FX1FindResult<Elem> FindMaxOrMinElement(const TArray<Elem> &array, const Compare &lessThan, bool min)
    {
        FX1FindResult<Elem> result{};
        if (array.IsEmpty())
            return result;

        int index = 0;
        result.Index = 0;
        result.Item = array[0];
        result.bFound = true;
        for (const auto &elem : array)
        {
            if (min ^ lessThan(elem, result.Item))
            {
                result.Index = index;
                result.Item = elem;
            }
            index++;
        }
        return result;
    }

public:
    template <typename Elem, G1CompareFn<Elem> Compare>
    static FX1FindResult<Elem> FindMinElement(const TArray<Elem> &array, const Compare &lessThan)
    {
        return FindMaxOrMinElement(array, lessThan, true);
    }

    template <typename Elem, G1CompareFn<Elem> Compare>
    static FX1FindResult<Elem> FindMaxElement(const TArray<Elem> &array, const Compare &lessThan)
    {
        return FindMaxOrMinElement(array, lessThan, false);
    }
};
