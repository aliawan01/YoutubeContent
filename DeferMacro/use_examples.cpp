// Created by Ayoree 20.04.2025

/*
DEFER macro works only with g++ and gcc compilers (not MSVC)
DEFER_SCOPE macro should work with g++, gcc and MSVC compilers
*/

#include <stdio.h>
#include <stdlib.h>
#include "defer_macro_v2.h"

#define BUF_SIZE 4

// Entrypoint
int main()
{
    // C-like heap matrix
    char** c_buf DEFER = (char**)malloc(BUF_SIZE * sizeof(char*));
    for (size_t i = 0; i < BUF_SIZE; ++i)
    {
        c_buf[i] = (char*)malloc(BUF_SIZE * sizeof(char));
    }
    DEFER_SCOPE([&c_buf]() {
        for (size_t i = 0; i < BUF_SIZE; ++i)
        {
            free(c_buf[i]);
            printf("free(%zu) %p\n", i, (void*)c_buf[i]);
        }
        // No need to free(c_buf)
    });


    // CXX-like heap matrix
    char** cxx_buf = new char*[BUF_SIZE];
    for (size_t i = 0; i < BUF_SIZE; ++i)
    {
        cxx_buf[i] = new char[BUF_SIZE];
    }
    DEFER_SCOPE([&cxx_buf]() {
        for (size_t i = 0; i < BUF_SIZE; ++i)
        {
            printf("delete[%zu] %p\n", i, (void*)cxx_buf[i]);
            delete[] cxx_buf[i];
        }
        printf("delete %p\n", (void*)cxx_buf);
        delete[] cxx_buf;
    });


    // Do Some Stuff:
    printf("\n");
    printf("c_buf ptr: %p\n", (void*)c_buf);
    printf("cxx_buf ptr: %p\n", (void*)cxx_buf);
    for (size_t i = 0; i < BUF_SIZE; ++i)
    {
        printf("c_buf[%zu] ptr: %p\n", i, (void*)c_buf[i]);
    }
    for (size_t i = 0; i < BUF_SIZE; ++i)
    {
        printf("cxx_buf[%zu] ptr: %p\n", i, (void*)cxx_buf[i]);
    }

    return 0;
}