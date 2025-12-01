#include "init_shm.hpp"
#include "init_sem.hpp"
#include "bin_sems.hpp"
#include "shm_block.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>

int main(int argc, char** argv){
    if (argc < 3){
        std::fprintf(stderr,"Usage: shm_writer <keyfile_shm> <keyfile_sem>\n");
        return 1;
    }
    const char* shm_keyfile = argv[1];
    const char* sem_keyfile = argv[2];

    key_t shm_key = shmutil::key_from_ftok(shm_keyfile, 'W');
    int shmid = shmget(shm_key, sizeof(shm_block), IPC_CREAT | 0666);
    if (shmid == -1){ std::perror("shmget"); return 1; }
    void* addr = shmat(shmid, nullptr, 0);
    if (addr == (void*)-1){ std::perror("shmat"); return 1; }
    shm_block* blk = reinterpret_cast<shm_block*>(addr);

    int semid = binsems::ensure_semset(sem_keyfile, /*init_as_writer=*/true);

    while (true){
        binsems::reserveSem(semid, binsems::SEM_WRITER);

        if (!std::cin.good()) { 
            blk->nbytes = 0; blk->eof = 1;
            binsems::releaseSem(semid, binsems::SEM_READER);
            break;
        }
        std::cin.read(blk->data, SHM_DATA_MAX);
        std::streamsize got = std::cin.gcount();
        if (got <= 0){
            blk->nbytes = 0; blk->eof = 1;
            binsems::releaseSem(semid, binsems::SEM_READER);
            break;
        }
        blk->nbytes = static_cast<size_t>(got);
        blk->eof = 0;

        binsems::releaseSem(semid, binsems::SEM_READER);
        if (std::cin.eof()) {
            ;
        }
    }


    shmdt(addr);
    return 0;
}
