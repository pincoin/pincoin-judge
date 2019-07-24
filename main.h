#ifndef __main_h__
#define __main_h__

#define PID_STATUS_FILE_PATH_MAX 2048

static void run_solution(int argc, char *argv[]);
static void watch_program(pid_t pid);
static int get_memory_usage(char *pid_stauts_file_path);

#endif
