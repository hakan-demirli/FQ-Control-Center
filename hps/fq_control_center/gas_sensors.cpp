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
        update_serv_sw();
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
    //initialize_fpga_bridge();
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
        qDebug() << "\nERROR: Failed to open the memory Map!";
        std::abort();
    }
    // Allocate a pointer to the maped address space
    uint32_t* fpga_bridge_base = (uint32_t*)fpga_bridge_map;

    serv_uart_adr = (fpga_bridge_base + (FPGA_SERV_UART_BASE / 4));
    serv_uart_dbg_adr = serv_uart_adr + (SERV_DEBUG_MODE_ADR_OFFSET/4);
}

void GasSensors::update_serv_sw(){
    static const uint8_t serv_hex[SERV_HEX_BYTE_SIZE] = {};

    qDebug() << "Enable Debug mode";
    *serv_uart_dbg_adr = 0xff; // enable debug mode
    for (int i = 0; i < SERV_HEX_BYTE_SIZE; i+=4) {
         uint32_t word = serv_hex[i] | ((serv_hex[i+1]) << 8) | ((serv_hex[i+2]) << 16) | ((serv_hex[i+3]) << 24);
         volatile uint32_t* wr_addr = serv_uart_adr + i;
         *wr_addr = word;
     }
    qDebug() << "Disable Debug mode";
    *serv_uart_dbg_adr = 0x00; // disable debug mode

    qDebug() << "First Instruction:" << *serv_uart_adr;
    qDebug() << "Finished serv software update";
}

void GasSensors::read_from_serv()
{
    for (int i = 0; i<constants::NUMBER_OF_GAS_SENSORS; ++i){
        #ifdef  FPGA
            int rint[4] = {serv_uart_adr[0],serv_uart_adr[1],serv_uart_adr[2],serv_uart_adr[3]};
        #else
            int rint[4] = {rand() % 10 + 1,rand() % 10 + 1,rand() % 10 + 1,rand() % 10 + 1};
        #endif
        gas_plot[i].push_back(rint[0]);
        gas_plot[i].push_back(rint[1]);
        gas_plot[i].push_back(rint[2]);
        gas_plot[i].push_back(rint[3]);
        gas_sensor_data[i].push_back(rint[0]);
        gas_sensor_data[i].push_back(rint[1]);
        gas_sensor_data[i].push_back(rint[2]);
        gas_sensor_data[i].push_back(rint[3]);
    }
}

void GasSensors::main_loop()
{

    #ifdef  FPGA
    #else
        srand (time(NULL));
    #endif

    unsigned int sleep_counter = 0;
    unsigned int send_counter = 0;

    while(toggle_sensors){
        QThread::msleep(cfg["read period"]);
        sleep_counter += 1;
        send_counter += 1;
        read_from_serv();

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
