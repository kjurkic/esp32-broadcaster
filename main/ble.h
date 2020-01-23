/*
 * ble.h
 *
 *  Created on: Jan 2, 2020
 *      Author: kristian
 */

#ifndef MAIN_BLE_H_
#define MAIN_BLE_H_

#include "freertos/FreeRTOS.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_gap_ble_api.h"
#include "esp_sleep.h"
#include "mbedtls/aes.h"
#include "eddystone.h"

static const char* ESP32_TAG = "BROADCASTER"; // tag for log messages

RTC_DATA_ATTR uint8_t sequence = 0; //sequence number saved in RTC so it can be used after wake up from deep sleep

/**
 * @brief						Encrypt namespace and instance of Eddystone-UID
 *
 * 								In this function AES-128 encryption is used.
 * 								First it converts the data from eddystone
 * 								data structure that is uint8_t to char and stores it
 * 								in an 16-byte array. Then it creates a aes context
 * 								structure, then initializes the aes context by passing
 * 								the address of context, then it sets the encryption key,
 * 								then it performs aes single-block encryption and then it
 * 								releases and clears the aes context. And last thing that
 * 								this function does is converting the data back to uint8_t.
 *
 * @param key:					128 bit key.
 * @param aes:					aes context.
 * @param eddystone_uidchar:	plain input array.
 * @param eddystone_uidchar_e:	encrypted output array.
 *
 */

void aes(){
	unsigned char key[16] = {'B', '?', 'D', '(', 'G', '+', 'K', 'b', 'P', 'e', 'S', 'h', 'V', 'm', 'Y', 'q'};
	unsigned char eddystone_uidchar[16];
	unsigned char eddystone_uidchar_e[16];

	for (int i = 0; i < 16; i++) {
		if (i > 9) {
			eddystone_uidchar[i] = (unsigned char) eddystone_uid.instance[i-10];
		} else {
			eddystone_uidchar[i] = (unsigned char) eddystone_uid.namespace_e[i];
		}
	}

	mbedtls_aes_context aes;
	mbedtls_aes_init(&aes);
	mbedtls_aes_setkey_enc(&aes, key, 128);
	mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_ENCRYPT,eddystone_uidchar,eddystone_uidchar_e);
	mbedtls_aes_free(&aes);

	for (int i = 0; i < 16; i++) {
		if (i > 9) {
			eddystone_uid.instance[i-10] = (uint8_t)eddystone_uidchar_e[i];
		} else {
			eddystone_uid.namespace_e[i] = (uint8_t)eddystone_uidchar_e[i];
		}
	}

}

/**
 * @brief			GAP callback function.
 *
 * 					When an gap event occurs this function is called.
 * 					After an event occurs it is processed.
 *
 * @param	event:	type of GAP event.
 * @param	param:  pointer to parameter of callback.
 *
 */

