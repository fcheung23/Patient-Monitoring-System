/*
 * data_packet.h
 *
 *  Created on: Oct 8, 2025
 *      Author: fcheung
 */

#ifndef INC_DATA_PACKET_H_
#define INC_DATA_PACKET_H_

// sDataSource type for recognizing sensor source
typedef enum
{
	temperature_sensor,
	heartbeat_sensor,
	accelerometer_sensor,
} DataSource_t;

// Sensor specific data for SensorPayload_t
typedef struct
{
	uint32_t tempValue;
} TempData_t;

typedef struct
{
	uint32_t bpmValue;
} HeartData_t;

typedef struct
{
	float vectorMag;
} AccelData_t;

// SensorPayload_t type for holding different sensor specific data
typedef union
{
	TempData_t temp;
	HeartData_t heart;
	AccelData_t accel;
} SensorPayload_t;

// Data_t type for holding entire data packet (sent in queues)
// EX: temperature = rxData.payload.temp.tempValue
typedef struct
{
	SensorPayload_t payload;
	DataSource_t sDataSource;
} Data_t;

#endif /* INC_DATA_PACKET_H_ */
