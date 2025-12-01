#include "init_shm.hpp"
#include <cstdio>
#include <cstdlib>


int main(int argc, char** argv) {
    if (argc < 2) {
        std::fprintf(stderr, "Usage: shm_rm shmid [shmid2 ...]\n");
        return 1;
    }
    for (int i=1;i<argc;++i){
        int shmid = std::strtol(argv[i], nullptr, 10);
        if (shmctl(shmid, IPC_RMID, nullptr) == -1) { std::perror("shmctl(IPC_RMID)"); return 1; }
        std::cout << "Removed shmid=" << shmid << "\n";
    }
    return 0;
}
