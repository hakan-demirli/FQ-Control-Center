#ifndef GAS_SENSORS_H
#define GAS_SENSORS_H

#include <QDebug>
#include <QThread>

#include <iostream>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include <time.h>

#include "constants.h"
#include "json.hpp"
#include "hps.h"


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
    std::vector<std::vector<float>> gas_plot;
    std::vector<std::vector<float>> gas_sensor_data;

    std::vector<std::fstream> gas_sensor_data_stream;
    json data_j;

public slots:
    void main_loop(void);

signals:
    void sendGasData(std::vector<std::vector<float>>);
};

#endif // GAS_SENSORS_H
