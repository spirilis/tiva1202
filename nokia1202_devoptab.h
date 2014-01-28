/* Nokia 1202 driver Newlib stubs
 * Intended for fopen(), printf() et al use in TI TivaWare applications.
 *
 * Copyright (c) 2014 Eric Brundick <spirilis [at] linux dot com>
 */

#ifndef NOKIA1202_DEVOPTAB_H
#define NOKIA1202_DEVOPTAB_H

#include <stdint.h>
#include "devoptab.h"


int nokia1202_open(const char *, int, int);
int nokia1202_open_backlit(const char *, int, int);
int nokia1202_close(int);
int nokia1202_read(int, char *, int);
int nokia1202_write(int, const char *, int);

extern const devoptab_t devoptab_nokia1202;
extern const devoptab_t devoptab_nokia1202_backlit;

#endif
