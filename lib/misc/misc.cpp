#include "misc.h"

String spi_controller_name(SPIClass *pspi)
{

    SPI_TypeDef *x = pspi->getHandle()->Instance;
    if (x == SPI1)
        return "SPI1";
    else if (x == SPI2)
        return "SPI2";
    else if (x == SPI3)
        return "SPI3";
    else if (x == SPI4)
        return "SPI4";
    else if (x == SPI5)
        return "SPI5";
    else if (x == SPI6)
        return "SPI6";
    else
        return "Unknown SPI controller";
}
