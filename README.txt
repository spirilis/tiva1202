Using the Nokia 1202 TivaWare library.

This library is designed to be incorporated directly into your code project, with "nokia1202_drv.h" being
a single point of custom configuration.

The library utilizes SPI along with GPIO for driving the ChipSelect line, and optionally GPIO for driving
the Backlight.

Backlight support may be omitted by commenting out a line in "nokia1202_drv.h"

The library portrays the Nokia 1202 LCD as a 16x8 character terminal supporting an underline-style non-blinking
cursor (optional; feature can be commented out in "nokia1202_drv.h") and rudimentary formatting including
tabs (\t), newlines (\n) and backspacing (\b).

This library seeks to optimize the I/O to the LCD by using an internal 128-byte framebuffer and queueing changes
before "flushing" them.  Any extraneous direct access to the LCD outside of this may result in corrupted LCD
contents should a scroll-event occur.

The library also supports scrolling; writing off the bottom end of the display causes an immediate upward
scrolling of text.  Again, these changes are buffered by an internal framebuffer until a "flush" can be
committed.

As this library is founded on a more generic "STE2007" controller library I wrote, there are direct-access functions
available under the ste2007_* function naming convention that may be used to configure features; such as enable
PowerSave mode, invert display, change contrast, etc.  These functions will use the SPI peripheral and defined
GPIO pin for SPI ChipSelect as is defined inside "nokia1202_drv.h"

The final feature of this library is Newlib STDIO stub support.  The library contains a rudimentary driver for
implementing POSIX open(), close(), read() (not implemented here) and write() support; a driver supporting up to
16 file descriptors (configurable) is included that may be used with other hardware drivers supporting its
scheme.  This is also optional; leaving out "devoptab.c" from the compilation should remove this functionality.

The newlib stub driver for the Nokia 1202 LCD support two filenames for opening; "LCD" and "LCDBACKLIT".  The former
does nothing with the backlight; the latter turns on the backlight.  If the user would prefer to control the
backlight themselves using PWM, they may ignore the "LCDBACKLIT" option and drive it externally.  Alternately,
a function "nokia1202_backlight(true|false)" exists that will turn the backlight GPIO on/off, if backlight support
is included through "nokia1202_drv.h"

The newlib stub driver prefers to open the device as STDOUT_FILENO, so that "printf()" will use it by default.
If you have already registered the STDOUT_FILENO with another driver before issuing fopen() to the LCD, it will
use the next available file descriptor (from 3+) and you will have to utilize fprintf() with the FILE handle
returned by fopen().

One final note- included in this is an example startup_gcc.c and linker script for the Tiva-C LaunchPad.  It is
important to include the heap- and stack-related definitions in the linker script so that Newlib's malloc() support
will work correctly for printf()/fprintf() et al.  startup_gcc.c likewise has modifications to support the stack-location
definition support inside that linker script; this differs from the original TivaWare example in that it puts
stack at the very top of RAM, as is customary for most system designs, rather than allocating a fixed 64-word section
of SRAM down in the globals for the stack.


