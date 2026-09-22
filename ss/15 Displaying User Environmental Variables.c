// 15. Displaying User Environmental Variables: Create a program to display the
// environmental variables of the user, utilizing the environ variable.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Declaration of environ - array of environment variable strings
extern char **environ;

int main(int argc, char *argv[]) {
    printf("Environment Variables:\n");
    printf("======================\n\n");

    // Method 1: Using environ variable directly
    printf("Method 1: Using environ global variable\n");
    for (char **env = environ; *env != NULL; env++) {
        printf("%s\n", *env);
    }

    printf("\n---\n\n");

    // Method 2: Using getenv for specific variables
    printf("Method 2: Specific variables via getenv()\n");
    const char *vars[] = {
        "HOME", "USER", "SHELL", "PATH", "LANG", 
        "PWD", "TERM", "LOGNAME", "HOSTNAME", "DISPLAY"
    };
    
    for (int i = 0; i < sizeof(vars)/sizeof(vars[0]); i++) {
        char *value = getenv(vars[i]);
        if (value) {
            printf("%-12s = %s\n", vars[i], value);
        } else {
            printf("%-12s = (not set)\n", vars[i]);
        }
    }

    printf("\n---\n\n");

    // Method 3: Using main's envp parameter (if available)
    // Note: envp is not standard but commonly available as third parameter to main
    if (argc > 0) {
        // Check if we have envp by looking at memory after argv
        // This is not portable but works on many systems
        printf("Method 3: Count of environment variables: ");
        int count = 0;
        for (char **env = environ; *env != NULL; env++) count++;
        printf("%d\n", count);
    }

    // Demonstrate setting/getting environment variables
    printf("\n---\n\n");
    printf("Setting a custom environment variable...\n");
    if (setenv("MY_CUSTOM_VAR", "Hello from OS Lab!", 1) == 0) {
        printf("MY_CUSTOM_VAR = %s\n", getenv("MY_CUSTOM_VAR"));
    }

    // Using putenv (alternative)
    putenv("ANOTHER_VAR=Value from putenv");
    printf("ANOTHER_VAR = %s\n", getenv("ANOTHER_VAR"));

    return 0;
}

/* Commands to run:
   gcc "15 Displaying User Environmental Variables.c" -o 15_env_vars
   ./15_env_vars
   
   # Compare with shell commands:
   env
   printenv
   echo $HOME
*/