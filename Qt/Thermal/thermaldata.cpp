#include "thermaldata.h"

ThermalData::ThermalData(QImage *thermalImage, int rows, int cols, int *temperatureData)
{
    this->thermalImage = thermalImage;
    this->rows = rows;
    this->cols = cols;
    this->temperatureData = temperatureData;
}

