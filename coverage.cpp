#include <cstdint>
#include <cstdio>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cstdlib>
#include <cstring>

unsigned char* coverage_map = nullptr;

//coverage map is inside the child process, so it dissapears when child exits
extern "C" { 
     void __sanitizer_cov_trace_pc_guard(uint32_t *guard) {
        if (!coverage_map) return;

         uint32_t index = *guard;

          if (index == 0) return; //check if already seen
         
          coverage_map[index % 65536] = 1;

          *guard =0; //mark as seen
          }
          
}

//attach in child
void attachCoverageMap(){
    char* shm_str = getenv("SHM_ID");
    if (!shm_str) return;

    int shm_id = atoi(shm_str);

    //attach coverage map to the shared memory segemt 
    coverage_map = (unsigned char*) shmat(shm_id, nullptr, 0);

}

//runs autimatically in child
__attribute__((constructor))
void setup() {
    attachCoverageMap();
}