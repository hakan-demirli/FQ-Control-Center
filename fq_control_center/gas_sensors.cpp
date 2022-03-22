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

    #ifdef  FPGA
        initialize_fpga_bridge();
    #endif
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
    initialize_fpga_bridge();
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

void GasSensors::initialize_fpga_bridge(){

    qDebug() << "Accessing the FPGA Brige";

    // Open the Linux memory driver with read and write privalages
    int fd = open("/dev/mem", (O_RDWR | O_SYNC));

    // Check that the opening was sucsessfull
    if (fd < 0){
        qDebug() << "\nERROR: Failed to read the memory driver!";
        std::abort();
    }

    // Create a Memory Map to access the FPGA Bridge address space
    fpga_bridge_map = mmap(NULL, LWH2F_RANGE, PROT_WRITE | PROT_READ, MAP_SHARED, fd,
        ALT_LWFPGASLVS_OFST);

    // Check that the opening was sucsessful
    if (fpga_bridge_map == MAP_FAILED){
        qDebug() << "\nERROR: Failed to open the memory Map!" << endl;
        std::abort();
    }
    // Allocate a pointer to the maped address space
    uint32_t* fpga_bridge_base = (uint32_t*)fpga_bridge_map;

    qDebug() << "Read the simple memory over the FPGA Bridge" << endl;

    simple_memory_0 = (fpga_bridge_base + (SIMPLE_MEMORY_OFFSET / 4));
    *simple_memory_0 = 0;

    qDebug() << "Simple Memory:" << *simple_memory_0;

    qDebug() << "Finished fpga bridge initialization";
}

void GasSensors::read_gas_sensors()
{
    for (int i = 0; i<constants::NUMBER_OF_GAS_SENSORS; ++i){
        //int rint = rand() % 10 + 1;
        *simple_memory_0 = *simple_memory_0 + 1;
        int rint = *simple_memory_0;
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
