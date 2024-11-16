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
                                  const FString &message = {});
     
    template <typename... T>
    static FString BuildFormattedString(const FString::ElementType *fmt,
                                        T &&...args)
    {
        TArray<FStringFormatArg> a = {FStringFormatArg(args)...};
        return FString::Format(fmt, a);
    }

    static void LogToScreenAndConsole(const FString &message);

private:
    static void X1AssertionFailed_WithCustomLog(
        const char *file, int line, const char *cond, const FString &message,
        const std::function<void(const FString &)> &logFunc);
};

#define _X1_ASSERT_NO_FMT(cond, ret, msg)                                      \
    {                                                                          \
        if (!(cond))                                                           \
        {                                                                      \
            _X1AssertionHelper::X1AssertionFailed(__FILE__, __LINE__, #cond );                        \
            ret;                                                               \
        }                                                                      \
    }

#define _X1_ASSERT_FMT(cond, ret, fmt, ...)                                    \
    {                                                                          \
        if (!(cond))                                                           \
        {                                                                      \
            _X1AssertionHelper::X1AssertionFailed(                             \
                __FILE__, __LINE__, #cond,                                     \
                _X1AssertionHelper::BuildFormattedString(TEXT(fmt),            \
                                                         __VA_ARGS__));        \
            ret;                                                               \
        }                                                                      \
    }

#define _X1_ASSERT_GET_OVLD(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, NAME, ...) \
    NAME
#define X1_ASSERT(cond, ret, ...)                                              \
    _X1_ASSERT_GET_OVLD(__VA_ARGS__, _X1_ASSERT_FMT, _X1_ASSERT_FMT,           \
                        _X1_ASSERT_FMT, _X1_ASSERT_FMT, _X1_ASSERT_FMT,        \
                        _X1_ASSERT_FMT, _X1_ASSERT_FMT, _X1_ASSERT_FMT,        \
                        _X1_ASSERT_FMT, _X1_ASSERT_NO_FMT)                     \
    (cond, ret, __VA_ARGS__)

// clang-format off

// 当条件不满足时，在屏幕和Log输出消息，并从当前函数返回 void（不会导致程序崩溃）。
#define X1_ASSERT_RET_VOID(cond,  ...) X1_ASSERT(cond, return, __VA_ARGS__)
// 当条件不满足时，在屏幕和Log输出消息，并从当前函数返回 {}（不会导致程序崩溃）。
#define X1_ASSERT_RET_EMPTY(cond,  ...) X1_ASSERT(cond, return {}, __VA_ARGS__)
// 当条件不满足时，在屏幕和Log输出消息，并 continue （不会导致程序崩溃）。
#define X1_ASSERT_CONTINUE(cond,  ...) X1_ASSERT(cond, continue, __VA_ARGS__)
// 当条件不满足时，在屏幕和Log输出消息，并继续执行（不会导致程序崩溃）。
#define X1_ASSERT_PASS(cond,  ...) X1_ASSERT(cond, ;, __VA_ARGS__)

// clang-format on