void esp_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
	esp_err_t err; // error type variable

    switch (event) {

    	/**
    	 * @brief					GAP event local privacy complete and
    	 * 							setting advertising data.
    	 *
    	 * 							After local privacy is set it sets sequence
    	 * 							number to instance[4] of eddystone structure.
    	 * 							Then it encrypts the data by calling the aes function,
    	 * 							after data is encrypted it calls
    	 * 							esp_ble_gap_config_adv_data_raw to set the data and
    	 * 							check if it was successful if not it prints a log error
    	 * 							with error code.
    	 *
    	 * @param  &eddystone_uid:	address of eddystone_uid structure.
    	 * @param  eddystone_len:	length of data structure.
    	 *
    	 * @return 					ESP_OK if advertising data successfully set.
    	 *
    	 */

    	case ESP_GAP_BLE_SET_LOCAL_PRIVACY_COMPLETE_EVT :
    		ESP_LOGI(ESP32_TAG,"Local privacy set complete.");
    		eddystone_uid.instance[4] = sequence;
    		aes();
    		err = esp_ble_gap_config_adv_data_raw(&eddystone_uid,eddystone_len);
    		if (err != ESP_OK){
    			ESP_LOGE(ESP32_TAG, "Failed to set advertising data ERROR: %s", esp_err_to_name(err));
    		}
    		break;

    	/**
    	 * @brief				GAP event advertising data set complete
    	 * 						and start advertising.
    	 *
    	 * 						After advertising data is set it increases
    	 * 						sequence number, and calls esp_gap_start_advertising
    	 * 						function to start with advertising. Then it checks if
    	 * 						advertising is started correctly if not, prints an error
    	 * 						log with error code.
    	 *
    	 * 	@param &parameters: address of esp_ble_adv_params_t structure.
    	 *
    	 * 	@return				ESP_OK if advertising started successfully.
    	 *
    	 */

    	case  ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT :
    		ESP_LOGI(ESP32_TAG,"Advertising data set complete.");
    		sequence++;
    		if (sequence == 255) {
    			sequence = 0;
    		}
    		err = esp_ble_gap_start_advertising(&parameters);
    		if (err != ESP_OK) {
    			ESP_LOGE(ESP32_TAG, "Failed to start advertising ERROR: %s", esp_err_to_name(err));
    		}
    		break;

    	/**
    	 * @brief		GAP event starting advertising complete.
    	 *
    	 * 				After advertisement started and sends an
    	 * 				packet then it calls esp_ble_gap_stop_advertising
    	 * 				function to stop advertising. Checks if stop advertising
    	 * 				was successful if not prints an log error with error code.
    	 *
    	 * 	@return		ESP_OK if advertising stopped successfully.
    	 *
    	 */

    	case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
    		ESP_LOGI(ESP32_TAG, "Advertising started.");
			err = esp_ble_gap_stop_advertising();
			if (err != ESP_OK) {
				ESP_LOGE(ESP32_TAG, "Failed to stop advertising ERROR: %s", esp_err_to_name(err));
			}
			break;

		/**
		 * @brief 		GAP event advertising stopped and entering
		 * 				deep sleep.
		 *
		 * 				After advertising stopped successfully it
		 * 				enters deep sleep mode for 10 seconds.
		 * 				Check if wake up time set correctly if not
		 * 				prints log error with error code.
		 *
		 * @param  us:	time before wake up in microseconds.
		 *
		 * @return 		ESP_OK if successfully set.
		 *
		 */

    	case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
    		ESP_LOGI(ESP32_TAG,"Advertising stopped.");
			err = esp_sleep_enable_timer_wakeup(10000000);
			if (err != ESP_OK) {
				ESP_LOGE(ESP32_TAG, "Failed to set wake up timer ERROR: %s", esp_err_to_name(err));
			}
			esp_deep_sleep_start();
			break;

    	default:
			break;

	}
}

/**
 * @brief	This function is called to process all needed steps
 * 			to start advertising BLE Eddystone-UID packets.
 *
 */

