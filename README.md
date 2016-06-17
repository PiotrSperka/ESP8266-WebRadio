# **ESP8266-WebRadio** #
###WiFi shoutcast player based on ESP8266 chip
##Basic informations
Version: 0.16<br />
Status: under design and development
##Used hardware
WiFi and main MCU: ESP8266 (ESP-12 with 32Mbits flash)<br />
Additional MCU (as a bridge UART<=>UI): AVR<br />
Audio decoder: VS1053<br />
##TODO list
-**[HIGH]**LCD and encoder <=> UART converter based on AVR (ATmega88?)<br />
-**[NORMAL]**FM module (RDA5807M) implementation
-**[NORMAL]**External SRAM implementation<br />
-**[NORMAL]**Reconnect to webradio when internet connection reappears<br />
-**[LOW]**Code cleanup and optimization
##Changelog
###v0.17
-**[MERGE]**Merged with karawin's fork (version 1.0.4)<br />
He made lots of changes, especially he improved stability and webpage user experience. For details please visit his Github webpage:<br />
https://github.com/karawin/Ka-Radio
###v0.16
-**[Hardware]**Slightly updated GERBER files
###v0.15
-**[ESP8266]**Metadata processing
###v0.14
-**[MERGE]**Merged with karawin's fork, he made following changes:<br />
-New development based on the new https://github.com/espressif/ESP8266_RTOS_SDK<br />
-Software improved, new web control<br />
-Compatible with mobile<br />
-Stable<br />
-Tools to save and restore the stations database
###v0.13
-**[ESP8266]**Some kind of beta version of web interface is ready.<br />
-**[ESP8266]**Works saving and loading stations and settings to/from flash<br />
-**[ESP8266]**Added "How to run" document
###v0.12
-**[ESP8266]**Development of functions for saving and reading settings from flash "eeprom"<br />
-**[ESP8266]**Functions for driving GPIO16<br />
-**[ESP8266]**Development of webinterface<br />
-**[ESP8266]**Webclient is using socket
###v0.11
-**[Hardware]**Finished first revision of schematic (main board) and PCB<br />
-**[Hardware]**Added GERBER files for main board (NOT TESTED YET)<br />
-**[Hardware]**AVR is going to be only an interface between UART in ESP8266 and UI (which is LCD, encoder and buttons)
###v0.10
-**[ESP8266]**Support for user data (stations, config, etc.) storage in last 64kb of flash<br />
-**[Hardware]**Schematic of main board
###v0.9
-**[ESP8266]**Sound settings through web interface<br />
-**[ESP8266]**Web interface improvements
###v0.8
-**[ESP8266]**Webserver uses sockets - improved stability<br />
-**[ESP8266]**Improvments of webinterface (now supports Instant Play)<br />
-**[ESP8266]**Improvments of webserver (simple POST parser)<br />
-**[ESP8266]**DNS lookup
###v0.7
-**[ESP8266]**Simple and rough sketch of VS1053 driver (working)<br />
-**[ESP8266]**Rough sketch of webclient is working<br />
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

