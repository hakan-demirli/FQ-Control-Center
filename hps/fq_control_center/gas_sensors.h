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

//#define FPGA
#define LWH2F_RANGE 0x07FFFF
#define FPGA_SERV_UART_BASE   0x8000
#define SERV_HEX_BYTE_SIZE   2048
#define SERV_DEBUG_MODE_ADR_OFFSET 0x4000

using json = nlohmann::json;

class GasSensors: public QObject {
    Q_OBJECT
public:
    const json cfg;
public:
    explicit GasSensors(json cfg, QObject *parent = nullptr);
    ~GasSensors();
    void run();

    bool toggle_sensors;
    unsigned int toggle_index;

private:
    void save_data(void);
    void initialize_fpga_bridge(void);
    void read_from_serv(void);
    void update_serv_sw(void);

    QThread m_thread;

    std::vector<std::vector<float>> gas_plot;
    std::vector<std::vector<float>> gas_sensor_data;
    std::vector<std::fstream> gas_sensor_data_stream;

    void* fpga_bridge_map;
    uint32_t* fpga_bridge_base;
    volatile uint32_t* serv_uart_adr;
    volatile uint32_t* serv_uart_dbg_adr;

public slots:
    void main_loop(void);

signals:
    void sendGasData(std::vector<std::vector<float>>);
};

#endif // GAS_SENSORS_H
