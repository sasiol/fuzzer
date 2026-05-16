#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "helpers.h"



unsigned char* coverage_map = nullptr;

// initiliase sanitizer coverage guards in the target program used for noticing edges
extern "C" void __sanitizer_cov_trace_pc_guard_init(uint32_t *start, uint32_t *end) {
    static uint32_t N = 1;

    for (uint32_t *x = start; x < end; x++) {
        *x = N++;
    }
}

//coverage hook automatically inserted by Clang
//called whenever code edge is executed
extern "C" void __sanitizer_cov_trace_pc_guard(uint32_t *guard) {
    //write(2, "HOOK\n", 5);
    if (!coverage_map) {
       // write(2, "NOMAP\n", 5);
        return;
    }
    uint32_t index = *guard;
    char buf[64];
    int n = snprintf(buf, sizeof(buf), "IDX=%u\n", index);
    //write(2, buf, n);
    coverage_map[index % MAP_SIZE] = 1;
}


//attach shared memory in child
void attachCoverageMap(){
    //write(2, "ATTACH\n", 7);

    char* shm_str = getenv("SHM_ID");
    if (!shm_str) {
        write(2, "NO ENV\n", 7);
        return;
    }

    int shm_id = atoi(shm_str);

    //attach coverage map to the shared memory segemt 
    coverage_map = (unsigned char*) shmat(shm_id, nullptr, 0);

     if (coverage_map == (void*)-1) {
        coverage_map = nullptr;
    }
}

//runs autimatically in child
__attribute__((constructor))
void setup() {
    attachCoverageMap();
}

