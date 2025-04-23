// Created by Ayoree 20.04.2025

#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <functional>

#define CONCAT_INNER(a, b) a##b
#define CONCAT(a, b) CONCAT_INNER(a, b)

// This macro can only be used when declaring variables
// Uses the “free” function to automatically free memory for the C-Like pointer
#define DEFER __attribute__((__cleanup__(__cleanup_func__)))

// Calls the function after leaving current scope
#define DEFER_SCOPE(func) __Defer__ CONCAT(defer_, CONCAT(__LINE__, __COUNTER__))(func)

template<typename F>
class __Defer__ {
public:
    explicit __Defer__(F&& func) noexcept : func_(std::forward<F>(func)) {}
    ~__Defer__() noexcept {
        func_();
    }
    __Defer__(const __Defer__&) = delete;
    __Defer__& operator=(const __Defer__&) = delete;

private:
    F func_;
};

void __cleanup_func__(void* ptr)
{
    printf("free %p\n", *(void**)ptr);
    free(*(void**)ptr);
}
