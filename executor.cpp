
#include "helpers.h"
#include "executor.h"
#include <cstdlib>
#include <iostream>

#include <atomic>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cstring>
#include <sys/resource.h>
#include <cerrno>
#include <cstring>

static pid_t currentChildPid = -1;
static volatile sig_atomic_t timedOut = 0;
//alarm for if target program gets stuck (kills the child)
static void handleAlarm(int) {
    timedOut = true;
    if (currentChildPid > 0) {
        kill(-currentChildPid, SIGKILL);
    }
}

bool runTarget(const std::string& inputFile) {
    //give shared memory id to child
    std::string shm_str = std::to_string(shm_id);
    setenv("SHM_ID", shm_str.c_str(), 1);
    //setenv("SHM_ID", std::to_string(shm_id).c_str(), 1);

    std::string targetPath = findTargetBinary();

    if (targetPath.empty()) {
        std::cerr << "No executable target found!\n";
        return false;
        }
    pid_t pid = fork(); //create child process(copy of the program)
    //incase fork fails and child is not created
    if (pid < 0) {
        std::cerr << "fork() failed\n";
        return false;
    }
    //if within child process
    if (pid ==0) {
        //set up and run the child process
        child(targetPath, inputFile);
       _exit(1); // child() should never return

    } // for parent process
    else {
        return monitorChild(pid);
    }

}




void executeChild(
    const std::string& targetPath,
    const std::string& inputFile
) {
    // Put the child into its own process group
    if (setpgid(0, 0) == -1) {
        _exit(1);
    }

    setResourceLimits();

    if (lmode == LogMode::NORMAL) {
        freopen("/dev/null", "w", stdout);
        freopen("/dev/null", "w", stderr);
    }

    // Replace child with target program
    execl(targetPath.c_str(),
          targetPath.c_str(),
          inputFile.c_str(),
          nullptr);

    // exec failed
    _exit(1);
}


bool monitorChild( pid_t pid ){
    //ensure that the child has joined the right process group
        if (setpgid(pid, pid) == -1) {
            std::cerr << "setpgid() failed\n";
        }
        timedOut = false;
        currentChildPid = pid;
        signal(SIGALRM, handleAlarm);
        alarm(FUZZ_TIMEOUT_SECS); //schedules alarm to fire after 2 secs (prevent hangs)

        //parent waits for child
        int status;
        waitpid(pid, &status, 0); //

        alarm(0);  // cancel alarm if target exited normally
        currentChildPid = -1;
        if (timedOut) {
            if (lmode == LogMode::DEBUG)
                std::cout << "TIMEOUT — target hung, input skipped\n";
            return false; // don't count hangs as crashes
        }

        //debug to see coverage snapshot
        if (lmode == LogMode::DEBUG) {
            std::cout << "Coverage snapshot:\n";
            for (int i = 0; i < MAP_SIZE; i++) {
                if (shm_map[i]) {
                    std::cout << i << " ";
                }
            }
            std::cout << "\n";
        }

        //WIFSIGNALED return true if the child process was terminated by a signal
        if (WIFSIGNALED(status)) {
            int signal = WTERMSIG(status);  //returns number of the signal

            if (lmode == LogMode::DEBUG) std::cout << "CRASH! Signal: " << signal << "\n";

            if (signal == SIGABRT) {
               if (lmode == LogMode::DEBUG)  std::cout << "SIGBART detected(abort) \n";
            }

            if (signal == SIGSEGV) {
              if (lmode == LogMode::DEBUG)   std::cout << "→ Detected SIGSEGV (segfault)\n";
            }
            return true;
        } //later add more crash signals

        //norml exit
        return false;
}



void setResourceLimits() {
    // Limit CPU time
    struct rlimit cpuLimit;
    cpuLimit.rlim_cur = 1; //soft limit
    cpuLimit.rlim_max = 1; // hard limit

    if (setrlimit(RLIMIT_CPU, &cpuLimit) == -1) {
    std::cerr << "setrlimit(RLIMIT_CPU) failed: "
              << std::strerror(errno) << "\n";
    _exit(1);
    }

    // Limit virtual address space to 256 MiB
    struct rlimit memoryLimit;
    memoryLimit.rlim_cur = 256 * 1024 * 1024;
    memoryLimit.rlim_max = 256 * 1024 * 1024;

    if (setrlimit(RLIMIT_AS, &memoryLimit) == -1) {
        std::cerr << "setrlimit(RLIMIT_AS) failed: "
              << std::strerror(errno) << "\n";
        _exit(1);
    }
}