#pragma once
#include "Arduino.h"
#include "nrf51.h"
#include "nrf51_bitfields.h"

#define MICROBIT_RADIO_STATUS_INITIALISED       0x0001

// Default configuration values
#define MICROBIT_RADIO_BASE_ADDRESS             0x75626974
#define MICROBIT_RADIO_DEFAULT_GROUP            1
#define MICROBIT_RADIO_DEFAULT_TX_POWER         6
#define MICROBIT_RADIO_MAX_PACKET_SIZE          32
#define MICROBIT_RADIO_HEADER_SIZE              4
#define MICROBIT_RADIO_MAXIMUM_RX_BUFFERS       4
#define MICROBIT_BLE_POWER_LEVELS               8

// Known Protocol Numbers
#define MICROBIT_RADIO_PROTOCOL_DATAGRAM        1       // A simple, single frame datagram. a little like UDP but with smaller packets. :-)
#define MICROBIT_RADIO_PROTOCOL_EVENTBUS        2       // Transparent propogation of events from one micro:bit to another.
#define MICROBIT_BLE_ENABLED                    0
// Events
#define MICROBIT_RADIO_EVT_DATAGRAM             1       // Event to signal that a new datagram has been received.

#define MICROBIT_OK								0x01
#define MICROBIT_NOT_SUPPORTED					0xff
#define MICROBIT_INVALID_PARAMETER				0xfe
#define	MICROBIT_NO_RESOURCES					0xfd

// Sets the default radio channel
#ifndef MICROBIT_RADIO_DEFAULT_FREQUENCY
#define MICROBIT_RADIO_DEFAULT_FREQUENCY 7
#endif

// Sets the minimum frequency band permissable for the device
#ifndef MICROBIT_RADIO_LOWER_FREQ_BAND
#define MICROBIT_RADIO_LOWER_FREQ_BAND 0
#endif

// Sets the maximum frequency band permissable for the device
#ifndef MICROBIT_RADIO_UPPER_FREQ_BAND
#define MICROBIT_RADIO_UPPER_FREQ_BAND 83
#endif
const int8_t MICROBIT_BLE_POWER_LEVEL[] = { -30, -20, -16, -12, -8, -4, 0, 4 };
extern "C" void RADIO_IRQHandler(void);

struct FrameBuffer
{
	uint8_t         length;                             // The length of the remaining bytes in the packet. includes protocol/version/group fields, excluding the length field itself.
	uint8_t         version;                            // Protocol version code.
	uint8_t         group;                              // ID of the group to which this packet belongs.
	uint8_t         protocol;                           // Inner protocol number c.f. those issued by IANA for IP protocols

	uint8_t         payload[MICROBIT_RADIO_MAX_PACKET_SIZE];    // User / higher layer protocol data
	FrameBuffer     *next;                              // Linkage, to allow this and other protocols to queue packets pending processing.
	int             rssi;                               // Received signal strength of this frame.
};



class NRF51_Radio
{
	uint8_t                 group;      // The radio group to which this micro:bit belongs.
	uint8_t                 queueDepth; // The number of packets in the receiver queue.
	int                     rssi;
	FrameBuffer             *rxQueue;   // A linear list of incoming packets, queued awaiting processing.
	FrameBuffer             *rxBuf;     // A pointer to the buffer being actively used by the RADIO hardware.

public:

	static NRF51_Radio    *instance;  // A singleton reference, used purely by the interrupt service routine.

	NRF51_Radio();
	

	void hello(String msg);
	
	/**
	* Sets the RSSI for the most recent packet.
	* The value is measured in -dbm. The higher the value, the stronger the signal.
	* Typical values are in the range -42 to -128.
	*
	* @param rssi the new rssi value.
	*
	* @note should only be called from RADIO_IRQHandler...
	*/
	int setRSSI(int rssi);


	/**
	 * Change the output power level of the transmitter to the given value.
	 *
	 * @param power a value in the range 0..7, where 0 is the lowest power and 7 is the highest.
	 *
	 * @return MICROBIT_OK on success, or MICROBIT_INVALID_PARAMETER if the value is out of range.
	 */
	int setTransmitPower(int power);

	/**
	  * Change the transmission and reception band of the radio to the given channel
	  *
	  * @param band a frequency band in the range 0 - 100. Each step is 1MHz wide, based at 2400MHz.
	  *
	  * @return MICROBIT_OK on success, or MICROBIT_INVALID_PARAMETER if the value is out of range,
	  *         or MICROBIT_NOT_SUPPORTED if the BLE stack is running.
	  */
	int setFrequencyBand(int band);

	/**
	  * Retrieve a pointer to the currently allocated receive buffer. This is the area of memory
	  * actively being used by the radio hardware to store incoming data.
	  *
	  * @return a pointer to the current receive buffer.
	  */
	FrameBuffer * getRxBuf();

	/**
	  * Attempt to queue a buffer received by the radio hardware, if sufficient space is available.
	  *
	  * @return MICROBIT_OK on success, or MICROBIT_NO_RESOURCES if a replacement receiver buffer
	  *         could not be allocated (either by policy or memory exhaustion).
	  */
	int queueRxBuf();



	/**
	  * Retrieves the current RSSI for the most recent packet.
	  * The return value is measured in -dbm. The higher the value, the stronger the signal.
	  * Typical values are in the range -42 to -128.
	  *
	  * @return the most recent RSSI value or MICROBIT_NOT_SUPPORTED if the BLE stack is running.
	  */
	int getRSSI();

	/**
	  * Initialises the radio for use as a multipoint sender/receiver
	  *
	  * @return MICROBIT_OK on success, MICROBIT_NOT_SUPPORTED if the BLE stack is running.
	  */
	int enable();

	/**
	  * Disables the radio for use as a multipoint sender/receiver.
	  *
	  * @return MICROBIT_OK on success, MICROBIT_NOT_SUPPORTED if the BLE stack is running.
	  */
	int disable();

	/**
	  * Sets the radio to listen to packets sent with the given group id.
	  *
	  * @param group The group to join. A micro:bit can only listen to one group ID at any time.
	  *
	  * @return MICROBIT_OK on success, or MICROBIT_NOT_SUPPORTED if the BLE stack is running.
	  */
	int setGroup(uint8_t group);

	
	/**
	  * Determines the number of packets ready to be processed.
	  *
	  * @return The number of packets in the receive buffer.
	  */
	int dataReady();

	/**
	  * Retrieves the next packet from the receive buffer.
	  * If a data packet is available, then it will be returned immediately to
	  * the caller. This call will also dequeue the buffer.
	  *
	  * @return The buffer containing the the packet. If no data is available, NULL is returned.
	  *
	  * @note Once recv() has been called, it is the callers responsibility to
	  *       delete the buffer when appropriate.
	  */
	FrameBuffer* recv();

	/**
	  * Transmits the given buffer onto the broadcast radio.
	  * The call will wait until the transmission of the packet has completed before returning.
	  *
	  * @param data The packet contents to transmit.
	  *
	  * @return MICROBIT_OK on success, or MICROBIT_NOT_SUPPORTED if the BLE stack is running.
	  */
	int send(FrameBuffer *buffer);



private:
	int status;
	
	
};

