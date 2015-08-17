# **ESP8266-WebRadio** #
###WiFi shoutcast player based on ESP8266 chip
##Basic informations
Version: 0.11<br />
Status: under design and development
##Used hardware
WiFi and main MCU: ESP8266 (ESP-12)<br />
Additional MCU (for UI): AVR<br />
Audio decoder: VS1053<br />
##TODO list
###ESP8266
-**[HIGH]**Finish web interface<br />
-**[NORMAL]**Metadata processing<br />
-**[LOW]**UART commands for VS1053 sound settings<br />
-**[LOW]**SRAM implementation<br />
-**[LOW]**Code cleanup
###AVR
-Hardware design<br />
-Code
##Changelog
###v0.11
-**[Hardware]**Finished first revision of schematic (main board) and PCB<br />
-**[Hardware]**Added GERBER files for main board (NOT TESTED YET)<br />
-**[Hardware]**AVR is going to be only an interface between UART in ESP8266 and UI (which is LCD, encoder and buttons)
###v0.10
-**[ESP8266]**Support for user data (stations, config, etc.) storage in last 64kb of flash
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
