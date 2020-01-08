
#include "ble.h"


void app_main(void)
{

	ESP_ERROR_CHECK(nvs_flash_init());

	eddystone();


}

