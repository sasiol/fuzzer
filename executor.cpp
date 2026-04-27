#include <cstdlib>
#include <iostream>

#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cstring>

static const int MAP_SIZE =65536; //move to comon.h?

unsigned char* shm_map = nullptr; //common.h better place etc
int shm_id = -1;

void shareMemory(){
    //id for allocated shared memory segment
    shm_id =shmget(IPC_PRIVATE, MAP_SIZE, IPC_CREAT | 0600 );

    if (shm_id <0) {
        std::cerr << "shmget failed\n";
        exit(1);
    }
    //attach the shared memory segment to this process
    shm_map = (unsigned char*) shmat(shm_id, nullptr, 0);

    if (shm_map == (void*)-1) {
        std::cerr << "shmat failed\n";
        exit(1);
    }

    memset(shm_map, 0, MAP_SIZE);

}

bool runTarget(const std::string& inputFile) {
    memset(shm_map, 0, MAP_SIZE);
    //give shared memory id to child
    std::string shm_str = std::to_string(shm_id);
    setenv("SHM_ID", shm_str.c_str(), 1);
    setenv("SHM_ID", std::to_string(shm_id).c_str(), 1);
    pid_t pid = fork(); //create child process(copy of the program)

    //if within child process
    if (pid ==0) {
        //exacl replaces child process with target program (switch to execv?)
             //arguments: (path, program name, input, end of argument)
        execl("./target/tidy", "./target/tidy", inputFile.c_str(), NULL);

        //if exec fails
        exit(1);

    } else {
        //parent waits for child
        int status;
        waitpid(pid, &status, 0); //

        std::cout << "Coverage snapshot:\n";

        for (int i = 0; i < MAP_SIZE; i++) {
            if (shm_map[i]) {
                std::cout << i << " ";
            }
        }

        std::cout << "\n";

        //WIFSIGNALED return true if the child process was terminated by a signal
        if (WIFSIGNALED(status)) {
            int signal = WTERMSIG(status);  //returns number of the signal

            std::cout << "CRASH! Signal: " << signal << "\n";

            if (signal == SIGABRT) {
                std::cout << "SIGBART detected(abort) \n";
            }

            if (signal == SIGSEGV) {
                std::cout << "→ Detected SIGSEGV (segfault)\n";
            }
            return true;
        } //later add more crash signals

        //norml exit
        return false;
    }

}