#include <stdio.h>
#include <string.h>
#include <math.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <esp_err.h>
#include <driver/spi_master.h>
#include <driver/gpio.h>
#include <sdmmc_cmd.h>
#include <esp_vfs_fat.h>
#include <bmp280.h>
#include <i2cdev.h>

#define I2C_SDA 8
#define I2C_SCL 9
#define MOUNT_POINT "/sdcard"

static const char *TAG = "BMP_SD";

void bmp280_sd_task(void *pvParameters)
{
    ESP_LOGI(TAG, "Inicializando BMP280...");
    bmp280_t dev;
    bmp280_params_t params;
    bmp280_init_default_params(&params);
    memset(&dev, 0, sizeof(bmp280_t));
    ESP_ERROR_CHECK(bmp280_init_desc(&dev, 0x76, 0, I2C_SDA, I2C_SCL));
    ESP_ERROR_CHECK(bmp280_init(&dev, &params));
    ESP_LOGI(TAG, "BMP280 inicializado com sucesso!");

    ESP_LOGI(TAG, "Inicializando cartão SD...");
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.slot = SPI2_HOST;

    spi_bus_config_t bus_cfg = {
        .mosi_io_num = 11,
        .miso_io_num = 13,
        .sclk_io_num = 12,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };

    ESP_ERROR_CHECK(spi_bus_initialize(host.slot, &bus_cfg, SPI_DMA_CH_AUTO));

    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = 10;
    slot_config.host_id = host.slot;

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };

    sdmmc_card_t *card;
    esp_err_t ret = esp_vfs_fat_sdspi_mount(MOUNT_POINT, &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Falha ao montar cartão SD (%s)", esp_err_to_name(ret));
        vTaskDelete(NULL);
        return;
    }

    ESP_LOGI(TAG, "Cartão SD montado com sucesso!");

    FILE *f = fopen(MOUNT_POINT "/dados.txt", "w");
    if (f == NULL) {
        ESP_LOGE(TAG, "Falha ao abrir arquivo para escrita.");
        vTaskDelete(NULL);
        return;
    }

    ESP_LOGI(TAG, "Salvando dados no arquivo...");

    for (int i = 0; i < 10; i++) {
        float temperature, pressure, humidity;
        if (bmp280_read_float(&dev, &temperature, &pressure, &humidity) == ESP_OK) {
            fprintf(f, "Temperatura: %.2f C, Pressao: %.2f Pa\n", temperature, pressure);
            ESP_LOGI(TAG, "Dado salvo: %.2f C, %.2f Pa", temperature, pressure);
        } else {
            ESP_LOGW(TAG, "Falha na leitura do sensor.");
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    fclose(f);
    ESP_LOGI(TAG, "Arquivo salvo com sucesso!");

    esp_vfs_fat_sdcard_unmount(MOUNT_POINT, card);
    ESP_LOGI(TAG, "Cartão SD desmontado.");

    spi_bus_free(host.slot);
    vTaskDelete(NULL);
}

void app_main()
{
    ESP_ERROR_CHECK(i2cdev_init());
    xTaskCreate(bmp280_sd_task, "bmp280_sd_task", 4096 * 2, NULL, 5, NULL);
}
