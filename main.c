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
#include "mifare_tag.h"


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
    // do
    // {
    //     err = pn532_inlistpassivetargetscan(&pn532_io, TYPEA_106K, 2, PN532_BRTY_ISO14443A_106KBPS, mainTag, secondaryTag);
    //     if (ESP_OK != err) {
    //         ESP_LOGI(TAG, "no card detected");
    //         pn532_reset(&pn532_io);
    //         vTaskDelay(1000 / portTICK_PERIOD_MS);
    //     } 
    //     ESP_LOGI(TAG, "Tags detected:");
    //     if (mainTag->tg_number >= 1 && mainTag != NULL) {
    //     ESP_LOGI(TAG, "Displaying Tag 1 info:");
    //     show_tag_info(mainTag);
    //     }
    //     if (secondaryTag->tg_number >= 1 && secondaryTag != NULL) {
    //     ESP_LOGI(TAG, "Displaying Tag 2 info:");
    //     show_tag_info(secondaryTag);
    //     }
    //     vTaskDelay(1000 / portTICK_PERIOD_MS);

    // }while (mainTag->tg_number == 0 );
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

        uint8_t ***tag_data;
        mifare_classic_1k_t tag1k ={0} ;
        //uint8_t key []={0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        uint8_t key []={0x4A,0x63 ,0x52,0x68 ,0x46 ,0x77};
        
        tag_data =(uint8_t ***)malloc(16*sizeof(uint8_t**));
        for (int j = 0; j < 16; j++) {
            tag_data[j]=(uint8_t **)malloc(4*sizeof(uint8_t*));
            for (int i = 0; i < 4; i++) {
                tag_data[j][i] = (uint8_t *)malloc(16 * sizeof(uint8_t));
            }
        }
        uint8_t nb_poll=0x03;
        uint8_t nb_types=1;
        uint8_t period=0x06;
        uint8_t type[]={0x10};
        err=pn532_inautopoll(&pn532_io,nb_poll,period,nb_types,type);
        if (ESP_OK != err){
            ESP_LOGI(TAG, "polling failed **-*-*-*-* ");
        }
        
        
        // err = mfc_dump_1k_tag(&pn532_io, mainTag,tag_data , MIFARE_CMD_AUTH_A, key);
        // if (ESP_OK != err) {
        //     ESP_LOGI(TAG, "Failed to dump tag 1K classic mifare ");
        //     return;
        // }
        // err=show_tag_1k_data(tag_data);
        // if (err!=ESP_OK){
        //     ESP_LOGI(TAG, "failed to show tag data ");
        // }
        
        // if (ESP_OK != pn532_indeselect(&pn532_io,mainTag->tg_number)){
        //     ESP_LOGI(TAG, "deselecting failed  ************  ");
        // }
        // if (ESP_OK != pn532_inselect(&pn532_io,mainTag->tg_number)){
        //     ESP_LOGI(TAG, "selecting failed  ************  ");
        // }
        // do{
        //     err=mfc_authenticate_block(&pn532_io,mainTag,25,MIFARE_CMD_AUTH_A,NULL);
        //     if (ESP_OK != err) {
        //         ESP_LOGI(TAG, "Failed to auth with keyA -------------------------------------  ");
        //     vTaskDelay(5000 / portTICK_PERIOD_MS);
        //     }

            
        // }while (ESP_OK != err);    

        // err=mfc_authenticate_block(&pn532_io,mainTag,25,MIFARE_CMD_AUTH_B,NULL);
        // if (ESP_OK != err) {
        //     ESP_LOGI(TAG, "Failed to auth with keyB  ");
        // }
        // uint8_t **data;
        // data =(uint8_t **)malloc(4 * sizeof(uint8_t *));
        // for (int i = 0; i < 4; i++) {
        //     data[i] = (uint8_t *)malloc(16 * sizeof(uint8_t));
        // }

        // uint8_t key1 []={0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        // err=mfc_dump_sector_1K(&pn532_io, mainTag,14,data , MIFARE_CMD_AUTH_A, NULL);
        // if (err!=ESP_OK){
        //     ESP_LOGI(TAG, "failed to sector data ");
        // }

        // err=parse_pdata_to_tag(tag_data,&tag1k);
        // if (err!=ESP_OK){
        //     ESP_LOGI(TAG, "failed to parse tag data ");
        // }
        // err=show_tag_1k_data_struct(&tag1k);
    }
    uint8_t nb_poll=0x03;
    uint8_t nb_types=1;
    uint8_t period=0x06;
    uint8_t type[]={0x00};
    ESP_LOGI(TAG,"HELLLLLLLO");
    do{
    err=pn532_inautopoll(&pn532_io,nb_poll,period,nb_types,type);
    if (ESP_OK != err){
        ESP_LOGI(TAG, "polling failed **-*-*-*-* ");
    }
    }while(err!=ESP_OK)  ; 
    // err=pn532_inautopoll(&pn532_io,nb_poll,period,nb_types,type);
    // if (ESP_OK != err){
    //     ESP_LOGI(TAG, "polling failed **-*-*-*-* ");
    // }


    ESP_LOGI(TAG, "Found chip PN5%x", (unsigned int)(version_data >> 24) & 0xFF);
    ESP_LOGI(TAG, "Firmware ver. %d.%d", (int)(version_data >> 16) & 0xFF, (int)(version_data >> 8) & 0xFF);

    ESP_LOGI(TAG, "Waiting for an ISO14443A Card ...");
        
    
}
