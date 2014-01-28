/* Device Operations Table for Newlib device I/O
 *
 *
    Copyright (c) 2013 Eric Brundick <spirilis [at] linux dot com>

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use, copy,
    modify, merge, publish, distribute, sublicense, and/or sell copies
    of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */

#include <stdint.h>
#include <stdbool.h>
#include <reent.h>
#include "devoptab.h"
#include "nokia1202_devoptab.h"
#include <string.h>
#include <unistd.h>



/* List of registered drivers implementing open, close, read, write primitives.
 * Last entry should be NULL.
 */
const devoptab_t *devoptab_list[] = {
    &devoptab_nokia1202,
    &devoptab_nokia1202_backlit,
    NULL };

// File descriptor map (maps to devoptab_list[] index to determine which driver handles it)
static int devoptab_opens[MAX_FILEDES];
static uint32_t devoptab_init;

/* Utility function usable by drivers to determine the next available file descriptor
 * This intentionally ignores STDIN, STDOUT, STDERR as functions which provide one of the those
 * are expected to automatically assume those file descriptors.
 */
int devoptab_next_filedes(int prefer_fd)
{
    int fd;

    // Allow caller to use prefer_fd if it's available.
    if (prefer_fd >= 0 && prefer_fd < MAX_FILEDES && devoptab_opens[prefer_fd] < 0) {
        return prefer_fd;
    }
    for (fd = 3; fd < MAX_FILEDES; fd++) {
        if (devoptab_opens[fd] < 0)
            return fd;
    }
    return -1;
}

/* Newlib primitives for open, close, read, write */
int _write (int fd, const void *buf, size_t cnt)
{
    if (fd >= MAX_FILEDES || devoptab_opens[fd] < 0) {
        errno = ENODEV;
        return -1;
    }

    return devoptab_list[devoptab_opens[fd]]->write(fd, buf, cnt);
}

int _read (int fd, void *buf, size_t cnt)
{
    if (fd >= MAX_FILEDES || devoptab_opens[fd] < 0) {
        errno = ENODEV;
        return -1;
    }

    return devoptab_list[devoptab_opens[fd]]->read(fd, buf, cnt);
}

int _open (const char *file, int flags, int mode)
{
    int which_devoptab = 0;
    int fd, idstrlen;
    char *idstr = (char *)file, idstr_tmp[128];

    // Init catch
    if (devoptab_init != 0xDEADBEEF) {
        for (int i=0; i < MAX_FILEDES; i++) {
            devoptab_opens[i] = -1;
        }
        devoptab_init = 0xDEADBEEF;
    }

    /* Devices with /dev only match the "/dev/XXXXX" portion with the drivers, so a driver can implement
     * an open-ended filesystem underneath its /dev/XXXXX node.
     * The full path is still passed as the parameter to open().
     */
    if (!strncmp(file, "/dev/", 5) || !strncmp(file, "/sys/", 5)) {
        strncpy(idstr_tmp, file, 5);
        idstr = (char *)file+5;
        while (*idstr != '/' && (idstr-file) < 127)
            strncat(idstr_tmp, idstr++, 1);
        idstr = idstr_tmp;
    }
    idstrlen = strlen(idstr);

    do {
        if (strncmp(devoptab_list[which_devoptab]->name, idstr, idstrlen) == 0) {
            fd = devoptab_list[which_devoptab]->open(file, flags, mode);
            if (fd >= 0)
                devoptab_opens[fd] = which_devoptab;
            return fd;
        }
    } while (devoptab_list[which_devoptab++]);

    errno = ENODEV;
    return -1;
}

int _close (int fd)
{
    if (fd >= MAX_FILEDES || devoptab_opens[fd] < 0) {
        errno = ENODEV;
        return -1;
    }

    int _origopen = devoptab_opens[fd];
    devoptab_opens[fd] = -1;

    return devoptab_list[_origopen]->close(fd);
}

int _fstat(int fd, struct stat *buf)
{
    return 0;
}

int _isatty(int fd)
{
    if (fd <= STDERR_FILENO)
        return 1;
    errno = ENOTTY;
    return 0;
}

off_t _lseek(int fd, off_t offset, int whence)
{
    errno = ENOSYS;
    return -1;
}

/* _sbrk() needed since default newlib install doesn't have it.
 * Needed for malloc() et all to work.
 * Looks like newlib DOES have malloc, free, realloc, etc... but it needs sbrk
 * to define its interface with the actual SRAM heap.
 */
static char *heap_end = 0;
extern uint32_t _heap_bottom;
extern uint32_t _heap_top;

caddr_t _sbrk(unsigned int incr)
{
    char *prev_heap_end;

    if (heap_end == 0) {
        heap_end = (caddr_t) &_heap_bottom;
    }

    prev_heap_end = heap_end;
    if (heap_end + incr > (caddr_t)&_heap_top) {
        return (caddr_t)0;
    }

    heap_end += incr;
    return (caddr_t) prev_heap_end;
}
