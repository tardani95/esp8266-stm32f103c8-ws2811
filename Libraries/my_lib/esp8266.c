/*
 * esp8266.c
 *
 *  Created on: 31.05.2018
 *      Author: tar
 */

/*
******************************************************************************
File        : esp8266.c
Author      : Daniel Tar
Version     :
Copyright   :
Description : esp8266 (ESP-01) library for STM32F10x
Info        : 31.05.2018
*******************************************************************************
*                  ESP8266 AT COMMANDS AND DESCRIPTION						  *
*******************************************************************************
ESP8266 AT Commands:

	Test if the communication functions
	AT

	Setting the Default Wi-Fi mode; Configuration Saved in the Flash
	AT+CWMODE_DEF=<mode>
		1: Station mode
		2: SoftAP mode
		3: SoftAP+Station mode

	Listing Available APs
	AT+CWLAP

	Connecting to an AP; Configuration Saved in the Flash
	AT+CWJAP_DEF=<ssid>,<pwd>
	AT+CWJAP_DEF="ws2811_test","0123456789"

	Disconnecting from the AP
	AT+CWQAP

	Auto-Connecting to the AP or Not
	AT+CWAUTOCONN=<enable>
		0: does NOT auto-connect to AP on power-up.
 		1: connects to AP automatically on power-up.

 	Setting the MAC Address of the ESP8266 Station; Configuration Saved
	in the Flash
	AT+CIPSTAMAC_DEF=<mac>
	AT+CIPSTAMAC_DEF="18:fe:35:98:d3:7b"

	Setting the Default IP Address of the ESP8266 Station; Configuration Saved
	in the Flash
	AT+CIPSTA_DEF=<ip>[,<gateway>,<netmask>]
	AT+CIPSTA_DEF="192.168.0.100","192.168.0.1","255.255.255.0"

	Quering the IP address of the ESP8266
	Station
	AT+CIPSTA?

	Getting the Connection Status
	AT+CIPSTATUS
	Response:
		STATUS:<stat>
		+CIPSTATUS:<linkID>,<type>,<remoteIP>,<remotePort>,<localPort>,<tetype>

 			<stat>: status of the ESP8266 Station interface.
				2: The ESP8266 Station is connected to an AP and its IP is obtained.
				3: The ESP8266 Station has created a TCP or UDP transmission.
				4: The TCP or UDP transmission of ESP8266 Station is disconnected.
				5: The ESP8266 Station does NOT connect to an AP.
			<tetype>:
				0: ESP8266 runs as a client.
				1: ESP8266 runs as a server.

	Establishing TCP Connection, UDP Transmission or SSL Connection
	AT+CIPSTART=<type>,<remoteIP>,<remotePort>[,(<UDP_localPort>),(<UDP_mode>)]
	AT+CIPSTART="UDP","0",0,1302,2

		[<UDP	mode>]: optional. In the UDP transparent transmission, the value of this parameter has to be 0.
			0: the destination peer entity of UDP will not change; this is the default setting.
			1: the destination peer entity of UDP can change once.
			2: the destination peer entity of UDP is allowed to change.

	Setting Transmission Mode
	AT+CIPMODE=<mode>

		<mode>:
			0: normal transmission mode.
			1: UART-Wi-Fi passthrough mode (transparent transmission), which can only be enabled in TCP
			   single connection mode or in UDP mode when the remote IP and port do not change.

	Saving UDP Transmission in Flash
	AT+SAVETRANSLINK=<mode>,<remoteIP>,<remotePort>,<type>[,<UDP_localPort>]

		To exit send: "+++AT+SAVETRANSLINK=1\r\n"

	Updating the Software Through Wi-Fi
	AT+CIUPDATE

	Receives Network Data
	+IPD,<len>[,<remoteIP>,<remotePort>]:<data>
	\r\n+IPD,4: '255' '0' '187' '0'

*******************************************************************************
Initialize ESP8266:
*******************************************************************************
	AT
	AT+CWMODE_DEF=1
	AT+CWLAP
	AT+CWJAP_DEF="ws2811_test","0123456789"
	AT+CWAUTOCONN=1

*******************************************************************************
After Startup:
*******************************************************************************
	AT+CIPSTART="UDP","0",0,1302,2

*******************************************************************************
Waiting for the packages:
*******************************************************************************
	\r\n+IPD,<len>[,<remoteIP>,<remotePort>]:<data>

*/

/******************************************************************************/
/*                               Includes									  */
/******************************************************************************/
#include "esp8266.h"



