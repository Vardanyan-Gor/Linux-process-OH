#pragma once
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <string>
#include <stdexcept>
#include <iostream>


union semun {
    int              val;
    struct semid_ds *buf;
    unsigned short  *array;
    struct seminfo  *__buf;
};

namespace semutil {

inline key_t key_from_ftok(const std::string& path, int proj='S') {
    key_t k = ftok(path.c_str(), proj);
    if (k == -1) throw std::runtime_error(std::string("ftok failed: ") + strerror(errno));
    return k;
}

inline int create_or_get(int nsems, key_t key, int perms = 0666, bool create=true) {
    int flags = create ? (IPC_CREAT | perms) : 0;
    int semid = semget(key, nsems, flags);
    if (semid == -1) throw std::runtime_error(std::string("semget failed: ") + strerror(errno));
    return semid;
}

inline void setval(int semid, unsigned short idx, int v) {
    union semun arg{}; arg.val = v;
    if (semctl(semid, idx, SETVAL, arg) == -1) throw std::runtime_error(std::string("semctl(SETVAL) failed: ") + strerror(errno));
}

inline void reserve(int semid, unsigned short idx, int flags=0) {
    struct sembuf op{}; op.sem_num=idx; op.sem_op=-1; op.sem_flg=flags;
    if (semop(semid, &op, 1) == -1) throw std::runtime_error(std::string("semop -1 failed: ") + strerror(errno));
}

inline void release(int semid, unsigned short idx, int flags=0) {
    struct sembuf op{}; op.sem_num=idx; op.sem_op=+1; op.sem_flg=flags;
    if (semop(semid, &op, 1) == -1) throw std::runtime_error(std::string("semop +1 failed: ") + strerror(errno));
}

inline void remove_set(int semid) {
    if (semctl(semid, 0, IPC_RMID) == -1) throw std::runtime_error(std::string("semctl(IPC_RMID) failed: ") + strerror(errno));
}

} 
