#include <cstdlib>
#include <iostream>

#include <unistd.h>
#include <sys/wait.h>
#include <iostream>


bool runTarget(const std::string& inputFile) {
    pid_t pid = fork(); //create child process(copy of the program)

    //if within child process
    if (pid ==0) {
        //exacl replaces child process with target program (switch to execv?)
             //arguments: (path, program name, input, end of argument)
        execl("./tidy", "./tidy", inputFile.c_str(), NULL);

        //if exec fails
        exit(1);

    } else {
        //parent waits for child
        int status;
        waitpid(pid, &status, 0); //

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