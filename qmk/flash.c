// Functions for interacting with SPI flash
#include <string.h>
#include "flash.h"
#include "steno.h"

#ifndef __AVR__
#include "nrf_drv_qspi.h"
#include "nrf_log.h"

#define QSPI_STD_CMD_RSTEN  0x66
#define QSPI_STD_CMD_RST    0x99
#endif

void flash_init(void) {
#ifdef __AVR__
    /* enable outputs for MOSI, SCK, SS, input for MISO */
    DDRB |= _BV(DDB0);
    configure_pin_mosi();
    configure_pin_sck();
    configure_pin_ss();
    configure_pin_miso();

    unselect_card();

    SPCR = _BV(MSTR) | _BV(SPE);
    SPSR = _BV(SPI2X);
#else
    nrf_drv_qspi_config_t config = {
        .xip_offset  = 0,
        .pins = {
           .sck_pin     = 14,
           .csn_pin     = 40,
           .io0_pin     = 13,
           .io1_pin     = 15,
           .io2_pin     = NRF_QSPI_PIN_NOT_CONNECTED,
           .io3_pin     = NRF_QSPI_PIN_NOT_CONNECTED,
        },
        .irq_priority   = (uint8_t) 2,
        .prot_if = {
            .readoc     = (nrf_qspi_readoc_t) 0,
            .writeoc    = (nrf_qspi_writeoc_t) 0,
            .addrmode   = (nrf_qspi_addrmode_t) 0,
            .dpmconfig  = false,
        },
        .phy_if = {
            .sck_freq   = (nrf_qspi_frequency_t) 0,
            .sck_delay  = (uint8_t) 0,
            .spi_mode   = (nrf_qspi_spi_mode_t) 0,
            .dpmen      = false
        },
    };
    uint32_t err_code = nrfx_qspi_init(&config, NULL, NULL);
    APP_ERROR_CHECK(err_code);

    nrf_qspi_cinstr_conf_t cinstr_cfg = {
        .opcode    = QSPI_STD_CMD_RSTEN,
        .length    = NRF_QSPI_CINSTR_LEN_1B,
        .io2_level = true,
        .io3_level = true,
        .wipwait   = true,
        .wren      = true
    };
    err_code = nrf_drv_qspi_cinstr_xfer(&cinstr_cfg, NULL, NULL);
    APP_ERROR_CHECK(err_code);
    cinstr_cfg.opcode = QSPI_STD_CMD_RST;
    err_code = nrf_drv_qspi_cinstr_xfer(&cinstr_cfg, NULL, NULL);
    APP_ERROR_CHECK(err_code);
#endif
}

#ifdef __AVR__
void spi_send_byte(uint8_t b) {
    SPDR = b;
    while(!(SPSR & _BV(SPIF)));
}

uint8_t spi_recv_byte(void) {
    SPDR = 0xff;
    while(!(SPSR & _BV(SPIF)));

    return SPDR;
}
#endif

void flash_read(uint32_t addr, uint8_t *buf, uint8_t len) {
#ifdef __AVR__
    select_card();
    spi_send_byte(0x03);    // read 
    spi_send_byte((addr >> 16) & 0xFF);
    spi_send_byte((addr >> 8) & 0xFF);
    spi_send_byte(addr & 0xFF);
    for (uint8_t i = 0; i < len; i ++) {
        buf[i] = spi_recv_byte();
    }
    unselect_card();
#else
    uint8_t _buf[128];
    uint32_t read_len = (len / 4 + 1) * 4;
    uint32_t err_code = nrfx_qspi_read(_buf, read_len, addr);
    APP_ERROR_CHECK(err_code);
    memcpy(buf, _buf, len);
#endif
}

void flash_write(uint32_t addr, uint8_t *buf, uint8_t len) {
#ifdef __AVR__
    select_card();
    while (1) {
        spi_send_byte(0x05);    // read status reg
        uint8_t status = spi_recv_byte();
        if (!(status & 0x01)) {
            break;
        }
    }
    unselect_card();

    select_card();
    spi_send_byte(0x06);    // write enable
    unselect_card();

    select_card();
    spi_send_byte(0x02);    // program
    spi_send_byte((addr >> 16) & 0xFF);
    spi_send_byte((addr >> 8) & 0xFF);
    spi_send_byte(addr & 0xFF);
    for (uint8_t i = 0; i < len; i ++) {
        spi_send_byte(buf[i]);
    }
    unselect_card();
#else
    uint32_t err_code = nrfx_qspi_write(buf, len, addr);
    APP_ERROR_CHECK(err_code);
#endif
}

void flash_erase_page(uint32_t addr) {
#ifdef __AVR__
    select_card();
    while (1) {
        spi_send_byte(0x05);    // read status reg
        uint8_t status = spi_recv_byte();
        if (!(status & 0x01)) {
            break;
        }
    }
    unselect_card();

    select_card();
    spi_send_byte(0x06);    // write enable
    unselect_card();

    select_card();
    spi_send_byte(0xD8);
    spi_send_byte((addr >> 16) & 0xFF);
    spi_send_byte((addr >> 8) & 0xFF);
    spi_send_byte(addr & 0xFF);
    unselect_card();
#else
    uint32_t err_code = nrfx_qspi_erase(NRF_QSPI_ERASE_LEN_64KB, addr);
    APP_ERROR_CHECK(err_code);
#endif
}
