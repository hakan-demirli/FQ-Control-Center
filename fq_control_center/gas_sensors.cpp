#include "gas_sensors.h"


GasSensors::GasSensors(QObject *parent) :
    QObject(parent),
    gas_plot_1(10)
{
    gas_sensor_1_data["all"] = {0};
}

void GasSensors::run()
{

}

void GasSensors::save_data()
{

}

void GasSensors::update_data()
{

}

void GasSensors::read_gas_sensors()
{

}
