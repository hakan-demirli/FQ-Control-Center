#ifndef GAS_SENSORS_H
#define GAS_SENSORS_H

#include <QDebug>

#include <iostream>
#include <vector>

#include "json.hpp"


using json = nlohmann::json;

class gas_sensors: public QObject {
    Q_OBJECT
public:
    explicit gas_sensors(QObject *parent = nullptr);

private:
    void save_data();
    void update_data();
    void read_gas_sensors();

    std::vector<float> gas_plot_1;
    json gas_sensor_1_data;


};

#endif // GAS_SENSORS_H