void eddystone(){

	esp_err_t err; // error type variable

	/**
	 * @brief 	Initialize Non-Volatile Storage partition (NVS).
	 *
	 * 			Checks if initialization was successful or not
	 * 			if not prints an error log with error code,
	 * 			if it was successful prints an information log.
	 *
	 * @return 	ESP_OK if NVS is successfully initialized.
	 *
	 */

	err = nvs_flash_init();
	if (err != ESP_OK) {
		ESP_LOGE(ESP32_TAG, "Failed to initialize NVS partition ERROR: %s", esp_err_to_name(err));
	} else {
		ESP_LOGI(ESP32_TAG, "Successfully initialized NVS partition.");
	}

	/**
	 * @brief 	Memory release for Classic BT.
	 *
	 * 			Check if memory for Classic BT mode was
	 * 			successfully released if not prints an error log
	 * 			with error code, if it was successful prints an
	 * 			information log.
	 *
	 * @return 	ESP_OK 	if memory is successfully released.
	 *
	 */

	err = esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);
	if (err != ESP_OK) {
		ESP_LOGE(ESP32_TAG, "Failed to release controller memory ERROR: %s", esp_err_to_name(err));
	} else {
		ESP_LOGI(ESP32_TAG, "Successfully released controller memory.");
	}

	/**
	 * @brief 	Controller configuration set with default parameters
	 *
	 */

	esp_bt_controller_config_t config = BT_CONTROLLER_INIT_CONFIG_DEFAULT();

	/**
	 * @brief 			Initialize BT controller.
	 *
	 * 					This function is called to initialize BT
	 * 					controller to allocate task and resources.
	 * 					Checks if BT controller was successfully initialized
	 * 					if not prints an error log with error code, if it
	 * 					was successful prints an information log.
	 *
	 * @param  config:  pointer to initial configuration parameters.
	 *
	 * @return 			ESP_OK if controller is successfully initialized.
	 *
	 */

	err = esp_bt_controller_init(&config);
	if (err != ESP_OK) {
		ESP_LOGE(ESP32_TAG, "Controller failed to initialize  ERROR: %s", esp_err_to_name(err));
	} else {
		ESP_LOGI(ESP32_TAG, "Controller successfully initialized.");
	}

	/**
	 * @brief					Enable BT controller for BLE mode.
	 *
	 * 							Enables BLE mode, and checks if it was successful
	 * 							if not prints an error log with error code, if it was
	 * 							successful prints an information log.
	 *
	 * @param  ESP_BT_MODE_BLE:	value of ESP_BT_MODE_BLE is 1 and defines BLE mode.
	 *
	 * @return 					ESP_OK if BT controller is successfully enabled.
	 *
	 */

	err = esp_bt_controller_enable(ESP_BT_MODE_BLE);
	if (err != ESP_OK) {
		ESP_LOGE(ESP32_TAG, "Failed to enable BLE mode ERROR: %s", esp_err_to_name(err));
	} else {
		ESP_LOGI(ESP32_TAG, "Successfully enabled BLE mode.");
	}

	/**
	 * @brief	Initialize and allocate resource for BT.
	 *
	 * 			Checks if initialization and allocation was
	 * 			successful if not prints an error log with
	 * 			error code, if it was successful prints an
	 * 			information log.
	 *
	 * @return	ESP_OK if successfully initialized and allocated.
	 *
	 */

	err = esp_bluedroid_init();
	if (err != ESP_OK) {
		ESP_LOGE(ESP32_TAG, "Failed to initialize and allocate resource for BT ERROR: %s", esp_err_to_name(err));
	} else {
		ESP_LOGI(ESP32_TAG, "Successfully initialized and allocated resource for BT.");
	}

	/**
	 * @brief 	Enable BT.
	 *
	 * 			Checks if BT is enabled successfully if not
	 * 			prints an error log with error code, if it was
	 * 			successful prints an information log.
	 *
	 * @return 	ESP_OK if BT successfully enabled.
	 *
	 */

	err = esp_bluedroid_enable();
	if (err != ESP_OK) {
		ESP_LOGE(ESP32_TAG, "Failed to enable BT ERROR: %s", esp_err_to_name(err));
	} else {
		ESP_LOGI(ESP32_TAG, "Successfully enabled BT.");
	}

	/**
	 * @brief 			Register callback function.
	 *
	 * 					This function is called to register the callback
	 * 					function that will occur gap event.
	 * 					Checks if successfully set if not prints an error
	 * 					log with error code, if it was successful prints
	 * 					an information log.
	 *
	 * @param   esp_cb:	callback function.
	 *
	 * @return 			ESP_OK if callback function is registered.
	 *
	 */

	err = esp_ble_gap_register_callback(esp_cb);
	if (err != ESP_OK) {
		ESP_LOGE(ESP32_TAG, "Failed to set callback function that occurs GAP event ERROR: %s", esp_err_to_name(err));
	} else {
		ESP_LOGI(ESP32_TAG, "Callback function that occurs GAP event set successfully.");
	}

	/**
	 * @brief		Enable local privacy.
	 *
	 * 				Local privacy needs to be enabled to achieve
	 * 				broadcasting using a resolvable address.
	 * 				Broadcasting address is updated every 15 minutes.
	 * 				Checks if successfully set if not prints an error log
	 * 				with error code.
	 *
	 * @param true:	enable local privacy.
	 *
	 * @return		ESP_OK if local privacy successfully enabled.
	 *
	 */

	err = esp_ble_gap_config_local_privacy(true);
	if (err != ESP_OK) {
		ESP_LOGE(ESP32_TAG, "Failed to set local privacy ERROR: %s", esp_err_to_name(err));
	}
}


#endif /* MAIN_BLE_H_ */
