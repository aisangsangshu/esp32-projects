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
	gpio_config_t io_conf_leds = {
		.pin_bit_mask = (1ULL<<LED1_GPIO)|
			(1ULL<<LED2_GPIO)|
			(1ULL<<LED3_GPIO),
		.mode=GPIO_MODE_OUTPUT,
		.pull_up_en=GPIO_PULLUP_DISABLE,
		.pull_down_en=GPIO_PULLDOWN_DISABLE,
		.intr_type=GPIO_INTR_DISABLE
	};
	gpio_config(&io_conf_leds);

	gpio_set_level(LED1_GPIO, 0);
	gpio_set_level(LED2_GPIO, 0);
	gpio_set_level(LED3_GPIO, 0);

	gpio_config_t io_conf_button = {
		.pin_bit_mask=(1ULL<<BUTTON_GPIO),
		.mode=GPIO_MODE_INPUT,
		.pull_up_en=GPIO_PULLUP_ENABLE,
		.pull_down_en=GPIO_PULLDOWN_DISABLE,
		.intr_type=GPIO_INTR_DISABLE
	};
	gpio_config(&io_conf_button);
}

static void set_led_by_index(int index)
{
	gpio_set_level(LED1_GPIO, (index==0)?1:0);
	gpio_set_level(LED2_GPIO, (index==1)?1:0);
	gpio_set_level(LED3_GPIO, (index==2)?1:0);
}

void app_main(void)
{
	init_gpio();

	int current_led=0;
	set_led_by_index(current_led);
	printf("LED on: %d\n", current_led+1);

	int last_button_state=BUTTON_RELEASE_LEVEL;
	
	while(1){
		int button_state = gpio_get_level(BUTTON_GPIO);

		if (last_button_state == BUTTON_RELEASE_LEVEL && button_state == BUTTON_PRESSED_LEVEL){
			current_led = (current_led + 1)%3;
			set_led_by_index(current_led);
			printf("LED on: %d\n", current_led +1);
		}

		last_button_state = button_state;

		vTaskDelay(pdMS_TO_TICKS(20));
	}
}





































































