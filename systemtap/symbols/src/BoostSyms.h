
#ifndef _BOOST_SYMS_H_H
#define _BOOST_SYMS_H_H
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

struct _boost_ptr_vector
{
    uint64_t _M_start;
    uint64_t _M_finish;
    uint64_t _M_end_of_storage;

    ~_boost_ptr_vector();
};
#endif
