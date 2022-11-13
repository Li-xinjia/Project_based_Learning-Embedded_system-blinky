/*
  Blinky
    @omzn  2020/10/20
*/
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include "inc/hw_i2c.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_qei.h"
#include "inc/hw_timer.h"
#include "driverlib/rom.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/pwm.h"
#include "driverlib/timer.h"
#include "driverlib/qei.h"
#include "utils/uartstdio.h"
#include "utils/uartstdio.c" //strong-arm method

#include "periphConf.h" //generated by PinMux

#include "my_util.h"


uint32_t base_led_color;
uint32_t led_color = LED_ALL;

bool led_on = false;

//*****************************************************************************
//
// This function sets up UART0 to be used for a console to display information
// as the example is running.
//
//*****************************************************************************
void initConsole(void) {
    // Enable GPIO port A which is used for UART0 pins.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    // Configure the pin muxing for UART0 functions on port A0 and A1.
    // This step is not necessary if your part does not support pin muxing.
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    // Enable UART0 so that we can configure the clock.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    // Use the internal 16MHz oscillator as the UART clock source.
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
    // Select the alternate (UART) function for these pins.
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    // Initialize the UART for console I/O.
    UARTStdioConfig(0, 9600, 16000000);
}

void initInterruptPins(void) {
    // You can write your own code here.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIODirModeSet(GPIO_PORTF_BASE, INT_LEFT_BUTTON, GPIO_DIR_MODE_IN);
    GPIOPadConfigSet(GPIO_PORTF_BASE, INT_LEFT_BUTTON,
                     GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOIntClear(GPIO_PORTF_BASE, INT_LEFT_BUTTON);

    GPIOIntRegister(GPIO_PORTF_BASE,SW1PinIntHandler);
    GPIOIntTypeSet(GPIO_PORTF_BASE,INT_LEFT_BUTTON,
                   GPIO_FALLING_EDGE);
    GPIOIntEnable(GPIO_PORTF_BASE,INT_LEFT_BUTTON);

    UARTprintf("Interrupt pins initiate over\n");
}

//*****************************************************************************
// Event handers
//*****************************************************************************

void SysTickIntHandler(void) {
    static uint32_t tick_count = 0;
    if (tick_count % 16 == 0) {
        tick_count = 0;
        /*
         * Note that the 2nd param of GPIOPinWrite denotes pins to be written in bit pattern.
         * The 3rd param of denotes values to be written in bit pattern.
         * What really written to GPIO pins is a logical AND of 2nd and 3rd params.
         */
        if (led_on) {
            GPIOPinWrite(GPIO_PORTF_BASE, base_led_color, led_color);
        } else {
            GPIOPinWrite(GPIO_PORTF_BASE, base_led_color, 0); // turn off all LEDs
        }
        led_on = !led_on;
    }
    tick_count++;
}

void SW1PinIntHandler(void) {
    GPIOIntDisable(GPIO_PORTF_BASE,INT_LEFT_BUTTON);
    GPIOIntClear(GPIO_PORTF_BASE,INT_LEFT_BUTTON);

    switch(led_color){
        case LED_BLUE:
            led_color = LED_RED;
            break;
        case LED_RED:
            led_color = LED_GREEN;
            break;
        case LED_GREEN:
            led_color = LED_WHITE;
            break;
        case LED_WHITE:
            led_color = LED_BLUE;
            break;
        default:
            break;
    }

    UARTprintf("SW1 pushed\n");
    GPIOIntEnable(GPIO_PORTF_BASE,INT_LEFT_BUTTON);
}

int main(void) {
    // Set the clocking to run directly from the crystal.
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                       SYSCTL_XTAL_16MHZ);
    // Set up ports hardware (see periphConf.c)
    PortFunctionInit();

    // Initialize console
    initConsole();

    // Set up interrupts (you can specify GPIO interrupt initialization here)
    initInterruptPins();

//  UARTprintf("Hello, world!\n");

    led_color = LED_BLUE;
    base_led_color = LED_ALL;

    GPIOIntEnable(GPIO_PORTF_BASE, INT_LEFT_BUTTON);
    GPIOPinWrite(GPIO_PORTF_BASE, base_led_color, led_color);

    SysTickPeriodSet(SysCtlClockGet() / SYSTICKS_PER_SEC);
    SysTickEnable();
    SysTickIntRegister(SysTickIntHandler);
    SysTickIntEnable();

    while (1);
}

