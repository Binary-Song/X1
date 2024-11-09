#pragma once
#include "CoreMinimal.h"
#include <format>
DECLARE_LOG_CATEGORY_EXTERN(LogAssert, Log, All);

void X1AssertionFailed(const char *file, int line, const char *cond, FString message);

#define _X1_ASSERT_RET_IMPL(cond, ret, msg)                                                                            \
    {                                                                                                                  \
        if (!(cond))                                                                                                   \
        {                                                                                                              \
            X1AssertionFailed(__FILE__, __LINE__, #cond, msg);                                                         \
            ret;                                                                                                       \
        }                                                                                                              \
    }

// clang-format off
#define X1_ASSERTF_RET_VOID(cond, fmt, ...) _X1_ASSERT_RET_IMPL(cond, return, FString::Printf(TEXT(fmt), __VA_ARGS__))
#define X1_ASSERTF_RET_EMPTY(cond, fmt, ...) _X1_ASSERT_RET_IMPL(cond, return {}, FString::Printf(TEXT(fmt), __VA_ARGS__))
#define X1_ASSERTF_CONTINUE(cond, fmt, ...) _X1_ASSERT_RET_IMPL(cond, continue, FString::Printf(TEXT(fmt), __VA_ARGS__))
#define X1_ASSERT_RET_VOID(cond) _X1_ASSERT_RET_IMPL(cond, return, {})
#define X1_ASSERT_RET_EMPTY(cond) _X1_ASSERT_RET_IMPL(cond, return {}, {})
#define X1_ASSERT_CONTINUE(cond) _X1_ASSERT_RET_IMPL(cond, continue, {})
// clang-format on