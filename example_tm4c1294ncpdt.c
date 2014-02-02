/* newlp1202.c
 */

// needed for driverlib rom stuff
#define TARGET_IS_SNOWFLAKE_RA0 1

#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c1294ncpdt.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/ssi.h"

#include <stdio.h>
#include "nokia1202_drv.h"
#include "ste2007.h"

int main()
{
    MAP_SysCtlClockFreqSet(SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_25MHZ | SYSCTL_CFG_VCO_480,
                           120000000);  // 120MHz CPU
    // SPI config
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    MAP_GPIOPinConfigure(GPIO_PD3_SSI2CLK);
    MAP_GPIOPinConfigure(GPIO_PD1_SSI2XDAT0);
    MAP_GPIOPinConfigure(GPIO_PD0_SSI2XDAT1);
    MAP_GPIOPinTypeSSI(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_3);
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI2);
    MAP_SSIConfigSetExpClk(SSI2_BASE, 120000000, SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 4000000, 8);
    MAP_SSIEnable(SSI2_BASE);

    // Wait 250ms for Nokia LCD to finish reset/wakeup
    MAP_SysTickPeriodSet(120000*125);
    MAP_SysTickIntDisable();
    NVIC_ST_CURRENT_R = 1;
    MAP_SysTickEnable();
    while (MAP_SysTickValueGet() < 100000) ;
    while (MAP_SysTickValueGet() > 100000) ;
    while (MAP_SysTickValueGet() < 100000) ;
    while (MAP_SysTickValueGet() > 100000) ;
    MAP_SysTickDisable();

    // Display ready for init
    fopen("LCDBACKLIT", "w");
    char mylcd_buf[256];
    setvbuf(stdout, mylcd_buf, _IOLBF, 256);
    ste2007_contrast(12);

    printf("Totally cool %g!  New TM4C129 LaunchPad!\n", 6.9);

    while(1) ;
}

