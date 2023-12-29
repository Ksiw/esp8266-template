#include "defines.h"
#include <Arduino.h>
#include "gpio.h"


static struct {
    bool f_button_pressed = false;
    unsigned long previous_pressed = 0;
}gpio;
//-------------------------------------------------------------------------------
void gpio_init()
{
    return;    
}
//-------------------------------------------------------------------------------

void gpio_process()
{
    return;    
}
//-------------------------------------------------------------------------------
