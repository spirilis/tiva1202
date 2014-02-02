/* nokia1202.c
 */

// needed for driverlib rom stuff
#define TARGET_IS_BLIZZARD_RA1 1

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/ssi.h"

#include "nokia1202_drv.h"
#include "ste2007.h"
#include "font_5x7.h"

int main()
{
    MAP_SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL |
                       SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);  // 80MHz CPU; speed is calculated as 200MHz/SYSDIV

    // SPI, GPIO config for Nokia 1202 LCD
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    MAP_GPIOPinConfigure(GPIO_PB4_SSI2CLK);
    MAP_GPIOPinConfigure(GPIO_PB6_SSI2RX);
    MAP_GPIOPinConfigure(GPIO_PB7_SSI2TX);
    MAP_GPIOPinTypeSSI(GPIO_PORTB_BASE, (GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_7));

    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI2);
    MAP_SSIConfigSetExpClk(SSI2_BASE, MAP_SysCtlClockGet(), SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 4000000, 8);
    MAP_SSIEnable(SSI2_BASE);

    // Need at least 250ms wait for Nokia LCD to be ready for commands after power-on.
    ROM_SysTickPeriodSet(80000*125);
    ROM_SysTickIntDisable();
    //HWREG(NVIC_ST_CURRENT_R) = 1;  // Doesn't work on TM4C1294NCPDT (need to omit HWREG())
    NVIC_ST_CURRENT_R = 1;
    ROM_SysTickEnable();
    while (ROM_SysTickValueGet() < 10000) ;
    while (ROM_SysTickValueGet() > 10000) ;
    while (ROM_SysTickValueGet() < 10000) ;
    while (ROM_SysTickValueGet() > 10000) ;
    ROM_SysTickDisable();

    // Display ready for init
    fopen("LCDBACKLIT", "w");
    char mylcd_buf[128];
    setvbuf(stdout, mylcd_buf, _IOLBF, 128);
    ste2007_contrast(12);
    printf("Totally cool! %g\n", 1.055);

    while(1) ;
}
