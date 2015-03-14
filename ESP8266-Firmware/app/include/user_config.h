#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__

// #define FLASH_512K
// #define FLASH_1M
// #define FLASH_2M
// #define FLASH_4M
// #define FLASH_8M
// #define FLASH_16M
#define FLASH_AUTOSIZE
// #define DEVELOP_VERSION
#define FULL_VERSION_FOR_USER

#define USE_OPTIMIZE_PRINTF

#define ICACHE_STORE_TYPEDEF_ATTR __attribute__((aligned(4),packed))
#define ICACHE_STORE_ATTR __attribute__((aligned(4)))
#define ICACHE_RAM_ATTR __attribute__((section(".iram0.text")))

#define CLIENT_SSL_ENABLE
#define GPIO_INTERRUPT_ENABLE

#endif	/* __USER_CONFIG_H__ */
