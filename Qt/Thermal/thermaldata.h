#ifndef THERMALDATA_H
#define THERMALDATA_H

#include <QImage>

class ThermalData
{
public:
    ThermalData(QImage *thermalImage, int rows, int cols, int *temperatureData);
    QImage *thermalImage;
    int rows, cols;
    int *temperatureData;
};

#endif // THERMALDATA_H
