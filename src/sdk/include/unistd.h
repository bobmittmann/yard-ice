/* 
 * Copyright(c) 2004-2012 BORESTE (www.boreste.com). All Rights Reserved.
 *
 * This file is part of the YARD-ICE.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You can receive a copy of the GNU Lesser General Public License from 
 * http://www.gnu.org/
 */

/** 
 * @file unistd.h
 * @brief YARD-ICE libc
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef	__UNISTD_H__
#define	__UNISTD_H__

#define	__need_size_t
#define __need_NULL
#include <stddef.h>
#include <sys/types.h>
#include <stdint.h>

/* Standard file descriptors.  */
#define	STDIN_FILENO	0	/* Standard input.  */
#define	STDOUT_FILENO	1	/* Standard output.  */
#define	STDERR_FILENO	2	/* Standard error output.  */

/* Values for the second argument to access.
   These may be OR'd together.  */
#define	R_OK	4		/* Test for read permission.  */
#define	W_OK	2		/* Test for write permission.  */
#define	X_OK	1		/* Test for execute permission.  */
#define	F_OK	0		/* Test for existence.  */

typedef	unsigned int uid_t;
typedef	unsigned int gid_t;
typedef	unsigned int pid_t;

#ifdef __cplusplus
extern "C" {
#endif

/* Values for the WHENCE argument to lseek.  */
#ifndef	__STDIO_H__		/* <stdio.h> has the same definitions.  */
#define SEEK_SET	0	/* Seek from beginning of file.  */
#define SEEK_CUR	1	/* Seek from current position.  */
#define SEEK_END	2	/* Seek from end of file.  */
#endif

/* Move FD's file position to OFFSET bytes from the
   beginning of the file (if WHENCE is SEEK_SET),
   the current position (if WHENCE is SEEK_CUR),
   or the end of the file (if WHENCE is SEEK_END).
   Return the new file position.  */
off_t lseek(int __fd, off_t __offset, int __whence);

/* Close the file descriptor FD.

   This function is a cancellation point and therefore not marked with
   __THROW.  */
int close(int __fd);

/* Read NBYTES into BUF from FD.  Return the
   number read, -1 for errors or 0 for EOF.

   This function is a cancellation point and therefore not marked with
   __THROW.  */
ssize_t read(int __fd, void *__buf, size_t __nbytes);

/* Write N bytes of BUF to FD.  Return the number written, or -1.

   This function is a cancellation point and therefore not marked with
   __THROW.  */
ssize_t write(int __fd, const void *__buf, size_t __n);

/* Create a one-way communication channel (pipe).
   If successful, two file descriptors are stored in PIPEDES;
   bytes written on PIPEDES[1] can be read from PIPEDES[0].
   Returns 0 if successful, -1 if not.  */
int pipe(int __pipedes[2]);

/* Schedule an alarm.  In SECONDS seconds, the process will get a SIGALRM.
   If SECONDS is zero, any currently scheduled alarm will be cancelled.
   The function returns the number of seconds remaining until the last
   alarm scheduled would have signaled, or zero if there wasn't one.
   There is no return value to indicate an error, but you can set `errno'
   to 0 and check its value after calling `alarm', and this might tell you.
   The signal may come late due to processor scheduling.  */
unsigned int alarm(unsigned int __seconds);

/* Make the process sleep for SECONDS seconds, or until a signal arrives
   and is not ignored.  The function returns the number of seconds less
   than SECONDS which it actually slept (thus zero if it slept the full time).
   If a signal handler does a `longjmp' or modifies the handling of the
   SIGALRM signal while inside `sleep' call, the handling of the SIGALRM
   signal afterwards is undefined.  There is no return value to indicate
   error, but if `sleep' returns SECONDS, it probably didn't work.

   This function is a cancellation point and therefore not marked with
   __THROW.  */
unsigned int sleep(unsigned int __seconds);

void usleep(unsigned long __usec);

/* Suspend the process until a signal arrives.
   This always returns -1 and sets `errno' to EINTR.

   This function is a cancellation point and therefore not marked with
   __THROW.  */
int pause(void);

/* Change the owner and group of FILE.  */
int chown(const char *__file, uid_t __owner, gid_t __group);

/* Change the process's working directory to PATH.  */
int chdir(const char *__path);

/* Get the pathname of the current working directory,
   and put it in SIZE bytes of BUF.  Returns NULL if the
   directory couldn't be determined or SIZE was too small.
   If successful, returns BUF.  In GNU, if BUF is NULL,
   an array is allocated with `malloc'; the array is SIZE
   bytes long, unless SIZE == 0, in which case it is as
   big as necessary.  */
char * getcwd(char * __buf, size_t __size);

/* Duplicate FD, returning a new file descriptor on the same file.  */
int dup(int __fd);

/* Duplicate FD to FD2, closing FD2 and making it open on the same file.  */
int dup2(int __fd, int __fd2);

/* NULL-terminated array of "NAME=VALUE" environment variables.  */
extern char **__environ;

/* Replace the current process, executing PATH with arguments ARGV and
   environment ENVP.  ARGV and ENVP are terminated by NULL pointers.  */
int execve(const char * __path, char *const __argv[], char *const __envp[]);

/* Execute PATH with arguments ARGV and environment from `environ'.  */
int execv(const char * __path, char *const __argv[]);

/* Execute PATH with all arguments after PATH until a NULL pointer,
   and the argument after that for environment.  */
int execle(const char * __path, const char *__arg, ...);

/* Execute PATH with all arguments after PATH until
   a NULL pointer and environment from `environ'.  */
int execl(const char * __path, const char *__arg, ...);

/* Execute FILE, searching in the `PATH' environment variable if it contains
   no slashes, with arguments ARGV and environment from `environ'.  */
int execvp(const char * __file, char *const __argv[]);

/* Execute FILE, searching in the `PATH' environment variable if
   it contains no slashes, with all arguments after FILE until a
   NULL pointer and environment from `environ'.  */
int execlp(const char *__file, const char *__arg, ...);

/* Terminate program execution with the low-order 8 bits of STATUS.  */
void _exit(int __status) __attribute__((__noreturn__));


/* Get the `_PC_*' symbols for the NAME argument to `pathconf' and `fpathconf';
   the `_SC_*' symbols for the NAME argument to `sysconf';
   and the `_CS_*' symbols for the NAME argument to `confstr'.  */
 /* #include <sys/confname.h> */

/* Get file-specific configuration information about PATH.  */
long int pathconf(const char *__path, int __name);

/* Get file-specific configuration about descriptor FD.  */
long int fpathconf(int __fd, int __name);

/* Get the value of the system variable NAME.  */
long int sysconf(int __name) __attribute__ ((__const__));

/* Get the process ID of the calling process.  */
pid_t getpid(void);

/* Get the process ID of the calling process's parent.  */
pid_t getppid(void);

/* Get the process group ID of process PID.  */
pid_t __getpgid (pid_t __pid);

/* Set the process group ID of the process matching PID to PGID.
   If PID is zero, the current process's process group ID is set.
   If PGID is zero, the process ID of the process is used.  */
int setpgid (pid_t __pid, pid_t __pgid);

/* Create a new session with the calling process as its leader.
   The process group IDs of the session and the calling process
   are set to the process ID of the calling process, which is returned.  */
pid_t setsid(void);

/* Get the real user ID of the calling process.  */
uid_t getuid(void);

/* Get the effective user ID of the calling process.  */
uid_t geteuid(void);

/* Get the real group ID of the calling process.  */
gid_t getgid(void);

/* Get the effective group ID of the calling process.  */
gid_t getegid(void);

/* If SIZE is zero, return the number of supplementary groups
   the calling process is in.  Otherwise, fill in the group IDs
   of its supplementary groups in LIST and return the number written.  */
int getgroups(int __size, gid_t __list[]);

/* Set the user ID of the calling process to UID.
   If the calling process is the super-user, set the real
   and effective user IDs, and the saved set-user-ID to UID;
   if not, the effective user ID is set to UID.  */
int setuid(uid_t __uid);

/* Set the group ID of the calling process to GID.
   If the calling process is the super-user, set the real
   and effective group IDs, and the saved set-group-ID to GID;
   if not, the effective group ID is set to GID.  */
int setgid(gid_t __gid);

/* Clone the calling process, creating an exact copy.
   Return -1 for errors, 0 to the new process,
   and the process ID of the new process to the old process.  */
pid_t fork(void);

/* Return the pathname of the terminal FD is open on, or NULL on errors.
   The returned storage is good only until the next call to this function.  */
char *ttyname (int __fd);

/* Store at most BUFLEN characters of the pathname of the terminal FD is
   open on in BUF.  Return 0 on success, otherwise an error number.  */
int ttyname_r(int __fd, char *__buf, size_t __buflen);

/* Return 1 if FD is a valid descriptor associated
   with a terminal, zero if not.  */
int isatty(int __fd);

/* Make a link to FROM named TO.  */
int link(const char *__from, const char *__to);

/* Remove the link NAME.  */
int unlink(const char *__name);

/* Remove the directory PATH.  */
int rmdir(const char *__path);

/* Return the foreground process group ID of FD.  */
pid_t tcgetpgrp(int __fd);

/* Set the foreground process group ID of FD set PGRP_ID.  */
int tcsetpgrp(int __fd, pid_t __pgrp_id);


/* Set the end of accessible data space (aka "the break") to ADDR.
   Returns zero on success and -1 for errors (with errno set).  */
int brk (void * __addr);

/* Increase or decrease the end of accessible data space by DELTA bytes.
   If successful, returns the address the previous end of data space
   (i.e. the beginning of the new space, if DELTA > 0);
   returns (void *) -1 for errors (with errno set).  */

void * sbrk (intptr_t __delta);


/* Return the login name of the user. */
char * getlogin(void);

#ifdef __cplusplus
}
#endif

#endif /* __UNISTD_H__ */
/*! @} */
/*! @} */
