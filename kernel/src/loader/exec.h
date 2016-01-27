/* This file is a part of the RazOS project
 *
 * exec.h -- exec(), the implementation behind the sys_exec()
 *
 * Razbit 2015, 2016 */

#ifndef EXEC_H
#define EXEC_H

uint32_t* execve(char* path, char** argv, char** envp);

#endif /* EXEC_H */
