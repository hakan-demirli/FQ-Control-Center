#include "gas_sensors.h"


GasSensors::GasSensors(json cfg, QObject *parent) :
    QObject(parent),
    cfg(cfg),
    gas_plot(constants::NUMBER_OF_GAS_SENSORS, std::vector<float>((int)cfg["packet size"])),
    gas_sensor_data(constants::NUMBER_OF_GAS_SENSORS, std::vector<float>((int)cfg["packet size"]))
{
    for (int i = 0; i<constants::NUMBER_OF_GAS_SENSORS; ++i){
        std::string file_name = constants::DATA_FILE_NAME + std::to_string(i) + ".log";
        std::fstream temp;
        temp.open(file_name, std::ios::app);
        gas_sensor_data_stream.push_back(std::move(temp));
    }
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
    for (int i = 0; i<constants::NUMBER_OF_GAS_SENSORS; ++i){
        for (float flt : gas_sensor_data[i])
            gas_sensor_data_stream[i] << flt << ",\n";
        gas_sensor_data_stream[i].clear();
        gas_sensor_data_stream[i].flush();
    }
}

void GasSensors::read_gas_sensors()
{
    for (int i = 0; i<constants::NUMBER_OF_GAS_SENSORS; ++i){
        int rint = rand() % 10 + 1;
        gas_plot[i].push_back(rint);
        gas_sensor_data[i].push_back(rint);
    }
}

void GasSensors::main_loop()
{
    srand (time(NULL));
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
            for (auto& t : gas_sensor_data)
                t.clear();
        }
        if(send_counter == cfg["send period"]){
            send_counter = 0;
            emit sendGasData(gas_plot);
            for (auto& t : gas_plot)
                t.clear();
        }
    }
}
