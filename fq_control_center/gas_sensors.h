#ifndef GAS_SENSORS_H
#define GAS_SENSORS_H

#include <QDebug>

#include <iostream>
#include <vector>

#include "json.hpp"


using json = nlohmann::json;

class GasSensors: public QObject {
    Q_OBJECT
public:
    explicit GasSensors(QObject *parent = nullptr);
    void run();

private:
    void save_data();
    void update_data();
    void read_gas_sensors();

    std::vector<float> gas_plot_1;
    json gas_sensor_1_data;

};

#endif // GAS_SENSORS_H
