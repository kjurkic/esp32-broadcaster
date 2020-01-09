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

void esp_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    switch (event) {


    case ESP_GAP_BLE_SET_LOCAL_PRIVACY_COMPLETE_EVT :
    	ESP_ERROR_CHECK(esp_ble_gap_config_adv_data_raw(&eddystone_uid,eddystone_len ));
    	break;

    case  ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT :
    	ESP_ERROR_CHECK(esp_ble_gap_start_advertising(&parameters));
    	break;


    	case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:

			ESP_LOGI(ESP32_TAG,"CALLBACK ESP ADV START COMPLETE");
			break;

    	default:
			break;
	}
}


void eddystone(){



	ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

	esp_bt_controller_config_t config = BT_CONTROLLER_INIT_CONFIG_DEFAULT();


	ESP_ERROR_CHECK(esp_bt_controller_init(&config));

	ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_BLE));

	ESP_LOGI(ESP32_TAG, "BLUETOOTH CONTROLLER STATUS: %d", esp_bt_controller_get_status());


	ESP_ERROR_CHECK(esp_bluedroid_init());

	ESP_ERROR_CHECK(esp_bluedroid_enable());


	ESP_ERROR_CHECK(esp_ble_gap_register_callback(esp_cb));

	ESP_ERROR_CHECK(esp_ble_gap_config_local_privacy(true));


}


#endif /* MAIN_BLE_H_ */
