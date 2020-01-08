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




#endif /* MAIN_EDDYSTONE_H_ */
