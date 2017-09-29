#include "tpi_access_layer.h"
#include <stdint.h>

#define SLD                 (0x20)
#define SLD_POSTINC         (0x24)
#define SST                 (0x60)
#define SST_POSTINC         (0x64)
#define SSTPR_LO            (0x68)
#define SSTPR_HI            (0x69)
#define SIN                 (0x10)
#define SOUT                (0x90)
#define SLDCS               (0x80)
#define SSTCS               (0xC0)
#define SKEY                (0xE0)

#define IO_ADDR_HI_MASK     (0x30)
#define IO_ADDR_LO_MASK     (0x0F)
#define IO_ADDR_HI_OFFSET   (1)

#define IO_ADDR(x)          (                                                       \
                                    (((x) & IO_ADDR_HI_MASK) << IO_ADDR_HI_OFFSET)  \
                                |   (((x) & IO_ADDR_LO_MASK)                     )  \
                            )

#define CS_ADDR_MASK        (0x0F)
#define CS_ADDR(x)          ((x) & CS_ADDR_MASK)

#ifndef LOW_BYTE
#define LOW_BYTE(x)         ( (uint8_t) ( (x) & 0xFF) )
#endif

#ifndef HIGH_BYTE
#define HIGH_BYTE(x)        ( (uint8_t) ( ( (x) >> 8) & 0xFF) )
#endif

#ifdef __cplusplus
extern "C" {
#endif


TPI_WRITE_FN    write = 0;
TPI_READ_FN     read = 0;

static const uint8_t NMV_PROGRAM_ENABLE[] = { 0xFF, 0x88, 0xD8, 0xCD, 0x45, 0xAB, 0x89, 0x12 };

static void tpi_write(uint8_t value)
{
    if (write) {
        (*write)(value);
    }
}

static uint8_t tpi_read(void)
{
    if (read) {
        return (*read)();
    }

    return 0;
}

void tpi_init(TPI_WRITE_FN write_fn, TPI_READ_FN read_fn)
{
    write = write_fn;
    read  = read_fn;
}

uint8_t tpi_sld(void)
{
    tpi_write(SLD);
    return tpi_read();
}

uint8_t tpi_sld_postinc(void)
{
    tpi_write(SLD_POSTINC);
    return tpi_read();
}

void tpi_sst(uint8_t data)
{
    tpi_write(SST);
    tpi_write(data);
}

void tpi_sst_postinc(uint8_t data)
{
    tpi_write(SST_POSTINC);
    tpi_write(data);
}

void tpi_sstpr(uint16_t data)
{
    tpi_write(SSTPR_LO);
    tpi_write(LOW_BYTE(data));
    tpi_write(SSTPR_HI);
    tpi_write(HIGH_BYTE(data));
}

uint8_t tpi_sin(uint8_t address)
{
    tpi_write(SIN | IO_ADDR(address));
    return tpi_read();
}

void tpi_sout(uint8_t data, uint8_t address)
{
    tpi_write(SOUT | IO_ADDR(address));
    tpi_write(data);
}

uint8_t tpi_sldcs(uint8_t address)
{
    tpi_write(SLDCS | CS_ADDR(address));
    return tpi_read();
}

void tpi_sstcs(uint8_t data, uint8_t address)
{
    tpi_write(SLDCS | CS_ADDR(address));
    tpi_write(data);
}

void tpi_skey(void)
{
    tpi_write(SKEY);

    for (uint8_t i = 0; i < 8; i++) {
        tpi_write(NMV_PROGRAM_ENABLE[i]);
    }
}

#ifdef __cplusplus
} // extern "C"
#endif

