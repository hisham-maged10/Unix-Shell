/**
 * A CPP program that stimulates a Shell by doing the system calls to the kernel to do operations
 */ 
#include <iostream>
#include <unistd.h> //has the fork(),execvp() functions 
#include <string.h>
#include <vector> 
#include <sys/wait.h> // has the wait() function
#include <fstream>
#define LIMIT 1024 // max size of command

using namespace std;

void child_process(char**); // main function for execution of commands as child processes
void terminate_shell(); // exits the shell
bool isInvalid(char* input); // checks if the input was empty
vector<char*> get_arguments(char*,bool*); // gets the tokens of the command


ofstream file; // file stream to write log when process terminates
bool isBackground = false; // used to determine if it's a background process or foreground

// on termination, the child process sends a SIGChLD signal
// and this function handles that signal by checking if the process was a background one
// if so then parent waits till it sends its exit code then logs, if not it logs directly
void log(int signal)
{
    if(isBackground)
        wait(NULL); // since parent didn't wait for the child to close, it didn't capture its exit code, so this makes sure it waits to capture its
                    // exit code and not turn into zombie state on termination
    file << "Process has been terminated" << endl;
}

int main()
{
    file.open("log.txt"); //creates log file and overwrites it if it existed
    system("clear"); // clears the screen on execution to give the terminal feel
    char command[LIMIT];
    const char* TERMINATE = "exit"; // const string to compare against for exit command
    pid_t pid;
    // infinite loop that doesn't break unless the user inputs the exit command
    // each iteration outputs Hisham's Shell > and then prompts the user to input command
    // checks if the command is empty, if so it skips to next iterations
    // executes the command if it's valid, if not it prints Invalid Command and exits the child process
    while(true)
    {
        cout << "$hisham-maged10 Shell > ";
        cin.getline(command,LIMIT);
        // if(isInvalid(command)) // if pressed enter without input
        //     continue;
        if(strcmp(command,TERMINATE) == 0) // if input exit
            terminate_shell();
        vector<char*> args = get_arguments(command,&isBackground); // gets arguments and changes value of isBackground by reference
        char** argv = args.data(); // gets internal array of vector args
        pid = fork(); // creates child process
        if(pid == 0)
        {
            child_process(argv); //executes child process
            return 0;
        }
        if(isBackground) // if background process, doesn't wait 
        {
            sleep(1); // waits a second  so loading lines of background process is finished then go to next iteration without waiting
            continue;
        }
        else // parent process so it waits till child process terminates to continue
        {
            signal(SIGCHLD,log); //handles sigchld signal when process terminates
            wait(NULL);
        }
    }
    file.close(); // closes the file stream
    return 0;
}

/**
 * Splits the command string into tokens by using strtok from string.h header using the space delimter
 * then returns the vector<char*> to get the internal array using .data() to be used in execvp
 * and sets the value of isBackground by dereference of sent addres
*/
vector<char*> get_arguments(char* command,bool* isBg)
{
    vector<char*> args;
    char* arg = strtok(command," ");

    while( arg != NULL )
    {
        args.push_back(arg);
        arg = strtok(NULL," ");
    }
    if(strcmp(args.at(args.size() -1),"&") == 0) //if background
    {
        *isBg = 1; //dereferences the background bool and sets its value
        args.at(args.size() -1) = NULL; // removes & from arguments
    }
    args.push_back(NULL); //adds null to internal array so when used when execvp it works correctly
    return args;
}   

/**
 * executes the command given if it was valid, if not it prints invalid command and exits
*/
void child_process(char* argv[])
{  
     int status = execvp(argv[0],argv);
        if(status == -1)
        {
            cout << "Invalid Command" << endl;
            exit(1); //exits with failure exit code
        }
    exit(0); //exits child process
}
/**
 * Checks if the input is empty string or not
*/
bool isInvalid(char* input)
{
    return strcmp(input,"") == 0;
}

/**
 * Terminates the Shell with success exit code
*/
void terminate_shell()
{
    exit(0);
}

