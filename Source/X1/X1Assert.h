#pragma once
#include "CoreMinimal.h"
#include "Logging/StructuredLog.h"
#include <format>
#include <functional>
DECLARE_LOG_CATEGORY_EXTERN(LogAssert, Log, All);

// use assertion macros instead
class _X1AssertionHelper
{
public:
    static void X1AssertionFailed(const char *file, int line, const char *cond,
                                  const FString &message);
    static void X1AssertionFailed_WithCustomLog(
        const char *file, int line, const char *cond, const FString &message,
        const std::function<void(const FString &)> &logFunc);

    template <typename... T>
    static FString BuildFormattedString(const FString::ElementType *fmt,
                                        T &&...args)
    {
        TArray<FStringFormatArg> a = {FStringFormatArg(args)...};
        return FString::Format(fmt, a);
    }
};

#define _X1_ASSERT_RET_IMPL(cond, ret, msg)                                    \
    {                                                                          \
        if (!(cond))                                                           \
        {                                                                      \
            _X1AssertionHelper::X1AssertionFailed(__FILE__, __LINE__, #cond,   \
                                                  msg);                        \
            ret;                                                               \
        }                                                                      \
    }
#define _X1_ASSERT_RET_IMPL2(cond, ret, fmt, ...)                              \
    {                                                                          \
        if (!(cond))                                                           \
        {                                                                      \
            _X1AssertionHelper::X1AssertionFailed(                             \
                __FILE__, __LINE__, #cond,                                     \
                _X1AssertionHelper::BuildFormattedString(TEXT(fmt),            \
                                                         __VA_ARGS__));        \
        }                                                                      \
    }

// clang-format off

// 当条件不满足时，在屏幕和Log输出消息，并从当前函数返回（不会导致程序崩溃）。
// ASSERT: 不指定消息
// ASSERTF: 使用 FString::Printf 格式化消息，使用C风格格式串
// ASSERTX: 使用 FString::Format 指定消息，使用 `{}` 风格格式串
// RET_VOID: 条件不满足时返回 void
// RET_EMPTY: 条件不满足时返回 {}
// RET_CONTINUE: 条件不满足时继续下个循环 （continue）
#define X1_ASSERTF_RET_VOID(cond, fmt, ...) _X1_ASSERT_RET_IMPL(cond,  return     , FString::Printf(TEXT(fmt), __VA_ARGS__))
#define X1_ASSERTF_RET_EMPTY(cond, fmt, ...) _X1_ASSERT_RET_IMPL(cond, return {}  , FString::Printf(TEXT(fmt), __VA_ARGS__))
#define X1_ASSERTF_CONTINUE(cond, fmt, ...) _X1_ASSERT_RET_IMPL(cond,  continue   , FString::Printf(TEXT(fmt), __VA_ARGS__))
#define X1_ASSERTF_IGNORE(cond, fmt, ...) _X1_ASSERT_RET_IMPL(cond,    ;          , FString::Printf(TEXT(fmt), __VA_ARGS__))
#define X1_ASSERT_RET_VOID(cond) _X1_ASSERT_RET_IMPL(cond, return, {})
#define X1_ASSERT_RET_EMPTY(cond) _X1_ASSERT_RET_IMPL(cond, return {}, {})
#define X1_ASSERT_CONTINUE(cond) _X1_ASSERT_RET_IMPL(cond, continue, {})
#define X1_ASSERT_IGNORE(cond) _X1_ASSERT_RET_IMPL(cond, ;, {})
#define X1_ASSERTX_RET_VOID(cond, fmt, ...) _X1_ASSERT_RET_IMPL2(cond, return, fmt, __VA_ARGS__)
#define X1_ASSERTX_RET_EMPTY(cond, fmt, ...) _X1_ASSERT_RET_IMPL2(cond, return {}, fmt, __VA_ARGS__)
#define X1_ASSERTX_CONTINUE(cond, fmt, ...) _X1_ASSERT_RET_IMPL2(cond, continue, fmt, __VA_ARGS__)
#define X1_ASSERTX_IGNORE(cond, fmt, ...) _X1_ASSERT_RET_IMPL2(cond, ;, fmt, __VA_ARGS__)
// clang-format on
