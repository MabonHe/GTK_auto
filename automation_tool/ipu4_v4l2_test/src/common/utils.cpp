/*
 * Copyright (c) 2014 Intel Corporation. All rights Reserved.
 */

#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <limits.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <stdarg.h>

#include "utils.h"
#include "gtest/gtest.h"

using namespace std;

static const char *sh_paths[] = { "/system/bin/sh", "/bin/sh" };

static const char *get_shell_path(void)
{
    size_t n;
    struct stat buf;
    for(n = 0; n < sizeof(sh_paths)/sizeof(char*); n++) {
        if (!stat(sh_paths[n], &buf))
            return sh_paths[n];
    }
    return NULL;
}

int exec_cmd(const char *cmd)
{
    int stat;
    pid_t pid;
    struct sigaction sa, savintr, savequit;
    sigset_t saveblock;
    if (cmd == NULL)
        return(1);
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigemptyset(&savintr.sa_mask);
    sigemptyset(&savequit.sa_mask);
    sigaction(SIGINT, &sa, &savintr);
    sigaction(SIGQUIT, &sa, &savequit);
    sigaddset(&sa.sa_mask, SIGCHLD);
    sigprocmask(SIG_BLOCK, &sa.sa_mask, &saveblock);
    if ((pid = fork()) == 0) {
        sigaction(SIGINT, &savintr, (struct sigaction *)0);
        sigaction(SIGQUIT, &savequit, (struct sigaction *)0);
        sigprocmask(SIG_SETMASK, &saveblock, (sigset_t *)0);
        execl(get_shell_path(), "sh", "-c", cmd, (char *)0);
        _exit(127);
    }
    if (pid == -1) {
        stat = -1; /* errno comes from fork() */
    } else {
        while (waitpid(pid, &stat, 0) == -1) {
            if (errno != EINTR){
                stat = -1;
                break;
            }
        }
    }
    sigaction(SIGINT, &savintr, (struct sigaction *)0);
    sigaction(SIGQUIT, &savequit, (struct sigaction *)0);
    sigprocmask(SIG_SETMASK, &saveblock, (sigset_t *)0);
    return(stat);
}

static void catsnprintf(char *str, size_t size, const char *format, ...)
{
    va_list ap;
    unsigned int len = strlen(str);

    str += len;
    size -= len;

    va_start(ap, format);
    vsnprintf(str, size, format, ap);
    va_end(ap);
}

void yavta_capture(bool data_prefix, bool mplane, bool userptr,
    unsigned int capture, unsigned int buffers,
    bool fill, unsigned int width, unsigned int height,
    bool write, const char *format, const char *video,
    bool use_isa_cfg_test, bool isa_active, unsigned int isa_stats, ...)
{
    char cmd[256] = "yavta";
    int pid = 0, status;

    if (data_prefix)
        catsnprintf(cmd, sizeof cmd, " --data-prefix");

    if (mplane)
        catsnprintf(cmd, sizeof cmd, " -B capture-mplane");

    if (userptr)
        catsnprintf(cmd, sizeof cmd, " -u");

    catsnprintf(cmd, sizeof cmd, " -c%u", SKIP_FRAMES + capture);

    catsnprintf(cmd, sizeof cmd, " -n%u", buffers);

    if (fill)
        catsnprintf(cmd, sizeof cmd, " -I");

    catsnprintf(cmd, sizeof cmd, " -s%ux%u", width, height);

    if (write)
        catsnprintf(cmd, sizeof cmd, " -F");

    catsnprintf(cmd, sizeof cmd, " -f %s", format);

    catsnprintf(cmd, sizeof cmd, " %s", video);

    if (use_isa_cfg_test) {
        va_list ap;
        char* test_vector_file = NULL;
        va_start(ap, isa_stats);
        test_vector_file = va_arg(ap, char*);
        printf("isa-----test: %s \n", test_vector_file);
        va_end(ap);

        char cfgcmd[256];
        snprintf(cfgcmd, sizeof cfgcmd,
                        "isa-cfg-test -d %s -3 %s "
                        "-c %s "
                        "-D %s "
                        "--dpc --lsc --blc --scaler --af --ae --awb "
                        "-N %d",
                        "/dev/video9",
                        "/dev/video10",
                        test_vector_file,
                        "/dev/v4l-subdev8",
                        SKIP_FRAMES + capture);

        printf("isa-cfg-test params %s\n", cfgcmd);

          if (!(pid = fork())) {
            exec_cmd(cfgcmd);
            exit(EXIT_SUCCESS);
        }
    }

    puts(cmd);
    EXPECT_EQ(exec_cmd(cmd), 0);

    if (pid) {
        pid = waitpid(pid, &status, 0);
        printf("status %d\n", status);
    }
}
