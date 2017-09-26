#ifndef TPI_ACCESS_LAYER_H
#define TPI_ACCESS_LAYER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint8_t tpi_sld         (void);
uint8_t tpi_sld_postinc (void);
void    tpi_sst         (uint8_t  data);
void    tpi_sst_postinc (uint8_t  data);
void    tpi_sstpr       (uint16_t data);
uint8_t tpi_sin         (uint8_t  address);
void    tpi_sout        (uint8_t  data, uint8_t address);
uint8_t tpi_sldcs       (uint8_t  address);
void    tpi_sstcs       (uint8_t  data, uint8_t address);
void    tpi_skey        (void);

#ifdef __cplusplus
}
#endif

#endif // TPI_ACCESS_LAYER_H
