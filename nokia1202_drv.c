/* Nokia 1202 driver library + Newlib stubs for TI TivaWare
 *
 * Copyright (c) 2014 Eric Brundick <spirilis [at] linux dot com>
 */

#include "hw_inc.h"
#include "driverlib/gpio.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "nokia1202_drv.h"
#include "nokia1202_devoptab.h"
#include "devoptab.h"
#include "font_5x7.h"
#include "ste2007.h"

/* Core Nokia 1202 framebuffer/terminal driver; relies on ste2007_* library for low-level I/O */
#define NOKIA1202_FRAMEBUFFER_SIZE (NOKIA1202_COLUMNS * NOKIA1202_LINES)
static uint8_t _framebuffer[NOKIA1202_FRAMEBUFFER_SIZE];
static uint16_t _dirtybits[ (NOKIA1202_COLUMNS/16) * NOKIA1202_LINES ];
static uint8_t _x, _y;

void nokia1202_init()
{
    memset(_framebuffer, ' ', NOKIA1202_FRAMEBUFFER_SIZE);
    _x = 0;
    _y = 0;
    ste2007_init();

#ifdef NOKIA1202_USE_CURSOR
    _framebuffer[_y * NOKIA1202_COLUMNS + _x] = NOKIA1202_CURSOR;
    _dirtybits[_y] |= 1 << _x;
    nokia1202_flush();
#endif

#ifdef NOKIA1202_CONTROL_BACKLIGHT
    MAP_SysCtlPeripheralEnable(NOKIA1202_BACKLIGHT_GPIO_SYSCTL);
    MAP_GPIOPinTypeGPIOOutput(NOKIA1202_BACKLIGHT_GPIO_BASE, NOKIA1202_BACKLIGHT_GPIO_PIN);
    MAP_GPIOPinWrite(NOKIA1202_BACKLIGHT_GPIO_BASE, NOKIA1202_BACKLIGHT_GPIO_PIN, 0);  // Backlight OFF by default
#endif
}

void nokia1202_putc(uint8_t c, uint8_t doflush)
{
    uint8_t i = 0;

    if (c >= 0x20) {
        _framebuffer[_y * NOKIA1202_COLUMNS + _x] = c;

        // Flag dirty buffer
        _dirtybits[_y] |= 1 << _x;
        _x++;
    } else {
        // Process control character
        switch (c) {
            case '\n':
#ifdef NOKIA1202_USE_CURSOR
                // Erase the cursor presently at _x,_y before moving it
                _framebuffer[_y * NOKIA1202_COLUMNS + _x] = ' ';
                _dirtybits[_y] |= 1 << _x;
#endif
                _x = 0;
                _y++;
                break;
            case '\t':
#ifdef NOKIA1202_USE_CURSOR
                // Erase the cursor presently at _x,_y before moving it
                _framebuffer[_y * NOKIA1202_COLUMNS + _x] = ' ';
                _dirtybits[_y] |= 1 << _x;
#endif
                if (_x % NOKIA1202_TAB_SPACING == 0) {
                    _x += NOKIA1202_TAB_SPACING;
                } else {
                    _x += _x % NOKIA1202_TAB_SPACING;
                }
                break;
            case '\b':
                if (_x > 0) {  // Nothing happens if @ beginning of line
                    _x--;
                    // Otherwise, the previous character gets erased.
                    _framebuffer[_y * NOKIA1202_COLUMNS + _x] = ' ';
                    _dirtybits[_y] |= 1 << _x;
                }
                break;
            // No default section; any other ctrl char is ignored
        }
    }

    if (_x >= NOKIA1202_COLUMNS) {
        // Shift down one row
        _y++;
        _x = 0;
    }

    if (_y >= NOKIA1202_LINES) {
        // Ut oh, we must scroll...
        for (i=1; i < NOKIA1202_LINES; i++) {
            memcpy(_framebuffer+(i-1)*NOKIA1202_COLUMNS,
                   _framebuffer+i*NOKIA1202_COLUMNS,
                   NOKIA1202_COLUMNS);
            _dirtybits[i-1] = 0xFFFF;
        }
        _y = NOKIA1202_LINES-1;
        memset(_framebuffer + _y*NOKIA1202_COLUMNS,
               ' ',
               NOKIA1202_COLUMNS);  // Clear last line
        _dirtybits[_y] = 0xFFFF;
    }

#ifdef NOKIA1202_USE_CURSOR
    _framebuffer[_y * NOKIA1202_COLUMNS + _x] = NOKIA1202_CURSOR;
    _dirtybits[_y] |= 1 << _x;
#endif

    if (doflush)
        nokia1202_flush();
}

