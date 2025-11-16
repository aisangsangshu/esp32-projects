#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED1_GPIO 4
#define LED2_GPIO 7
#define LED3_GPIO 17
#define BUTTON_GPIO 1

#define BUTTON_PRESSED_LEVEL 0
#define BUTTON_RELEASE_LEVEL 1

static void init_gpio(void)
{
	gpio_co_fig_t io_conf_leds = {
		.pin_bit_mask = (1ULL<<LED1_GPIO)|
			(1ULL<<LED2_GPIO)|
			(1ULL<<LED3_GPIO),
		.mode=GPIO_MODE_OUTPUT,
		.pull_up_en=GPIO_PULLUP_DISABLE,
		.pull_down_en=GPIO_PULLDOWN_DISABLE,
		.intr_type=GPIO_INTR_DISABLE
	};
	gpio_config(&io_conf_leds);

	gpio_set_level(LED1_GPIO, O);
	gpio_set_level(LED2_GPIO, O);
	gpio_set_level(LED3_GPIO, 0);

	gpio_config_t io_conf_button = {
		.pin_bit_mask=(1ULL<<BUTTON_GPIO),
		.mode = GPIO_MODE_INPUT,
		.pull_up_en=GPIO_PULLUP_ENABLE,
		.pull_down_en=GPIO_PULLDOWN_DISABLE
	};
	gpio_config(&io_conf_button);
}

static void set_led_by_index(int index)
{
	gpio_set_level(LED1_GPIO, (index==0)?1:0);
	gpio_set_level(LED2_GPIO, (index==1)?1:0);
	gpio_set_level(LED3_GPIO, (index==2)?1:0);
}


































