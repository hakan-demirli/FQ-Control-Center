#ifndef GAS_SENSORS_H
#define GAS_SENSORS_H

#include <QDebug>
#include <QThread>

#include <iostream>
#include <vector>

#include "json.hpp"


using json = nlohmann::json;

class GasSensors: public QObject {
    Q_OBJECT
public:
    explicit GasSensors(json cfg, QObject *parent = nullptr);
    ~GasSensors();
    void run();

private:
    void save_data(void);
    void update_data(void);
    void read_gas_sensors(void);

    QThread m_thread;
    json cfg;
    const unsigned int packet_size;
    std::vector<float> gas_plot_1;
    json gas_sensor_1_data;


public slots:
    void main_loop(void);

signals:
    void sendGasData(std::vector<float>);
};

#endif // GAS_SENSORS_H
