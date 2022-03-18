#include "gas_sensors.h"


gas_sensors::gas_sensors(QObject *parent) :
    QObject(parent),
    gas_plot_1(10)
{
    gas_sensor_1_data["all"] = {0};
}

void gas_sensors::save_data()
{

}

void gas_sensors::update_data()
{

}

void gas_sensors::read_gas_sensors()
{

}
