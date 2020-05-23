uint8_t flash_init(void)
{
    /* enable outputs for MOSI, SCK, SS, input for MISO */
    DDRB |= _BV(DDB0);
    PORTB |= _BV(PORTB0);
    configure_pin_mosi();
    configure_pin_sck();
    configure_pin_ss();
    configure_pin_miso();

    unselect_card();

    /* initialize SPI with lowest frequency; max. 400kHz during identification mode of card */
    SPCR = _BV(MSTR) | _BV(SPE) | _BV(SPR1) | _BV(SPR0);
    /* SPCR = (0 << SPIE) | /1* SPI Interrupt Enable *1/ */
    /*        (1 << SPE)  | /1* SPI Enable *1/ */
    /*        (0 << DORD) | /1* Data Order: MSB first *1/ */
    /*        (1 << MSTR) | /1* Master mode *1/ */
    /*        (0 << CPOL) | /1* Clock Polarity: SCK low when idle *1/ */
    /*        (0 << CPHA) | /1* Clock Phase: sample on rising SCK edge *1/ */
    /*        (0 << SPR1) | /1* Clock Frequency: f_OSC / 128 *1/ */
    /*        (0 << SPR0); */
    SPSR = 0;
