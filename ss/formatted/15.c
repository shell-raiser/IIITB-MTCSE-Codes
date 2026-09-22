// 15. Displaying User Environmental Variables: Create a program to display the
// environmental variables of the user, utilizing the environ variable.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Declaration of environ - array of environment variable strings
extern char **environ;

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
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
    
    for (size_t i = 0; i < sizeof(vars)/sizeof(vars[0]); i++) {
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

/*
 * Sample output for 15.c
 * Example runtime:
 * $ gcc 15.c -o 15
 * $ ./15
 *
 * Program executed successfully.
 */

/*
 * Actual output of 15.c
 * Command: gcc 15.c -o 15
 *
 * Environment Variables:
 * ======================
 *
 * Method 1: Using environ global variable
 * SHELL=/bin/bash
 * COLORTERM=truecolor
 * HISTCONTROL=ignorespace
 * XDG_MENU_PREFIX=gnome-
 * TERM_PROGRAM_VERSION=1.138.0
 * COPILOT_DEBUG_NONCE=75d4adb8cd9a80a0ef6f217f086b9230
 * QT_IM_MODULES=wayland;ibus
 * GEMINI_CLI_IDE_SERVER_PORT=34543
 * AI_AGENT=github_copilot_vscode_agent
 * VSSCRIPT_PATH=/usr/lib/python3.14/site-packages/vapoursynth/libvsscript.so
 * LC_ADDRESS=en_IN.UTF-8
 * SSH_AUTH_SOCK=/run/user/1000/gcr/ssh
 * MEMORY_PRESSURE_WRITE=c29tZSAyMDAwMDAgMjAwMDAwMAA=
 * PYDEVD_DISABLE_FILE_VALIDATION=1
 * XMODIFIERS=@im=ibus
 * DESKTOP_SESSION=gnome
 * LC_MONETARY=en_IN.UTF-8
 * NO_AT_BRIDGE=1
 * PWD=/mnt/dataDrive/IIITB/ss/formatted
 * XDG_SESSION_DESKTOP=gnome
 * LOGNAME=kss
 * QT_QPA_PLATFORMTHEME=gtk2
 * XDG_SESSION_TYPE=wayland
 * SYSTEMD_EXEC_PID=14622
 * BUNDLED_DEBUGPY_PATH=/home/kss/.vscode/extensions/ms-python.debugpy-2026.6.0-linux-x64/bundled/libs/debugpy
 * XAUTHORITY=/run/user/1000/.mutter-Xwaylandauth.UYM8V3
 * VSCODE_GIT_ASKPASS_NODE=/usr/share/code/code
 * GJS_DEBUG_TOPICS=JS ERROR;JS LOG
 * MOTD_SHOWN=pam
 * GDM_LANG=en_US.UTF-8
 * HOME=/home/kss
 * USERNAME=kss
 * LC_PAPER=en_IN.UTF-8
 * LANG=en_US.UTF-8
 * LS_COLORS=rs=0:di=01;34:ln=01;36:mh=00:pi=40;33:so=01;35:do=01;35:bd=40;33;01:cd=40;33;01:or=01;05;37;41:mi=01;05;37;41:su=37;41:sg=30;43:ca=30;41:tw=30;42:ow=34;42:st=37;44:ex=01;32:*.tar=01;31:*.tgz=01;31:*.arc=01;31:*.arj=01;31:*.taz=01;31:*.lha=01;31:*.lz4=01;31:*.lzh=01;31:*.lzma=01;31:*.tlz=01;31:*.txz=01;31:*.tzo=01;31:*.t7z=01;31:*.zip=01;31:*.z=01;31:*.Z=01;31:*.dz=01;31:*.gz=01;31:*.lrz=01;31:*.lz=01;31:*.lzo=01;31:*.xz=01;31:*.bz2=01;31:*.bz=01;31:*.tbz=01;31:*.tbz2=01;31:*.tz=01;31:*.deb=01;31:*.rpm=01;31:*.jar=01;31:*.war=01;31:*.ear=01;31:*.sar=01;31:*.rar=01;31:*.alz=01;31:*.ace=01;31:*.zoo=01;31:*.cpio=01;31:*.7z=01;31:*.rz=01;31:*.cab=01;31:*.jpg=01;35:*.jpeg=01;35:*.gif=01;35:*.bmp=01;35:*.pbm=01;35:*.pgm=01;35:*.ppm=01;35:*.tga=01;35:*.xbm=01;35:*.xpm=01;35:*.tif=01;35:*.tiff=01;35:*.png=01;35:*.svg=01;35:*.svgz=01;35:*.mng=01;35:*.pcx=01;35:*.mov=01;35:*.mpg=01;35:*.mpeg=01;35:*.m2v=01;35:*.mkv=01;35:*.webm=01;35:*.ogm=01;35:*.mp4=01;35:*.m4v=01;35:*.mp4v=01;35:*.vob=01;35:*.qt=01;35:*.nuv=01;35:*.wmv=01;35:*.asf=01;35:*.rm=01;35:*.rmvb=01;35:*.flc=01;35:*.avi=01;35:*.fli=01;35:*.flv=01;35:*.gl=01;35:*.dl=01;35:*.xcf=01;35:*.xwd=01;35:*.yuv=01;35:*.cgm=01;35:*.emf=01;35:*.axv=01;35:*.anx=01;35:*.ogv=01;35:*.ogx=01;35:*.pdf=00;32:*.ps=00;32:*.txt=00;32:*.patch=00;32:*.diff=00;32:*.log=00;32:*.tex=00;32:*.doc=00;32:*.aac=00;36:*.au=00;36:*.flac=00;36:*.m4a=00;36:*.mid=00;36:*.midi=00;36:*.mka=00;36:*.mp3=00;36:*.mpc=00;36:*.ogg=00;36:*.ra=00;36:*.wav=00;36:*.axa=00;36:*.oga=00;36:*.spx=00;36:*.xspf=00;36:
 * XDG_CURRENT_DESKTOP=GNOME
 * PYTHONSTARTUP=/home/kss/.config/Code/User/workspaceStorage/c3b3bec65705ff35b867cb1b3cd6bfa0/ms-python.python/pythonrc.py
 * MEMORY_PRESSURE_WATCH=/sys/fs/cgroup/user.slice/user-1000.slice/user@1000.service/session.slice/org.gnome.Shell@user.service/memory.pressure
 * CLOUDSDK_ROOT_DIR=/opt/google-cloud-cli
 * WAYLAND_DISPLAY=wayland-0
 * GIT_ASKPASS=/usr/share/code/resources/app/extensions/git/dist/askpass.sh
 * INVOCATION_ID=4fe13b09395640e59a8f3236daa1f101
 * MANAGERPID=14481
 * CHROME_DESKTOP=code.desktop
 * GJS_DEBUG_OUTPUT=stderr
 * VSCODE_GIT_ASKPASS_EXTRA_ARGS=
 * GNOME_SETUP_DISPLAY=unix:/tmp/.X11-unix/X1
 * VSCODE_PYTHON_AUTOACTIVATE_GUARD=1
 * XDG_SESSION_CLASS=user
 * GEMINI_CLI_IDE_WORKSPACE_PATH=/mnt/dataDrive/IIITB
 * TERM=xterm-256color
 * PYTHON_BASIC_REPL=1
 * GOOGLE_CLOUD_SDK_HOME=/opt/google-cloud-cli
 * USER=kss
 * GIT_PAGER=cat
 * VSCODE_GIT_IPC_HANDLE=/run/user/1000/vscode-git-049284b29e.sock
 * DISPLAY=:0
 * SHLVL=4
 * GIT_MERGE_AUTOEDIT=no
 * GIT_EDITOR=:
 * LC_TELEPHONE=en_IN.UTF-8
 * QT_IM_MODULE=ibus
 * LC_MEASUREMENT=en_IN.UTF-8
 * MANAGERPIDFDID=14206
 * ROCM_PATH=/opt/rocm
 * FC_FONTATIONS=1
 * XDG_RUNTIME_DIR=/run/user/1000
 * MKLROOT=/opt/intel/oneapi/mkl/latest
 * DEBUGINFOD_URLS=https://debuginfod.archlinux.org 
 * LC_TIME=en_IN.UTF-8
 * COPILOT_AGENT=1
 * VSCODE_GIT_ASKPASS_MAIN=/usr/share/code/resources/app/extensions/git/dist/askpass-main.js
 * JOURNAL_STREAM=10:156043
 * XDG_DATA_DIRS=/home/kss/.local/share/flatpak/exports/share:/var/lib/flatpak/exports/share:/usr/local/share/:/usr/share/
 * GDK_BACKEND=wayland
 * GEMINI_CLI_IDE_AUTH_TOKEN=c91f16d6-8773-40ae-a12b-fe64c5a69b02
 * PATH=/home/kss/.local/bin:/home/kss/bin:/usr/local/sbin:/usr/local/bin:/usr/bin:/etc/poetry/bin/:/mnt/dataDrive/fedora-scripts-and-configs/scripts/:/home/kss/.local/bin/:/home/kss/.local/share/gem/ruby/3.4.0/bin/
 * GDMSESSION=gnome
 * XDG_SESSION_EXTRA_DEVICE_ACCESS=render:accel
 * DBUS_SESSION_BUS_ADDRESS=unix:path=/run/user/1000/bus
 * MAIL=/var/spool/mail/kss
 * DEBIAN_FRONTEND=noninteractive
 * GIO_LAUNCHED_DESKTOP_FILE_PID=43868
 * GIO_LAUNCHED_DESKTOP_FILE=/usr/share/applications/code.desktop
 * LC_NUMERIC=en_IN.UTF-8
 * TERM_PROGRAM=vscode
 * OLDPWD=/mnt/dataDrive/IIITB/ss
 * _=/usr/bin/python3
 *
 * ---
 *
 * Method 2: Specific variables via getenv()
 * HOME         = /home/kss
 * USER         = kss
 * SHELL        = /bin/bash
 * PATH         = /home/kss/.local/bin:/home/kss/bin:/usr/local/sbin:/usr/local/bin:/usr/bin:/etc/poetry/bin/:/mnt/dataDrive/fedora-scripts-and-configs/scripts/:/home/kss/.local/bin/:/home/kss/.local/share/gem/ruby/3.4.0/bin/
 * LANG         = en_US.UTF-8
 * PWD          = /mnt/dataDrive/IIITB/ss/formatted
 * TERM         = xterm-256color
 * LOGNAME      = kss
 * HOSTNAME     = (not set)
 * DISPLAY      = :0
 *
 * ---
 *
 * Method 3: Count of environment variables: 88
 *
 * ---
 *
 * Setting a custom environment variable...
 * MY_CUSTOM_VAR = Hello from OS Lab!
 * ANOTHER_VAR = Value from putenv
 */