void nokia1202_flush()
{
    uint16_t i, j;

    for (i=0; i < NOKIA1202_LINES; i++) {
        if (_dirtybits[i]) {
            if (_dirtybits[i] == 0xFFFF) { // Quick optimization for refreshing a whole line
                ste2007_setxy(0, i);
                ste2007_chipselect(0);
                for (j=0; j < NOKIA1202_COLUMNS; j++) {
                    ste2007_write(font_5x7[_framebuffer[i*NOKIA1202_COLUMNS+j]-' '], NOKIA1202_CHAR_WIDTH);
                }
                ste2007_chipselect(1);
            } else {
                for (j=0; j < NOKIA1202_COLUMNS; j++) {
                    if (_dirtybits[i] & (1 << j)) {
                        ste2007_setxy(j*NOKIA1202_CHAR_WIDTH, i);
                        ste2007_chipselect(0);
                        ste2007_write(font_5x7[_framebuffer[i*NOKIA1202_COLUMNS+j]-' '], NOKIA1202_CHAR_WIDTH);
                        ste2007_chipselect(1);
                        _dirtybits[i] &= ~(1 << j);  // Flushed; clear bit
                    }
                }
            }
        }
    }
}

void nokia1202_move(uint8_t x, uint8_t y)
{
#ifdef NOKIA1202_USE_CURSOR
    _framebuffer[_y * NOKIA1202_COLUMNS + _x] = ' ';
    _dirtybits[_y] |= 1 << _x;
#endif
    _x = x;
    _y = y;
#ifdef NOKIA1202_USE_CURSOR
    _framebuffer[_y * NOKIA1202_COLUMNS + _x] = NOKIA1202_CURSOR;
    _dirtybits[_y] |= 1 << _x;
    nokia1202_flush();  // Flush only necessary if we're using a cursor.
#endif
}

void nokia1202_puts(const char *str)
{
    uint16_t i, j;

    j = strlen(str);
    for (i=0; i < j; i++) {
        nokia1202_putc((uint8_t)str[i], 0);
    }
    nokia1202_flush();
}

void nokia1202_putn(const char *str, const int len)
{
    uint16_t i;

    for (i=0; i < len; i++) {
        nokia1202_putc((uint8_t)str[i], 0);
    }
    nokia1202_flush();
}

void nokia1202_backlight(const uint8_t onoff)
{
#ifdef NOKIA1202_CONTROL_BACKLIGHT
    MAP_GPIOPinWrite(NOKIA1202_BACKLIGHT_GPIO_BASE, NOKIA1202_BACKLIGHT_GPIO_PIN, (onoff ? NOKIA1202_BACKLIGHT_GPIO_PIN : 0));
#endif
}

/* Newlib stubs */

const devoptab_t devoptab_nokia1202 = {
    "LCD",
    nokia1202_open,
    nokia1202_close,
    nokia1202_write,
    nokia1202_read
};

const devoptab_t devoptab_nokia1202_backlit = {
    "LCDBACKLIT",
    nokia1202_open_backlit,
    nokia1202_close,
    nokia1202_write,
    nokia1202_read
};

static int myflags, mymode;

int nokia1202_open(const char *path, int flags, int mode)
{
    int fd = devoptab_next_filedes(STDOUT_FILENO);
    if (fd < 0) {
        errno = ENFILE;
        return -1;
    }
    myflags = flags; mymode = mode;
    nokia1202_init();
    errno = 0;
    return fd;
}

void nokia1202_dumpflags()
{
    printf("open called with flags = %d, mode = %d\n", myflags, mymode);
}

int nokia1202_open_backlit(const char *path, int flags, int mode)
{
    int fd = nokia1202_open(path, flags, mode);
    if (fd >= 0)
        nokia1202_backlight(true);
    return fd;
}

int nokia1202_close(int fd)
{
    nokia1202_backlight(false);
    ste2007_init();
    errno = 0;
    return 0;
}

int nokia1202_read(int fd, char *ptr, int len)
{
    errno = ENOSYS;
    return -1;
}

int nokia1202_write(int fd, const char *ptr, int len)
{
    nokia1202_putn(ptr, len);
    errno = 0;
    return 0;
}

