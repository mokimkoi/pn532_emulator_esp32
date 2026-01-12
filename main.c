#include <stdio.h>
#include <stdlib.h>
#include <esp_log.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "sdkconfig.h"
#include "pn532_driver_i2c.h"
#include "pn532_driver_hsu.h"
#include "pn532_driver_spi.h"
#include "pn532_i2c.h"


// select ONLY ONE interface for the PN532
#define PN532_MODE_I2C 1
#define PN532_MODE_HSU 0
#define PN532_MODE_SPI 

#if PN532_MODE_I2C

// I2C mode needs only SDA, SCL and IRQ pins. RESET pin will be used if valid.
// IRQ pin can be used in polling mode or in interrupt mode. Use menuconfig to select mode.
#define SCL_PIN    (21)
#define SDA_PIN    (23)
#define RESET_PIN  (-1)
#define IRQ_PIN    (-1)

#endif

static const char *TAG = "MAIN:PN532";

void app_main()
{
    pn532_io_t pn532_io;
    esp_err_t err;
    TagInfo *mainTag;
    TagInfo *secondaryTag;
    mainTag = (TagInfo *)malloc(sizeof(TagInfo));
    secondaryTag = (TagInfo *)malloc(sizeof(TagInfo));
    init_taginfo(mainTag);
    init_taginfo(secondaryTag); 

    printf("APP MAIN\n");

#if 1
    // Enable DEBUG logging
    esp_log_level_set("PN532", ESP_LOG_DEBUG);
    esp_log_level_set("pn532_driver", ESP_LOG_DEBUG);
    esp_log_level_set("pn532_driver_i2c", ESP_LOG_DEBUG);
    esp_log_level_set("i2c.master", ESP_LOG_DEBUG);
    esp_log_level_set("pn532_driver_hsu", ESP_LOG_DEBUG);
    esp_log_level_set("pn532_driver_spi", ESP_LOG_DEBUG);
    esp_log_level_set("spi", ESP_LOG_DEBUG);
#endif

    vTaskDelay(1000 / portTICK_PERIOD_MS);

#if PN532_MODE_I2C

    ESP_LOGI(TAG, "init PN532 in I2C mode");
    ESP_ERROR_CHECK(pn532_new_driver_i2c(SDA_PIN, SCL_PIN, RESET_PIN, IRQ_PIN, 0, &pn532_io));

#endif

    do {
        err = pn532_init(&pn532_io);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "failed to initialize PN532");
            pn532_release(&pn532_io);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    } while(err != ESP_OK);

    ESP_LOGI(TAG, "get firmware version");
    uint32_t version_data = 0;
    uint8_t status = 0;

    
    do {
        err = pn532_get_firmware_version(&pn532_io, &version_data);
        if (ESP_OK != err) {
            ESP_LOGI(TAG, "Didn't find PN53x board");
            pn532_reset(&pn532_io);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        } 
    } while (ESP_OK != err);
   
    do {
        err = pn532_get_general_status(&pn532_io, &status);
        if (ESP_OK != err) {
            ESP_LOGI(TAG, "can't get general status");
            pn532_reset(&pn532_io);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        } 
    } while (ESP_OK != err);
   
    do {
        err = pn532_setparameters(&pn532_io, 0b00110100);
        if (ESP_OK != err) {
            ESP_LOGI(TAG, "can't set parameter");
            pn532_reset(&pn532_io);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        } 
    } while (ESP_OK != err);
   
    // do {
    //     err = pn532_powerdown(&pn532_io, WU_SOURCE_I2C|WU_SOURCE_RF, 0);
    //     if (ESP_OK != err) {
    //         ESP_LOGI(TAG, "can't powerdown PN532");
    //         pn532_reset(&pn532_io);
    //         vTaskDelay(1000 / portTICK_PERIOD_MS);
    //     } 
    // } while (ESP_OK != err);
    err=pn532_set_passive_activation_retries(&pn532_io, 0x5);
    if (ESP_OK != err) {
        ESP_LOGI(TAG, "can't set passive activation retries");
        pn532_reset(&pn532_io);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    // do
    // {
    //     err = pn532_in_list_passive_target(&pn532_io);
    //     err=pn532_get_general_status(&pn532_io, &status);
    //     ESP_LOGI(TAG, "General status: 0x%02X", status);
    //     if (ESP_OK != err) {
    //         ESP_LOGI(TAG, "no card detected");
    //         pn532_reset(&pn532_io);
    //         vTaskDelay(1000 / portTICK_PERIOD_MS);
    //     } 
    //     vTaskDelay(1000 / portTICK_PERIOD_MS);
        
    // }while (1);
    do
    {
        err = pn532_inlistpassivetargetscan(&pn532_io, TYPEA_106K, 2, PN532_BRTY_ISO14443A_106KBPS, mainTag, secondaryTag);
        if (ESP_OK != err) {
            ESP_LOGI(TAG, "no card detected");
            pn532_reset(&pn532_io);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        } 
        ESP_LOGI(TAG, "Tags detected:");
        if (mainTag->tg_number >= 1 && mainTag != NULL) {
        ESP_LOGI(TAG, "Displaying Tag 1 info:");
        show_tag_info(mainTag);
        }
        if (secondaryTag->tg_number >= 1 && secondaryTag != NULL) {
        ESP_LOGI(TAG, "Displaying Tag 2 info:");
        show_tag_info(secondaryTag);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);

    }while (mainTag->tg_number == 0 );
    // if (mainTag->tg_number >= 1 && mainTag != NULL && mainTag->uidLength > 0) {
    //     ESP_LOGI(TAG, "Displaying Tag 1 info:");
    //     show_tag_info(mainTag);
    //     uint8_t **sector_data;
    //     uint8_t key []={0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    //     sector_data = (uint8_t **)malloc(4 * sizeof(uint8_t *));
    //     for (int i = 0; i < 4; i++) {
    //         sector_data[i] = (uint8_t *)malloc(16 * sizeof(uint8_t));
    //     }
    //     uint8_t sector_number=0;
    //     err = mfc_dump_sector_1K(&pn532_io, mainTag, sector_number,sector_data , MIFARE_CMD_AUTH_A, key);
    //     if (ESP_OK != err) {
    //         ESP_LOGI(TAG, "Failed to dump sector 5");
    //         return;
    //     }
    //     ESP_LOGI(TAG, "Sector 5 data:");
    //     ESP_LOGI(TAG, "after  Dumping sector 5 data:");
    //     for (int i = 0; i < 4; i++) {
    //         ESP_LOGI(TAG, " Block %d:", i + 24);
    //         ESP_LOG_BUFFER_HEXDUMP(TAG, sector_data[i], 16, ESP_LOG_INFO);
    //     }
    // }
    if (mainTag->tg_number >= 1 && mainTag != NULL && mainTag->uidLength > 0) {
        

        ESP_LOGI(TAG, "Displaying Tag 1 info:");
        show_tag_info(mainTag);

        uint8_t **sector_data;
        uint8_t key []={0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        
        sector_data = (uint8_t **)malloc(4 * sizeof(uint8_t *));
        for (int i = 0; i < 4; i++) {
            sector_data[i] = (uint8_t *)malloc(16 * sizeof(uint8_t));
        }
        uint8_t sector_number=0;

        
        err = mfc_dump_sector_1K(&pn532_io, mainTag, sector_number,sector_data , MIFARE_CMD_AUTH_A, key);
        if (ESP_OK != err) {
            ESP_LOGI(TAG, "Failed to dump sector 5");
            return;
        }
        ESP_LOGI(TAG, "Sector 5 data:");
        ESP_LOGI(TAG, "after  Dumping sector 5 data:");
        for (int i = 0; i < 4; i++) {
            ESP_LOGI(TAG, " Block %d:", i + 24);
            ESP_LOG_BUFFER_HEXDUMP(TAG, sector_data[i], 16, ESP_LOG_INFO);
        }
    }


    
    ESP_LOGI(TAG, "Found chip PN5%x", (unsigned int)(version_data >> 24) & 0xFF);
    ESP_LOGI(TAG, "Firmware ver. %d.%d", (int)(version_data >> 16) & 0xFF, (int)(version_data >> 8) & 0xFF);

    ESP_LOGI(TAG, "Waiting for an ISO14443A Card ...");
    while (0)
    {
        uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0}; // Buffer to store the returned UID
        uint8_t uid_length=0;                     // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

        // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
        // 'uid' will be populated with the UID, and uid_length will indicate
        // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
        
        
            // Display some basic information about the card
            ESP_LOGI(TAG, "\nFound an ISO14443A card");
            ESP_LOGI(TAG, "UID Length: %d bytes", uid_length);
            ESP_LOGI(TAG, "UID Value:");
            ESP_LOG_BUFFER_HEX_LEVEL(TAG, uid, uid_length, ESP_LOG_INFO);

            err = pn532_in_list_passive_target(&pn532_io);
            if (err != ESP_OK) {
                ESP_LOGI(TAG, "Failed to inList passive target");
                continue;
            }

            // NTAG2XX_MODEL ntag_model = NTAG2XX_UNKNOWN;
            // err = ntag2xx_get_model(&pn532_io, &ntag_model);
            // if (err != ESP_OK)
            //     continue;

            // int page_max;
            // switch (ntag_model) {
            //     case NTAG2XX_NTAG213:
            //         page_max = 45;
            //         ESP_LOGI(TAG, "found NTAG213 target (or maybe NTAG203)");
            //         break;

            //     case NTAG2XX_NTAG215:
            //         page_max = 135;
            //         ESP_LOGI(TAG, "found NTAG215 target");
            //         break;

            //     case NTAG2XX_NTAG216:
            //         page_max = 231;
            //         ESP_LOGI(TAG, "found NTAG216 target");
            //         break;

            //     default:
            //         ESP_LOGI(TAG, "Found unknown NTAG target!");
            //         continue;
            // }

            // for(int page=0; page < page_max; page+=4) {
            //     uint8_t buf[16];
            //     err = ntag2xx_read_page(&pn532_io, page, buf, 16);
            //     if (err == ESP_OK) {
            //         ESP_LOG_BUFFER_HEXDUMP(TAG, buf, 16, ESP_LOG_INFO);
            //     }
            //     else {
            //         ESP_LOGI(TAG, "Failed to read page %d", page);
            //         break;
            //     }
            // }
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        
    }
}
