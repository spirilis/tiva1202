/* Nokia 1202 driver library for TI TivaWare
 *
 * Copyright (c) 2014 Eric Brundick <spirilis [at] linux dot com>
 */

#ifndef NOKIA1202_DRV_H
#define NOKIA1202_DRV_H


#include <stdint.h>
#include <stdbool.h>


// LCD parameters
#define NOKIA1202_PIXEL_WIDTH 96
#define NOKIA1202_PIXEL_ROWS 68

// Text using 5x7 font (6x8 pixel matrix)
#define NOKIA1202_CHAR_WIDTH 6
#define NOKIA1202_COLUMNS (NOKIA1202_PIXEL_WIDTH / NOKIA1202_CHAR_WIDTH)
#define NOKIA1202_LINES (NOKIA1202_PIXEL_ROWS / 8)

#define NOKIA1202_CURSOR 0x80
// Comment out this line to disable cursor logic
#define NOKIA1202_USE_CURSOR 1

// Font-character-width of a tabstop (physically 6x<TAB_SPACING> pixels)
#define NOKIA1202_TAB_SPACING 4

// Backlight GPIO details (uses TivaWare GPIO API to configure & control)
// Tiva-C & Stellaris LaunchPad (TM4C123)
#define NOKIA1202_BACKLIGHT_GPIO_SYSCTL SYSCTL_PERIPH_GPIOB
#define NOKIA1202_BACKLIGHT_GPIO_BASE GPIO_PORTB_BASE
#define NOKIA1202_BACKLIGHT_GPIO_PIN GPIO_PIN_2
//
// Tiva-C Connected Ethernet LaunchPad (TM4C129) - BoosterPack "A" footprint
//#define NOKIA1202_BACKLIGHT_GPIO_SYSCTL SYSCTL_PERIPH_GPIOM
//#define NOKIA1202_BACKLIGHT_GPIO_BASE GPIO_PORTM_BASE
//#define NOKIA1202_BACKLIGHT_GPIO_PIN GPIO_PIN_3
//
// Comment this out to avoid the library having any control over backlight
#define NOKIA1202_CONTROL_BACKLIGHT 1

// SPI peripheral used for this along with ChipSelect GPIO pin.
#define SPI_SSI_BASE SSI2_BASE
#define SPI_SSI_CR0 SSI2_CR0_R
// ^ used to configure 9-bit SPI mode on-the-fly (and restore when done)


// GPIO pin used for the Chip Select, including the SysCtlPeripheralEnable() value.
// Tiva-C & Stellaris LaunchPad (TM4C123)
#define SPI_CHIPSELECT_GPIO_SYSCTL SYSCTL_PERIPH_GPIOA
#define SPI_CHIPSELECT_GPIO_BASE GPIO_PORTA_BASE
#define SPI_CHIPSELECT_GPIO_PIN GPIO_PIN_4
//
// Tiva-C Connected Ethernet LaunchPad (TM4C129) - BoosterPack "A" footprint
//#define SPI_CHIPSELECT_GPIO_SYSCTL SYSCTL_PERIPH_GPION
//#define SPI_CHIPSELECT_GPIO_BASE GPIO_PORTN_BASE
//#define SPI_CHIPSELECT_GPIO_PIN GPIO_PIN_2



/* Functions */
void nokia1202_init();
void nokia1202_putc(uint8_t c, uint8_t do_flush);
void nokia1202_flush();
void nokia1202_move(uint8_t x, uint8_t y);
void nokia1202_puts(const char *);
void nokia1202_putn(const char *, const int);
void nokia1202_backlight(const uint8_t);


#endif
