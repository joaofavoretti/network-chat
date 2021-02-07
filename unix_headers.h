/* unix_header.h */

#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#include <errno.h> // https://android.googlesource.com/kernel/lk/+/upstream-master/include/errno.h

#define GETSOCKETERRNO() (errno)
#define SOCKET int
#define ISVALIDSOCKET(socket) ((socket) >=0)
#define CLOSESOCKET(socket) close(socket)

#include <stdio.h>
#include <string.h>

#include <time.h>
#include <stdlib.h>
