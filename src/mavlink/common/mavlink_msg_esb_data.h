// MESSAGE ESB_DATA PACKING

#define MAVLINK_MSG_ID_ESB_DATA 151

typedef struct __mavlink_esb_data_t
{
 uint16_t ESB_Data[7]; ///< This array will contain the data of the Environmental Sensor Bus. X to be replaced with the number of sensors attached to the current system. Each sensor will be assigned an ID starting with 0. Sensor data can be accessed by accessing the array. ESB_Data[X][0] will contain the sensor ID and ESB_DATA[X][1] Will contain the sensor value 
} mavlink_esb_data_t;

#define MAVLINK_MSG_ID_ESB_DATA_LEN 14
#define MAVLINK_MSG_ID_151_LEN 14

#define MAVLINK_MSG_ID_ESB_DATA_CRC 198
#define MAVLINK_MSG_ID_151_CRC 198

#define MAVLINK_MSG_ESB_DATA_FIELD_ESB_DATA_LEN 7

#define MAVLINK_MESSAGE_INFO_ESB_DATA { \
	"ESB_DATA", \
	1, \
	{  { "ESB_Data", NULL, MAVLINK_TYPE_UINT16_T, 7, 0, offsetof(mavlink_esb_data_t, ESB_Data) }, \
         } \
}


/**
 * @brief Pack a esb_data message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param ESB_Data This array will contain the data of the Environmental Sensor Bus. X to be replaced with the number of sensors attached to the current system. Each sensor will be assigned an ID starting with 0. Sensor data can be accessed by accessing the array. ESB_Data[X][0] will contain the sensor ID and ESB_DATA[X][1] Will contain the sensor value 
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_esb_data_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
						       const uint16_t *ESB_Data)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_ESB_DATA_LEN];

	_mav_put_uint16_t_array(buf, 0, ESB_Data, 7);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_ESB_DATA_LEN);
#else
	mavlink_esb_data_t packet;

	mav_array_memcpy(packet.ESB_Data, ESB_Data, sizeof(uint16_t)*7);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_ESB_DATA_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_ESB_DATA;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_ESB_DATA_LEN, MAVLINK_MSG_ID_ESB_DATA_CRC);
#else
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_ESB_DATA_LEN);
#endif
}

/**
 * @brief Pack a esb_data message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param ESB_Data This array will contain the data of the Environmental Sensor Bus. X to be replaced with the number of sensors attached to the current system. Each sensor will be assigned an ID starting with 0. Sensor data can be accessed by accessing the array. ESB_Data[X][0] will contain the sensor ID and ESB_DATA[X][1] Will contain the sensor value 
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_esb_data_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
							   mavlink_message_t* msg,
						           const uint16_t *ESB_Data)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_ESB_DATA_LEN];

	_mav_put_uint16_t_array(buf, 0, ESB_Data, 7);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_ESB_DATA_LEN);
#else
	mavlink_esb_data_t packet;

	mav_array_memcpy(packet.ESB_Data, ESB_Data, sizeof(uint16_t)*7);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_ESB_DATA_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_ESB_DATA;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_ESB_DATA_LEN, MAVLINK_MSG_ID_ESB_DATA_CRC);
#else
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_ESB_DATA_LEN);
#endif
}

/**
 * @brief Encode a esb_data struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param esb_data C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_esb_data_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_esb_data_t* esb_data)
{
	return mavlink_msg_esb_data_pack(system_id, component_id, msg, esb_data->ESB_Data);
}

/**
 * @brief Encode a esb_data struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param esb_data C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_esb_data_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_esb_data_t* esb_data)
{
	return mavlink_msg_esb_data_pack_chan(system_id, component_id, chan, msg, esb_data->ESB_Data);
}

/**
 * @brief Send a esb_data message
 * @param chan MAVLink channel to send the message
 *
 * @param ESB_Data This array will contain the data of the Environmental Sensor Bus. X to be replaced with the number of sensors attached to the current system. Each sensor will be assigned an ID starting with 0. Sensor data can be accessed by accessing the array. ESB_Data[X][0] will contain the sensor ID and ESB_DATA[X][1] Will contain the sensor value 
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_esb_data_send(mavlink_channel_t chan, const uint16_t *ESB_Data)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_ESB_DATA_LEN];

	_mav_put_uint16_t_array(buf, 0, ESB_Data, 7);
#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ESB_DATA, buf, MAVLINK_MSG_ID_ESB_DATA_LEN, MAVLINK_MSG_ID_ESB_DATA_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ESB_DATA, buf, MAVLINK_MSG_ID_ESB_DATA_LEN);
#endif
#else
	mavlink_esb_data_t packet;

	mav_array_memcpy(packet.ESB_Data, ESB_Data, sizeof(uint16_t)*7);
#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ESB_DATA, (const char *)&packet, MAVLINK_MSG_ID_ESB_DATA_LEN, MAVLINK_MSG_ID_ESB_DATA_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ESB_DATA, (const char *)&packet, MAVLINK_MSG_ID_ESB_DATA_LEN);
#endif
#endif
}

#if MAVLINK_MSG_ID_ESB_DATA_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_esb_data_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  const uint16_t *ESB_Data)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char *buf = (char *)msgbuf;

	_mav_put_uint16_t_array(buf, 0, ESB_Data, 7);
#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ESB_DATA, buf, MAVLINK_MSG_ID_ESB_DATA_LEN, MAVLINK_MSG_ID_ESB_DATA_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ESB_DATA, buf, MAVLINK_MSG_ID_ESB_DATA_LEN);
#endif
#else
	mavlink_esb_data_t *packet = (mavlink_esb_data_t *)msgbuf;

	mav_array_memcpy(packet->ESB_Data, ESB_Data, sizeof(uint16_t)*7);
#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ESB_DATA, (const char *)packet, MAVLINK_MSG_ID_ESB_DATA_LEN, MAVLINK_MSG_ID_ESB_DATA_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ESB_DATA, (const char *)packet, MAVLINK_MSG_ID_ESB_DATA_LEN);
#endif
#endif
}
#endif

#endif

// MESSAGE ESB_DATA UNPACKING


/**
 * @brief Get field ESB_Data from esb_data message
 *
 * @return This array will contain the data of the Environmental Sensor Bus. X to be replaced with the number of sensors attached to the current system. Each sensor will be assigned an ID starting with 0. Sensor data can be accessed by accessing the array. ESB_Data[X][0] will contain the sensor ID and ESB_DATA[X][1] Will contain the sensor value 
 */
static inline uint16_t mavlink_msg_esb_data_get_ESB_Data(const mavlink_message_t* msg, uint16_t *ESB_Data)
{
	return _MAV_RETURN_uint16_t_array(msg, ESB_Data, 7,  0);
}

/**
 * @brief Decode a esb_data message into a struct
 *
 * @param msg The message to decode
 * @param esb_data C-struct to decode the message contents into
 */
static inline void mavlink_msg_esb_data_decode(const mavlink_message_t* msg, mavlink_esb_data_t* esb_data)
{
#if MAVLINK_NEED_BYTE_SWAP
	mavlink_msg_esb_data_get_ESB_Data(msg, esb_data->ESB_Data);
#else
	memcpy(esb_data, _MAV_PAYLOAD(msg), MAVLINK_MSG_ID_ESB_DATA_LEN);
#endif
}
