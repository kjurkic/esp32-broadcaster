/*
 * eddystone.h
 *
 *  Created on: Jan 7, 2020
 *      Author: kristian
 */

#ifndef MAIN_EDDYSTONE_H_
#define MAIN_EDDYSTONE_H_


/*
 * Header file groups the core Bluetooth data type and
 * Eddystone-UID frame type in a structure
 *
 */

typedef struct {
	uint8_t length_flags;
	uint8_t flags_data_t;
	uint8_t flags_data;
	uint8_t length_uuid;
	uint8_t uuid_list;
	uint8_t eddystone_uuid_1[2];
	uint8_t length_service_d;
	uint8_t service_data_t;

	uint8_t eddystone_uuid[2];


	uint8_t frame_type;
	uint8_t ranging_data;
	uint8_t namespace_e[10];
	uint8_t instance[6];
	uint8_t rfu[2];

}__attribute__((packed))Eddystone;

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








	esp_ble_adv_params_t parameters = {.adv_int_min = 0x4000,
									   .adv_int_max = 0x4000,
									   .adv_type = 0x03,
									   .own_addr_type = 0x01,
									   .channel_map = 0x07
	};




#endif /* MAIN_EDDYSTONE_H_ */
