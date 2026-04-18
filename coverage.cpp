#include <cstdint>
#include <cstdio>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <unistd.h>


unsigned char* coverage_map = nullptr;


//coverage map is inside the child process, so it dissapears when child exits
extern "C" void __sanitizer_cov_trace_pc_guard(uint32_t *guard) {
    static int hit = 0;
    hit++;
    if (hit < 5) {
        write(2, "HOOK\n", 5);
    }
}






//attach in child
void attachCoverageMap(){
    char* shm_str = getenv("SHM_ID");
    if (!shm_str) return;

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

