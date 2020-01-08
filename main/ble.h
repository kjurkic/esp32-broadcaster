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
#include "esp_event_loop.h"
#include "esp_err.h"
#include "esp_log.h"

#include "esp_bt.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_gap_ble_api.h"
#include <string.h>

#include "eddystone.h"


static const char* ESP32_TAG = "BROADCASTER";

void eddystone(){



	ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT)){
		ESP_LOGI(ESP32_TAG, "CLASSIC BT CONTROLLER MEMORY RELEASED");
	}

	esp_bt_controller_config_t config = BT_CONTROLLER_INIT_CONFIG_DEFAULT();


	ESP_ERROR_CHECK(esp_bt_controller_init(&config)){
		ESP_LOGI(ESP32_TAG, "BT CONTROLLER INITIALIZED");
	}

	ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_BLE)){
		ESP_LOGI(ESP32_TAG, "BLE ONLY MODE ENABLED");
	}

	ESP_LOGI(ESP32_TAG, "BLUETOOTH CONTROLLER STATUS: %d", esp_bt_controller_get_status());


	ESP_ERROR_CHECK(esp_bluedroid_init()){
		ESP_LOGI(ESP32_TAG, "BLUETOOTH INITIALIZED");
	}

	ESP_ERROR_CHECK(esp_bluedroid_enable()){
			ESP_LOGI(ESP32_TAG, "BLUETOOTH ENABLED");

		}



	void esp_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
	{
	    switch (event) {

	    	case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:

				ESP_LOGI(ESP32_TAG,"CALLBACK ESP ADV START COMPLETE");
				break;

	    	default:
				break;
		}
	}


	ESP_ERROR_CHECK(esp_ble_gap_register_callback(esp_cb)){
		ESP_LOGI(ESP32_TAG, "REGISTER_CALLBACK");
	}

	ESP_ERROR_CHECK(esp_ble_gap_config_local_privacy(true)){
		ESP_LOGI(ESP32_TAG, "GAP PRIVACY ENABLED");


	}


	uint32_t eddystone_len = 31;


	Eddystone eddystone_uid = {.length_flags = 0x02,
							 .flags_data_t = 0x01,
							 .flags_data = 0x06,
							 .length_uuid = 0x03,
							 .uuid_list = 0x03,
							 .eddystone_uuid_1={0xAA, 0xFE},
							 .length_service_d = 0x17,
							 .service_data_t = 0x16,
							 .eddystone_uuid = {0xAA, 0xFE},


							 .frame_type = 0x00,
							 .ranging_data = 0xEB,
							 .namespace_e = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A},
							 .instance = {0x0B,0x0C,0xD,0x0E,0x0F,0x10},
							 .rfu = {0x00,0x00}
	};




	ESP_ERROR_CHECK(esp_ble_gap_config_adv_data_raw(&eddystone_uid,eddystone_len )){
		ESP_LOGI(ESP32_TAG, "ADV RAW DATA SET");

	}



	esp_ble_adv_params_t parameters = {.adv_int_min = 0x4000,
									   .adv_int_max = 0x4000,
									   .adv_type = 0x03,
									   .own_addr_type = 0x01,
									   .channel_map = 0x07
	};



	ESP_ERROR_CHECK(esp_ble_gap_start_advertising(&parameters)){
		ESP_LOGI(ESP32_TAG, "START ADVERTISING");
	}



}


#endif /* MAIN_BLE_H_ */
