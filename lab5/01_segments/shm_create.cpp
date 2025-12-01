#include "init_shm.hpp"
#include <cstdio>
#include <cstdlib>
#include <string>


int main(int argc, char** argv) {
    if (argc < 2) {
        std::fprintf(stderr, "Usage: shm_create size_bytes [keyfile [proj]]\n");
        return 1;
    }
    size_t size = std::strtoull(argv[1], nullptr, 10);
    key_t key;
    int shmflg = IPC_CREAT | 0666;

    if (argc >= 3) {
        std::string keyfile = argv[2];
        int proj = (argc >= 4) ? argv[3][0] : 'M';
        try {
            key = shmutil::key_from_ftok(keyfile, proj);
        } catch (const std::exception& e) {
            std::fprintf(stderr, "Error: %s\n", e.what());
            return 1;
        }
    } else {
        key = IPC_PRIVATE;
    }

    int shmid = shmget(key, size, shmflg);
    if (shmid == -1) { std::perror("shmget"); return 1; }
    std::cout << "shmid=" << shmid << " requested=" << size << " bytes\n";
    return 0;
}
