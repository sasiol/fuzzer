#include <cstdint>
#include <cstdio>

//coverage map is inside the child process, so it dissapears when child exits
// trasfer it via file or shared memory?
extern "C" { 
  
    unsigned char coverage_map[1 << 16];
    
     void __sanitizer_cov_trace_pc_guard(uint32_t *guard) {
         uint32_t index = *guard;

          if (index == 0) return; //check if already seen
         
          coverage_map[index % 65536] = 1;

          *guard =0; //mark as seen
          }
          
}