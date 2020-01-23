/*
 * eddystone.h
 *
 *  Created on: Jan 7, 2020
 *      Author: kristian
 */

#ifndef MAIN_EDDYSTONE_H_
#define MAIN_EDDYSTONE_H_


/**
 *@brief	Eddystone structure groups the core Bluetooth
 *			frame type and Eddystone-UID frame type.
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

uint32_t eddystone_len = 31; //length of Eddystone-UID packet

/**
 * @brief	Setting all needed fields for a packet
 * 			to be in Eddystone-UID frame type.
 *
 * 			Length of field flags 2-bytes,
 * 			Value 0x01 defines that it is a flag due to Bluetooth Core Specification,
 * 			Value 0x06 defines that LE General Discoverable Mode is enabled and BR/EDR is not suported,
 *
 * 			Length of field UUID 3-bytes,
 * 			Value 0x03 defines that it goes for Complete List of 16-bit Service Class UUIDs,
 * 			Value 0xFEAA defines Google Eddystone UUID,
 *
 * 			Length of field service data is 23-bytes,
 * 			Value 0x16 defines service data data type value,
 * 			Eddystone UUID 0xFEAA,
 * 			Value 0x00 defines that this is a UID frame type,
 * 			Value 0xEB defines Tx power at 0m to -21dBm,
 * 			Namespace in this case if we convert ASCII to characters:
 * 			F E R I D S P L A B
 * 			Instance in this case if we convert ASCII to characters:
 * 			K J 9 5 0 *
 * 			Rfu must be 0x0000 and is reserved for future use.
 *
 */
Eddystone eddystone_uid = {.length_flags = 0x02,
		.flags_data_t = 0x01,
		.flags_data = 0x06,

		.length_uuid = 0x03,
		.uuid_list = 0x03,
		.eddystone_uuid_1 = {0xAA, 0xFE},

		.length_service_d = 0x17,
		.service_data_t = 0x16,
		.eddystone_uuid = {0xAA, 0xFE},
		.frame_type = 0x00,
		.ranging_data = 0xEB,
		.namespace_e = {0x46,0x45,0x52,0x49,0x44,0x53,0x50,0x4C,0x41,0x42},
		.instance = {0x4B,0x4A,0x39,0x35,0x00,0x2A},
		.rfu = {0x00,0x00}
};

/**
 * @brief	Setting advertisement parameters
 *
 * 			Advertising interval (min and max) 20ms,
 * 			Advertising type non-connectable,
 * 			Address type random,
 * 			Advertising on all channels.
 *
 */
esp_ble_adv_params_t parameters = {.adv_int_min = 0x0020,
		.adv_int_max = 0x0020,
		.adv_type = 0x03,
		.own_addr_type = 0x01,
		.channel_map = 0x07
};


#endif /* MAIN_EDDYSTONE_H_ */
