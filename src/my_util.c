/*
 * my_util.c
 *    by @omzn 2020/10/20
 */

#include "my_util.h"

//*****************************************************************************
//
// Delay time[ms]
//
//*****************************************************************************
inline void delay_ms(uint32_t ms) {
    uint32_t i;
    if(ms < 10){
        SysCtlDelay(SysCtlClockGet() * 10 / 3 / 1000);
    }else{
        for(i = ms/10 ; i > 0 ; i-- ){
            SysCtlDelay(SysCtlClockGet() * 10 / 3 / 1000);
        }
    }
}

uint8_t *itoh(uint32_t num,uint8_t length) {
    uint8_t hex2ascii_table[]={
            '0','1','2','3',
            '4','5','6','7',
            '8','9','A','B',
            'C','D','E','F'};
    static uint8_t string[8];
    uint8_t i;
    for (i = 0;i < length;i++) {
        string[length-i-1] = hex2ascii_table[(num >> (4*i)) & 0x0F];
    }
    string[length]='\0';
    return string;
}

// itoa(number,length)
// length: the resultant length of the string
uint8_t *itoa(int32_t num,uint8_t length) {
    static uint8_t string[DIGITS + 2];
    uint8_t *p = string + DIGITS + 1;
    if (num >= 0) {
        do {
            *--p = '0' + (num % 10);
            num /= 10;
            length--;
        } while (num != 0);
    } else {            /* i < 0 */
        do {
            *--p = '0' - (num % 10);
            num /= 10;
            length--;
        } while (num != 0);
        *--p = '-';
        length--;
    }
    while (length > 0) {
        length--;
        *--p = ' ';
    }
    return p;
}



