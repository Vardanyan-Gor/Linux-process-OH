#pragma once
#include <cstddef>
#include <cstdint>

#define SHM_DATA_MAX 4096

struct shm_block {
    size_t nbytes;   
    int eof;        
    char data[SHM_DATA_MAX];
};
