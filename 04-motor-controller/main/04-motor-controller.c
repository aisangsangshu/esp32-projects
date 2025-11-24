//#include <inttypes.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include "esp_err.h"

// =====================
// Configuración de pines
// =====================

#define ESC_GPIO            26                 // señal al ESC (cable naranja/blanco)
#define POT_ADC_UNIT        ADC_UNIT_1        // ADC1
#define POT_ADC_CHANNEL     ADC_CHANNEL_6     // ADC1_CH6 = GPIO34

// =====================
// Configuración LEDC (PWM)
// =====================

#define LEDC_TIMER          LEDC_TIMER_0
#define LEDC_MODE           LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL        LEDC_CHANNEL_0
#define LEDC_DUTY_RES       LEDC_TIMER_14_BIT   // 16 bits -> 0..65535
#define LEDC_FREQUENCY      50                 // 50 Hz (periodo 20 ms)

// =====================
// Pulsos para el ESC
// =====================

#define PULSE_MIN_US        1000               // 1 ms -> mínimo
#define PULSE_MAX_US        2000               // 2 ms -> máximo
#define PWM_PERIOD_US       20000              // 20 ms -> 50 Hz

static const char *TAG = "MOTOR_CTRL";

// Handle global del ADC oneshot
static adc_oneshot_unit_handle_t s_adc_handle = NULL;

// =====================
// Inicialización LEDC
// =====================

static void esc_pwm_init(void)
{
    // Temporizador
    ledc_timer_config_t timer_conf = {
        .speed_mode       = LEDC_MODE,
        .duty_resolution  = LEDC_DUTY_RES,
        .timer_num        = LEDC_TIMER,
        .freq_hz          = LEDC_FREQUENCY,
        .clk_cfg          = LEDC_AUTO_CLK,
    };
    ESP_ERROR_CHECK(ledc_timer_config(&timer_conf));

    // Canal
    ledc_channel_config_t ch_conf = {
        .gpio_num   = ESC_GPIO,
        .speed_mode = LEDC_MODE,
        .channel    = LEDC_CHANNEL,
        .intr_type  = LEDC_INTR_DISABLE,
        .timer_sel  = LEDC_TIMER,
        .duty       = 0,
        .hpoint     = 0,
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ch_conf));
}

// =====================
// Inicialización ADC (potenciómetro)
// =====================

static void pot_adc_init(void)
{
    // Unidad ADC1
    adc_oneshot_unit_init_cfg_t unit_cfg = {
        .unit_id  = POT_ADC_UNIT,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&unit_cfg, &s_adc_handle));

    // Canal: ADC1_CH6 (GPIO34)
    adc_oneshot_chan_cfg_t chan_cfg = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,  // por defecto, máximo (12 bits en ESP32)
        .atten    = ADC_ATTEN_DB_12,       // para medir hasta ~3.3 V
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(s_adc_handle, POT_ADC_CHANNEL, &chan_cfg));
}

// =====================
// Enviar pulso al ESC
// =====================

static void esc_set_pulse(uint32_t pulse_us)
{
    if (pulse_us < PULSE_MIN_US) pulse_us = PULSE_MIN_US;
    if (pulse_us > PULSE_MAX_US) pulse_us = PULSE_MAX_US;

    const uint32_t max_duty = (1U << LEDC_DUTY_RES) - 1U;      // 65535 con 16 bits
    uint32_t duty = (pulse_us * max_duty) / PWM_PERIOD_US;     // escala lineal

    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
}

// =====================
// app_main
// =====================

void app_main(void)
{
    esc_pwm_init();
    pot_adc_init();

    // Acelerador al mínimo y tiempo para que el ESC arme
    esc_set_pulse(PULSE_MIN_US);
    ESP_LOGI(TAG, "ESC armado, manteniendo pulso minimo");
    vTaskDelay(pdMS_TO_TICKS(3000));

    while (1) {
        // 1) Leer potenciómetro (raw 0..4095 aprox)
         int raw = 0;
        ESP_ERROR_CHECK(adc_oneshot_read(s_adc_handle, POT_ADC_CHANNEL, &raw));

        // 2) Mapear a 1000..2000 us
        uint32_t pulse = PULSE_MIN_US
                         + ((uint32_t)raw * (PULSE_MAX_US - PULSE_MIN_US)) / 4095U;

        // 3) Enviar pulso al ESC
        esc_set_pulse(pulse);

        // 4) Log opcional
        ESP_LOGI(TAG, "raw=%d  pulse=%u us", raw, (unsigned int)pulse);
        ESP_LOGI(TAG, "raw=%d  pulse=%" PRIu32 " us", raw, (uint32_t)pulse);
        // 5) 20 ms de delay (~50 Hz de actualización)
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}
