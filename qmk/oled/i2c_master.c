#include <string.h>
#include "i2c_master.h"
#include "nrf_drv_twi.h"

static const nrf_drv_twi_t twi = NRF_DRV_TWI_INSTANCE(0);
static volatile bool twi_done = false;

void twi_handler(nrf_drv_twi_evt_t const *p_event, void *p_context) {
    switch (p_event->type) {
        case NRF_DRV_TWI_EVT_DONE:
            twi_done = true;
            break;
        default:
            break;
    }
}
void i2c_init(void) {
    const nrf_drv_twi_config_t twi_config = {
       .scl                = 11,
       .sda                = 12,
       .frequency          = NRF_DRV_TWI_FREQ_100K,
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
       .clear_bus_init     = false
    };
    uint32_t err_code = nrf_drv_twi_init(&twi, &twi_config, twi_handler, NULL);
    APP_ERROR_CHECK(err_code);
    nrf_drv_twi_enable(&twi);
}

i2c_status_t i2c_transmit(uint8_t address, const uint8_t* data, uint16_t length, uint16_t timeout) {
    twi_done = false;
    uint32_t err_code = nrf_drv_twi_tx(&twi, address, data, length, false);
    APP_ERROR_CHECK(err_code);
    while (!twi_done);
    return err_code;
}

i2c_status_t i2c_write_reg(uint8_t devaddr, uint8_t regaddr, const uint8_t* data, uint16_t length, uint16_t timeout) {
    twi_done = false;
    uint8_t buf[length + 1];
    buf[0] = regaddr;
    memcpy(buf + 1, data, length);
    uint32_t err_code = nrf_drv_twi_tx(&twi, devaddr, buf, length, false);
    APP_ERROR_CHECK(err_code);
    while (!twi_done);
    return err_code;
}
