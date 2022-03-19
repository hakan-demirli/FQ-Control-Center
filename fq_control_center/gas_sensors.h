#ifndef GAS_SENSORS_H
#define GAS_SENSORS_H

#include <QDebug>
#include <QThread>

#include <iostream>
#include <vector>
#include <fstream>

#include "json.hpp"


using json = nlohmann::json;

class GasSensors: public QObject {
    Q_OBJECT
public:
    json cfg;
public:
    explicit GasSensors(json cfg, QObject *parent = nullptr);
    ~GasSensors();
    void run();

    bool toggle_sensors;
    unsigned int toggle_index;

private:
    void save_data(void);
    void read_gas_sensors(void);

    QThread m_thread;
    std::vector<float> gas_plot_1;
    std::vector<float> gas_sensor_1_data;
    const std::string DATA_FILE_0 = "./data/gas_sensor_0_data.log";
    std::fstream gas_sensor_data_stream;
    json data_j;
    int a = 11;

public slots:
    void main_loop(void);

signals:
    void sendGasData(std::vector<float>);
};

#endif // GAS_SENSORS_H
