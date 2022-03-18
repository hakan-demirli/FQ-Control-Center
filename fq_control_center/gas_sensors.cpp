#include "gas_sensors.h"


GasSensors::GasSensors(json cfg, QObject *parent) :
    QObject(parent),
    cfg(cfg),
    packet_size(cfg["packet_size"]),
    gas_plot_1(packet_size)
{
    gas_sensor_1_data["all"] = {0};
    moveToThread(&m_thread);
    m_thread.start();
}

GasSensors::~GasSensors(){

    if( m_thread.isRunning() )
    {
        m_thread.quit();
        m_thread.wait();
    }
}

void GasSensors::run()
{
    QMetaObject::invokeMethod( this, "main_loop");
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

void GasSensors::main_loop()
{

}
