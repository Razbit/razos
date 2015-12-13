/* This file is a part of the RazOS project
 *
 * exec.h -- do_execve(), the implementation behind the sys_execve()
 *
 * Razbit 2015 */

#ifndef EXEC_H
#define EXEC_H

int do_execve(char* path, char** argv, char** envp);

#endif /* EXEC_H */
