#include "init_shm.hpp"
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
#include <csignal>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::fprintf(stderr, "Usage: shm_attach shmid [shmid2 ...]\n");
        return 1;
    }
    std::vector<void*> addrs;
    for (int i=1; i<argc; ++i) {
        int shmid = std::strtol(argv[i], nullptr, 10);
        void* addr = shmat(shmid, nullptr, 0);
        if (addr == (void*)-1) { std::perror("shmat"); return 1; }
        addrs.push_back(addr);
        std::cout << "Attached shmid=" << shmid << " at " << addr << "\n";
    }
    std::cout << "PID=" << getpid() << ". Sleeping... (Ctrl+C to exit. For task 2: Ctrl+Z to suspend and check /proc/<pid>/maps)\n";
    for (;;) pause();
    return 0;
}
