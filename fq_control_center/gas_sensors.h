#ifndef GAS_SENSORS_H
#define GAS_SENSORS_H

#include <QDebug>
#include <QThread>

#include <iostream>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>

#include "constants.h"
#include "json.hpp"
#include "hps.h"

#define FPGA 1
#define LWH2F_RANGE 0x07FFFF
#define SIMPLE_MEMORY_OFFSET 0x07FFFF


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
    void initialize_fpga_bridge();
    void read_gas_sensors(void);

    QThread m_thread;

    std::vector<std::vector<float>> gas_plot;
    std::vector<std::vector<float>> gas_sensor_data;
    std::vector<std::fstream> gas_sensor_data_stream;

    void* fpga_bridge_map;
    uint32_t* fpga_bridge_base;
    volatile uint32_t* simple_memory_0;

public slots:
    void main_loop(void);

signals:
    void sendGasData(std::vector<std::vector<float>>);
};

#endif // GAS_SENSORS_H
