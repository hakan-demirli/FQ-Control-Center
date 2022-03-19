#include "gas_sensors.h"


GasSensors::GasSensors(json cfg, QObject *parent) :
    QObject(parent),
    cfg(cfg),
    gas_plot_1((int)cfg["packet size"]),
    gas_sensor_1_data((int)cfg["packet size"])
{
    gas_sensor_data_stream.open(DATA_FILE_0, std::ios::app);
    gas_sensor_data_stream.clear();
    gas_sensor_data_stream.flush();
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
    for (float i : gas_sensor_1_data)
        gas_sensor_data_stream << i << ",\n";
    gas_sensor_data_stream.clear();
    gas_sensor_data_stream.flush();
}

void GasSensors::read_gas_sensors()
{
    a += 1;
    gas_plot_1.push_back(a);
    gas_sensor_1_data.push_back(a);
}

void GasSensors::main_loop()
{
    unsigned int sleep_counter = 0;
    unsigned int send_counter = 0;
    while(toggle_sensors){
        QThread::msleep(cfg["read period"]);
        sleep_counter += 1;
        send_counter += 1;
        read_gas_sensors();

        if(sleep_counter == cfg["save period"]){
            sleep_counter = 0;
            save_data();
            gas_sensor_1_data.clear();
            a=0;
        }
        if(send_counter == cfg["send period"]){
            send_counter = 0;
            emit sendGasData(gas_plot_1);
            gas_plot_1.clear();
        }
    }
}
