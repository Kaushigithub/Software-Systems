#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>

int main() {
    time_t time_now;
    struct tm exec_time;
    
    printf("Enter the execution time (HH:MM): ");
    scanf("%d:%d", &exec_time.tm_hour, &exec_time.tm_min);
    exec_time.tm_sec = 0;
   
    while (getchar() != '\n');

    // Validate user input
    if (exec_time.tm_hour < 0 || exec_time.tm_hour > 23 || exec_time.tm_min < 0 || exec_time.tm_min > 59) {
        printf("Invalid format of time. Please enter a valid time in format (HH:MM).\n");
        return 1;
    }

    char buff[256]; // Adjust the buffer size as needed
    printf("Enter the script path or name: ");
    fgets(buff, sizeof(buff), stdin);
    buff[strcspn(buff, "\n")] = '\0'; // Remove trailing newline character
    
    // Fork into a daemon process
    int pid = fork();
    if (pid < 0) {
        perror("Fork failed");
        return 1;
    }
    
    if (pid > 0) {
        // Parent process exits
        exit(0);
    }
    
    // Child process becomes the daemon
    setsid();
    
    while (1) {
        time(&time_now);
        struct tm* current_time = localtime(&time_now);
        
        if (current_time->tm_hour == exec_time.tm_hour && current_time->tm_min == exec_time.tm_min) {
            pid_t script_pid = fork();
            
            if (script_pid == 0) {
                execlp(buff, buff, NULL);
                perror("Script execution failed");
                exit(1);
            } else if (script_pid > 0) {
                int status;
                waitpid(script_pid, &status, 0);
                
                if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                    printf("\nScript executed successfully.\n");
                } else {
                    printf("\nScript execution failed.\n");
                }
                
                break; // Exit the loop
            }
        } else {
            // Calculate and display the countdown timer
            int remaining_seconds = (exec_time.tm_hour - current_time->tm_hour) * 3600 +
                                    (exec_time.tm_min - current_time->tm_min) * 60 -
                                    current_time->tm_sec;
            printf("\rScript will run in %d seconds.", remaining_seconds);
            fflush(stdout); // Flush the output to ensure it's displayed immediately
        }
        
        sleep(1); // Sleep for 1 second before checking the time again
    }
    
    return 0;
}

