# **ESP8266-WebRadio** #
###WiFi shoutcast player based on ESP8266 chip
##Basic informations
Version: 0.7
Status: under design and development
##Used hardware
MCU: AVR<br />
WiFi: ESP8266<br />
Audio decoder: VS1053<br />
##Changelog
###v0.7
-**[ESP8266]**Simple and rough sketch of VS1053 driver (working)
-**[ESP8266]**Simple client with hardcoded IP and port (working)
-I can say that alpha version of player works for now, because I can listen to some SHOUTcast streams.
###v0.6
-**[ESP8266]**Started making new firmware based on esp_iot_rtos_sdk with bare LwIP and freeRTOS<br />
-**[Hardware]**New conception of hardware, where main MCU is ESP8266 and some small AVR is only to provide user interface (LCD, etc.) and configure ESP via UART<br />
-First tests of new firmware are very promising, because HTTP server based on Netconn is very responsive (much better than previous version of firmware)
###v0.5
-**[ESP8266]**Improved: HTTP server configuration<br />
-**[ESP8266]**Added: HTTP client UART output<br />
-**[STM32]**Added: Unclean sketch of firmware for basic testing purposes
###v0.4
-**[ESP8266]**Added: Simple CGI parser<br />
-**[ESP8266]**Added: ICY header parser
###v0.3
-**[ESP8266]**Added: Simple admin panel mockup<br />
-**[ESP8266]**Added: Connecting to client
###v0.2
-**[ESP8266]**Added: Running simple http server on port 80<br />
-**[ESP8266]**Added: Simple UART command parser for wifi configuration
###v0.1
-**[ESP8266]**Initial release - ESP8266 firmware based on cleaned nodemcu source (deleted lua, flash-fs, etc.)
