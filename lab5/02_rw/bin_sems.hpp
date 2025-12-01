#pragma once
#include "init_sem.hpp"
#include <string>

namespace binsems {

static constexpr unsigned short SEM_WRITER = 0;
static constexpr unsigned short SEM_READER = 1;
static constexpr int N_SEMS = 2;

inline int ensure_semset(const std::string& keyfile, bool init_as_writer) {
    key_t key = semutil::key_from_ftok(keyfile, 'R');
    int semid = semutil::create_or_get(N_SEMS, key, 0666, true);

    if (init_as_writer) {
        semutil::setval(semid, SEM_WRITER, 1);
        semutil::setval(semid, SEM_READER, 0);
    } else {
        try {
            semutil::setval(semid, SEM_WRITER, 1);
            semutil::setval(semid, SEM_READER, 0);
        } catch (...) {}
    }
    return semid;
}

inline void initSemAvailable(int semid, unsigned short idx) {
    semutil::setval(semid, idx, 1);
}

inline void initSemInUse(int semid, unsigned short idx) {
    semutil::setval(semid, idx, 0);
}

inline void reserveSem(int semid, unsigned short idx) {
    semutil::reserve(semid, idx, 0);
}

inline void releaseSem(int semid, unsigned short idx) {
    semutil::release(semid, idx, 0);
}

} 