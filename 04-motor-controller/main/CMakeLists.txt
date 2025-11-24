idf_component_register(
    SRCS "04-motor-controller.c"
    INCLUDE_DIRS "."
    PRIV_REQUIRES esp_driver_ledc esp_adc esp_driver_gpio
)
