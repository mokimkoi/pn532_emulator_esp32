/**
 * @file     pn532.h
 * @author   D. Braun
 * @license  MIT (see license.txt)
 * This is a PN532 Driver for the ESP32 family and IDF 5.3 for NXP's PN532 NFC/13.56MHz RFID Transceiver.
 * This component is inspired the Adafruit library.
 */

#ifndef PN532_H
#define PN532_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "driver/gpio.h"
#include "pn532_driver.h"


#ifdef __cplusplus
extern "C"
{
#endif

#define PN532_PREAMBLE                      (0x00)
#define PN532_STARTCODE1                    (0x00)
#define PN532_STARTCODE2                    (0xFF)
#define PN532_POSTAMBLE                     (0x00)

#define PN532_HOSTTOPN532                   (0xD4)
#define PN532_PN532TOHOST                   (0xD5)

// PN532 Commands
#define PN532_COMMAND_DIAGNOSE              (0x00)
#define PN532_COMMAND_GETFIRMWAREVERSION    (0x02)
#define PN532_COMMAND_GETGENERALSTATUS      (0x04)
#define PN532_COMMAND_READREGISTER          (0x06)
#define PN532_COMMAND_WRITEREGISTER         (0x08)
#define PN532_COMMAND_READGPIO              (0x0C)
#define PN532_COMMAND_WRITEGPIO             (0x0E)
#define PN532_COMMAND_SETSERIALBAUDRATE     (0x10)
#define PN532_COMMAND_SETPARAMETERS         (0x12)
#define PN532_COMMAND_SAMCONFIGURATION      (0x14)
#define PN532_COMMAND_POWERDOWN             (0x16)
#define PN532_COMMAND_RFCONFIGURATION       (0x32)
#define PN532_COMMAND_RFREGULATIONTEST      (0x58)
#define PN532_COMMAND_INJUMPFORDEP          (0x56)
#define PN532_COMMAND_INJUMPFORPSL          (0x46)
#define PN532_COMMAND_INLISTPASSIVETARGET   (0x4A)
#define PN532_COMMAND_INATR                 (0x50)
#define PN532_COMMAND_INPSL                 (0x4E)
#define PN532_COMMAND_INDATAEXCHANGE        (0x40)
#define PN532_COMMAND_INCOMMUNICATETHRU     (0x42)
#define PN532_COMMAND_INDESELECT            (0x44)
#define PN532_COMMAND_INRELEASE             (0x52)
#define PN532_COMMAND_INSELECT              (0x54)
#define PN532_COMMAND_INAUTOPOLL            (0x60)
#define PN532_COMMAND_TGINITASTARGET        (0x8C)
#define PN532_COMMAND_TGSETGENERALBYTES     (0x92)
#define PN532_COMMAND_TGGETDATA             (0x86)
#define PN532_COMMAND_TGSETDATA             (0x8E)
#define PN532_COMMAND_TGSETMETADATA         (0x94)
#define PN532_COMMAND_TGGETINITIATORCOMMAND (0x88)
#define PN532_COMMAND_TGRESPONSETOINITIATOR (0x90)
#define PN532_COMMAND_TGGETTARGETSTATUS     (0x8A)

#define PN532_RESPONSE_INDATAEXCHANGE       (0x41)
#define PN532_RESPONSE_INLISTPASSIVETARGET  (0x4B)

#define PN532_SPI_STATREAD                  (0x02)
#define PN532_SPI_DATAWRITE                 (0x01)
#define PN532_SPI_DATAREAD                  (0x03)
#define PN532_SPI_READY                     (0x01)

#define PN532_I2C_RAW_ADDRESS               (0x24)
#define PN532_I2C_ADDRESS                   (0x48)
#define PN532_I2C_READ_ADDRESS				(0x49)
#define PN532_I2C_READBIT                   (0x01)
#define PN532_I2C_BUSY                      (0x00)
#define PN532_I2C_READY                     (0x01)
#define PN532_I2C_READYTIMEOUT              (20)

#define PN532_BRTY_ISO14443A_106KBPS        (0x00)
#define PN532_BRTY_FELICA_212KBPS           (0x01)
#define PN532_BRTY_FELICA_424KBPS           (0x02)
#define PN532_BRTY_ISO14443B_106KBPS        (0x03)
#define PN532_BRTY_JEWEL_TAG_106KBPS        (0x04)

// Mifare Commands
#define MIFARE_CMD_AUTH_A                   (0x60)
#define MIFARE_CMD_AUTH_B                   (0x61)
#define MIFARE_CMD_READ                     (0x30)
#define MIFARE_CMD_WRITE                    (0xA0)
#define MIFARE_CMD_TRANSFER                 (0xB0)
#define MIFARE_CMD_DECREMENT                (0xC0)
#define MIFARE_CMD_INCREMENT                (0xC1)
#define MIFARE_CMD_STORE                    (0xC2)
#define MIFARE_ULTRALIGHT_CMD_WRITE         (0xA2)

// Prefixes for NDEF Records (to identify record type)
#define NDEF_URIPREFIX_NONE                 (0x00)
#define NDEF_URIPREFIX_HTTP_WWWDOT          (0x01)
#define NDEF_URIPREFIX_HTTPS_WWWDOT         (0x02)
#define NDEF_URIPREFIX_HTTP                 (0x03)
#define NDEF_URIPREFIX_HTTPS                (0x04)
#define NDEF_URIPREFIX_TEL                  (0x05)
#define NDEF_URIPREFIX_MAILTO               (0x06)
#define NDEF_URIPREFIX_FTP_ANONAT           (0x07)
#define NDEF_URIPREFIX_FTP_FTPDOT           (0x08)
#define NDEF_URIPREFIX_FTPS                 (0x09)
#define NDEF_URIPREFIX_SFTP                 (0x0A)
#define NDEF_URIPREFIX_SMB                  (0x0B)
#define NDEF_URIPREFIX_NFS                  (0x0C)
#define NDEF_URIPREFIX_FTP                  (0x0D)
#define NDEF_URIPREFIX_DAV                  (0x0E)
#define NDEF_URIPREFIX_NEWS                 (0x0F)
#define NDEF_URIPREFIX_TELNET               (0x10)
#define NDEF_URIPREFIX_IMAP                 (0x11)
#define NDEF_URIPREFIX_RTSP                 (0x12)
#define NDEF_URIPREFIX_URN                  (0x13)
#define NDEF_URIPREFIX_POP                  (0x14)
#define NDEF_URIPREFIX_SIP                  (0x15)
#define NDEF_URIPREFIX_SIPS                 (0x16)
#define NDEF_URIPREFIX_TFTP                 (0x17)
#define NDEF_URIPREFIX_BTSPP                (0x18)
#define NDEF_URIPREFIX_BTL2CAP              (0x19)
#define NDEF_URIPREFIX_BTGOEP               (0x1A)
#define NDEF_URIPREFIX_TCPOBEX              (0x1B)
#define NDEF_URIPREFIX_IRDAOBEX             (0x1C)
#define NDEF_URIPREFIX_FILE                 (0x1D)
#define NDEF_URIPREFIX_URN_EPC_ID           (0x1E)
#define NDEF_URIPREFIX_URN_EPC_TAG          (0x1F)
#define NDEF_URIPREFIX_URN_EPC_PAT          (0x20)
#define NDEF_URIPREFIX_URN_EPC_RAW          (0x21)
#define NDEF_URIPREFIX_URN_EPC              (0x22)
#define NDEF_URIPREFIX_URN_NFC              (0x23)

// Wakeup sources for PN532 powerdown mode
#define WU_SOURCE_INT0                  (0x01)
#define WU_SOURCE_INT1                  (0x02)
#define WU_SOURCE_RF                    (0x08)
#define WU_SOURCE_HSU                   (0x10)
#define WU_SOURCE_SPI                   (0x20)
#define WU_SOURCE_GPIO                  (0x40)
#define WU_SOURCE_I2C                   (0x80)

//command RF Configuration items
#define RFCONFIG_RFFIELD                    (0x01)
#define RFCONFIG_VARIOUS_TIMINGS            (0x02)
#define RFCONFIG_MAXRTYCOM                  (0x04)
#define RFCONFIG_MAXRETRIES                 (0x05)
#define RFCONFIG_ANALOG_BRT_A106K           (0x0A)
#define RFCONFIG_ANALOG_BRT_212K_424K       (0x0B)
#define RFCONFIG_ANALOG_TYPE_B              (0x0C)
#define RFCONFIG_ANALOG_BRT_212_424_848K    (0x0D)

//tag type definitions
#define TYPEA_106K                 (0x00)
#define TYPEB_106K                 (0x01)
#define FELICA_212K                (0x02)
#define FELICA_424K                (0x03)
#define JEWEL_106K                 (0x04)
//baud rate and type definitions
#define PN532_BRTY_ISO14443A_106KBPS        (0x00)
#define PN532_BRTY_FELICA_212KBPS           (0x01)
#define PN532_BRTY_FELICA_424KBPS           (0x02)
#define PN532_BRTY_ISO14443B_106KBPS        (0x03)
#define PN532_BRTY_JEWEL_TAG_106KBPS        (0x04)

typedef enum {
    MIFARE_UNKNOWN ,
    MIFARE_CLASSIC_1K,
    MIFARE_CLASSIC_4K,
    MIFARE_CLASSIC_MINI,
    MIFARE_CLASSIC_EV1,
    MIFARE_ULTRALIGHT,
    MIFARE_ULTRALIGHT_C,
    MIFARE_PLUS_SX_SL1,
    MIFARE_PLUS_SX_SL2_SL3,
    MIFARE_PLUS_EV1,
    MIFARE_DESFIRE,
    SMARTMX,
    MIFARE_PROX,
} MIFARE_MODEL;

typedef struct TagInfo TagInfo;
typedef struct TagInfo {
    uint8_t  tg_number;     // Target number
    uint8_t  uid[10];        // UID of the tag
    uint8_t  uidLength;      // Length of the UID (4, 7 or 10 bytes)
    uint8_t  sak;            // SAK byte returned by the tag
    uint16_t  atqa;        // ATQA bytes returned by the tag
    uint8_t  type; 
    uint8_t  ATS[20];        // ATS bytes for ISO14443B/Felica tags
    uint8_t  ATSLength;      // Length of ATS
    uint8_t  getversionflag;  // Type of the tag (e.g., MIFARE Classic, NTAG2xx, etc.)
} TagInfo;

typedef enum {
    NTAG2XX_UNKNOWN,
    NTAG2XX_NTAG213,
    NTAG2XX_NTAG215,
    NTAG2XX_NTAG216
} NTAG2XX_MODEL;

// Generic PN532 functions

/**
 * Get firmware version of the PN5xx chip.
 * @param io_handle PN532 io handle
 * @param fw_version variable to store firmware version
 * @return ESP_OK if successful
 */
esp_err_t pn532_get_firmware_version(pn532_io_handle_t io_handle, uint32_t *fw_version);


/**
 * Get general status of the PN5xx chip.
 * @param io_handle PN532 io handle
 * @param status variable to store status
 * @return ESP_OK if successful
 */
esp_err_t pn532_get_general_status(pn532_io_handle_t io_handle, uint8_t *status);
/**
 * Set PN532 parameters.
 * @param io_handle PN532 io handle
 * @param mode parameter mode
 * @return ESP_OK if successful
 */
esp_err_t pn532_setparameters(pn532_io_handle_t io_handle, uint8_t mode);
/**
 * Power down the PN5xx chip.
 * @param io_handle PN532 io handle
 * @param wakeup_enable enable wakeup sources (WU_SOURCE_xxx)
 * @param irq_enable enable IRQ on wakeup
 * @return ESP_OK if successful
 */
esp_err_t pn532_powerdown(pn532_io_handle_t io_handle, uint8_t wakeup_enable, uint8_t irq_enable);

/**
 * Configure the RF settings of the PN5xx chip.
 * @param io_handle PN532 io handle
 * @param config_item configuration item
 * @param config_data pointer to configuration data
 * @param config_data_len length of configuration data
 * @return ESP_OK if successful
 */
esp_err_t pn532_RFconfiguration(pn532_io_handle_t io_handle, uint8_t config_item, const uint8_t *config_data, uint8_t config_data_len) ;


/**
 * InJumpForDep command.
 * @param io_handle PN532 io handle
 * @param actpass activation or passive
 * @param baud baud rate
 * @param next next command
 * @param data data to send
 * @param data_len length of data
 * @return ESP_OK if successful
 */
esp_err_t pn532_injumpfordep(pn532_io_handle_t io_handle,uint8_t actpass ,uint8_t baud ,uint8_t next ,const uint8_t *data ,uint8_t data_len);
/**
 * InListPassiveTarget command.
 * @param io_handle PN532 io handle
 * @param MaxTg maximum number of targets to list
 * @param BrTy baud rate and type, use PN532_BRTY_xxx defines.
 * @param intia_data pointer to initiator data
 * @param intia_data_len length of initiator data
 * @return ESP_OK if successful
*/
esp_err_t pn532_inlistpassivetarget(pn532_io_handle_t io_handle,uint8_t MaxTg ,uint8_t BrTy,uint8_t *intia_data ,uint8_t intia_data_len) ;
/**
 * InListPassiveTarget command scan version.
 * @param io_handle PN532 io handle
 * @param Tgtype target type, use TYPEA_106K, TYPEB_106K, FELICA_212K, FELICA_424K, JEWEL_106K
 * @param MaxTg maximum number of targets to list
 * @param BrTy baud rate and type, use PN532_BRTY_xxx defines.
 * @return ESP_OK if successful
*/
esp_err_t pn532_inlistpassivetargetscan(pn532_io_handle_t io_handle,uint8_t Tgtype, uint8_t MaxTg ,uint8_t BrTy,TagInfo *tag_info1,TagInfo *tag_info2);
/**
 * Set MxRtyPassiveActivate value.
 * MxRtyPassiveActivate sets the number of times that
 * the PN532 will retry to activate a target in InListPassiveTarget command.
 * @param io_handle PN532 io handle
 * @param maxRetries 0x00 - only one try, 0xFF - try for ever, 0x01..0xFE - number of retries
 * @return ESP_OK if successful
 * 
 */
esp_err_t pn532_set_passive_activation_retries(pn532_io_handle_t io_handle, uint8_t maxRetries);

// ISO14443A functions

/**
 * Wait for an ISO14443A card and get UID of card.
 * @param io_handle PN532 io handle
 * @param baud_rate_and_card_type baud rate and type, use PN532_BRTY_xxx defines.
 * @param uid buffer to receive the UID bytes (UIDs can be 4 bytes, 7 bytes or 10 bytes long)
 * @param uid_length length of the received UID
 * @param timeout timeout in milliseconds. If 0, wait forever
 * @return ESP_OK if successful
 */
esp_err_t pn532_read_passive_target_id(pn532_io_handle_t io_handle,
                                       uint8_t baud_rate_and_card_type,
                                       uint8_t *uid,
                                       uint8_t *uid_length,
                                       int32_t timeout);

/**
 * Exchange an APDU with the currently inListed target
 * @param io_handle PN532 io handle
 * @param send_buffer APDU data to send
 * @param send_buffer_length length of APDU data
 * @param response buffer for response data
 * @param response_length [inout] length of received response
 * @return ESP_OK if successful
 */
esp_err_t pn532_in_data_exchange(pn532_io_handle_t io_handle, const uint8_t *send_buffer, uint8_t send_buffer_length, uint8_t *response,
                                 uint8_t *response_length);

/**
 * InLists a passive target.
 * PN532 acting as reader/initiator, peer acting as card/responder.
 * @param io_handle PN532 io handle
 * @return ESP_OK if successful
 */
esp_err_t pn532_in_list_passive_target(pn532_io_handle_t io_handle);


// NTAG2xx functions

/**
 * Identify NTAG card size by storage size field in GET_VERSION response.
 * @param io_handle PN532 io handle
 * @param model the model detected
 * @return ESP_OK if successful
 */
esp_err_t ntag2xx_get_model(pn532_io_handle_t io_handle, NTAG2XX_MODEL *model);

/**
 * Authenticate a page.
 * @param io_handle PN532 io handle
 * @param page page to authenticate
 * @param key buffer containing the 6 byte key for authentication
 * @param uid buffer containing the UID bytes
 * @param uid_length length of the UID
 * @return ESP_OK if successful
 */
esp_err_t ntag2xx_authenticate(pn532_io_handle_t io_handle, uint8_t page, uint8_t *key, uint8_t *uid, uint8_t uid_length);

/**
 * Read a 4 byte page.
 * @param io_handle PN532 io handle
 * @param page page to read
 * @param buffer buffer to receive data
 * @return ESP_OK if successful
 */
esp_err_t ntag2xx_read_page(pn532_io_handle_t io_handle, uint8_t page, uint8_t *buffer, size_t read_len);

/**
 * Write a 4 byte page.
 * @param io_handle PN532 io handle
 * @param page page to write
 * @param data pointer to data to write
 * @return ESP_OK if successful
 */
esp_err_t ntag2xx_write_page(pn532_io_handle_t io_handle, uint8_t page, const uint8_t *data);

/**
 * Check the error frame of the PN532.
 * @return ESP_OK if no error
 */
esp_err_t error_frame_check();
/**
 * Decode the MIFARE type from SAK and ATQA and if getversion is needed.
 * @param sak SAK value
 * @param ATQA ATQA value
 * @param mifare_type pointer to store the decoded MIFARE type
 * @param versionflag pointer to store the version flag
 * @return ESP_OK if successful
 */
esp_err_t mifare_type_decode(TagInfo *Tag) ;

/**
 * Send a command to PN532 and receive the response.
 * @param io_handle PN532 io handle
 * @param commandlen length of command to send
 * @param replylen expected length of reply
 * @param timeout timeout in milliseconds
 * @param function_name function name for debug output
 * @param debug_flag enable debug output if 1
 * @return ESP_OK if successful     
*/
esp_err_t pn532_sendrecv_command(pn532_io_handle_t io_handle, uint8_t commandlen,uint8_t replylen,uint16_t timeout,char *function_name,uint8_t debug_flag);
esp_err_t pn532_indataexchange(pn532_io_handle_t io_handle,const uint8_t *send_buffer,uint8_t send_buffer_length,uint8_t *response,uint8_t *response_length) ;

esp_err_t show_tag_info(TagInfo *Tag) ;
esp_err_t typeA_response_parse(TagInfo *tag_info1,TagInfo *tag_info2) ;   
esp_err_t init_taginfo(TagInfo *Tag) ;

esp_err_t mfc_authenticate_block(pn532_io_handle_t io_handle,TagInfo *tag_info,uint8_t block_number, uint8_t key_type, const uint8_t *inkey) ;
esp_err_t mfc_read_block(pn532_io_handle_t io_handle, uint8_t tg_number, uint8_t block_number, uint8_t *data) ;
esp_err_t mfc_write_block(pn532_io_handle_t io_handle, uint8_t tg_number, uint8_t block_number, const uint8_t *data) ;
esp_err_t mfc_dump_sector_1K(pn532_io_handle_t io_handle, TagInfo *tag_info, uint8_t sector_number, uint8_t **dump_buffer,uint8_t key_type,uint8_t *inkey) ;
#ifdef __cplusplus
}
#endif

#endif
