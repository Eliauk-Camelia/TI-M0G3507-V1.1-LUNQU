#include "led.h"


void LED_ON(void)
{
	DL_GPIO_clearPins(LED_PORT,LED_led_PIN);
}

void LED_OFF(void)
{
	DL_GPIO_setPins(LED_PORT,LED_led_PIN);
}

void LED_Toggle(void)
{
	DL_GPIO_togglePins(LED_PORT,LED_led_PIN);
}

void LED_Flash(uint16_t time)
{
	static uint16_t temp;
	if(time==0) LED_ON();
	else if(++temp==time) LED_Toggle(),temp=0;
}



