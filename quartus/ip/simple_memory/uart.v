// synthesis translate_off
`timescale 1ns / 1ps
// synthesis translate_on

module uart (
    input wire         i_clk,
    input wire         i_rst,
    input wire [16: 0] i_prescalar,
    input wire         i_rx,
    input wire [ 4: 0] cfg, 
    output reg         o_data_ready, 
    output reg [ 10: 0] o_data
);

reg [ 10: 0] data_buffer;
reg [15:0] counter;
reg [3:0] packet_size;
reg uart_clk;

reg [1:0]top_state;
localparam WAIT_STATE = 2'b00;
localparam READ_STATE = 2'b01;
localparam PAUSE_STATE = 2'b10;
localparam CONTROL_STATE = 2'b11;

wire data_state;
wire stop_bit_state;
assign data_state = cfg[4:3];
assign parity_type = cfg[2];
assign parity_enable = cfg[1];
assign stop_bit_state = cfg[0];

localparam FIVE_DATA = 2'b00;
localparam SIX_DATA = 2'b01;
localparam SEVEN_DATA = 2'b10;
localparam EIGHT_DATA = 2'b11;

localparam EVEN_PARITY = 1'b0;
localparam ODD_PARITY = 1'b1;


always @(posedge i_clk or posedge i_rst)
begin
    /*
    generate desired baud rate
    */
    if (i_rst == 1) begin
        counter <= 0;
        uart_clk <= 0;
    end else if(counter == i_prescalar+1) begin
        counter <= 0;
    end else begin
        counter <= counter + 1;
        if(counter == i_prescalar) begin
            uart_clk = !uart_clk;
        end
    end

end


reg [3:0] data_counter;
reg data_get;
always @(posedge uart_clk or posedge i_rst)
begin
    /*
    Wait for the start bit. 
    sample the specified amount of bits
    save if stop bit(s) exist discard if doesn't
    */
    if (i_rst == 1) begin
        top_state <= WAIT_STATE;
    end else begin
        case(top_state)
            WAIT_STATE: begin
                data_counter <= 0;
                data_get <= 0;
                if(i_rx == 0) // detected start bit
                    top_state <= READ_STATE;
                else
                    top_state <= WAIT_STATE;
            end
            READ_STATE: begin // read packet sized number of bits after start bit
                data_buffer[data_counter] <= i_rx;
                if(data_counter == packet_size-1)begin
                    top_state <= PAUSE_STATE;
                end else
                    data_counter <= data_counter + 1;
            end
            PAUSE_STATE: begin // wait for parity and stop bit check
                    top_state <= CONTROL_STATE;
            end
            CONTROL_STATE: begin // if data is legit announce it
                if(o_data_ready)begin
                    top_state <= WAIT_STATE;
                    data_get <= 1;
                    o_data <= data_buffer;
                end else
                    top_state <= WAIT_STATE;
            end
        endcase
    end
end


always @(posedge i_clk or posedge i_rst)
begin
    /*
    check if parity and stop bits are correct
    */
    if (i_rst == 1) begin
        o_data_ready <= 0;
    end else begin
        if(CONTROL_STATE == top_state)begin
            case(data_state)
                FIVE_DATA: begin
                    if(stop_bit_state)begin // two stop bits
                        if(parity_enable) begin
                            case(parity_type)
                                EVEN_PARITY: // 5+1+2
                                    o_data_ready <= (~^(data_buffer[5:0])) & data_buffer[7] & data_buffer[6];
                                ODD_PARITY: // 5+1+2
                                    o_data_ready <= (^(data_buffer[5:0])) & data_buffer[7] & data_buffer[6];
                            endcase
                        end else begin // 5+2
                            o_data_ready <= data_buffer[6] & data_buffer[5];
                        end 
                    end else begin // one stop bit
                        if(parity_enable) begin
                            case(parity_type)
                                EVEN_PARITY: // 5+1+1
                                    o_data_ready <= (~^(data_buffer[5:0])) & data_buffer[6];
                                ODD_PARITY: // 5+1+1
                                    o_data_ready <= (^(data_buffer[5:0])) & data_buffer[6];
                            endcase
                        end else begin // 5+1
                            o_data_ready <= data_buffer[5];
                        end 
                    end
                end
                SIX_DATA: begin
                    if(stop_bit_state)begin // two stop bits
                        if(parity_enable) begin
                            case(parity_type)
                                EVEN_PARITY: // 6+1+2
                                    o_data_ready <= (~^(data_buffer[6:0])) & data_buffer[8] & data_buffer[7];
                                ODD_PARITY: // 6+1+2
                                    o_data_ready <= (^(data_buffer[6:0])) & data_buffer[8] & data_buffer[7];
                            endcase
                        end else begin // 6+2
                            o_data_ready <= data_buffer[7] & data_buffer[6];
                        end 
                    end else begin // one stop bit
                        if(parity_enable) begin
                            case(parity_type)
                                EVEN_PARITY: // 6+1+1
                                    o_data_ready <= (~^(data_buffer[6:0])) & data_buffer[7];
                                ODD_PARITY: // 6+1+1
                                    o_data_ready <= (^(data_buffer[6:0])) & data_buffer[7];
                            endcase
                        end else begin // 6+1
                            o_data_ready <= data_buffer[6];
                        end 
                    end
                end
                SEVEN_DATA: begin
                    if(stop_bit_state)begin // two stop bits
                        if(parity_enable) begin
                            case(parity_type)
                                EVEN_PARITY: // 7+1+2
                                    o_data_ready <= (~^(data_buffer[7:0])) & data_buffer[9] & data_buffer[8];
                                ODD_PARITY: // 7+1+2
                                    o_data_ready <= (^(data_buffer[7:0])) & data_buffer[9] & data_buffer[8];
                            endcase
                        end else begin // 7+2
                            o_data_ready <= data_buffer[8] & data_buffer[7];
                        end 
                    end else begin // one stop bit
                        if(parity_enable) begin
                            case(parity_type)
                                EVEN_PARITY: // 7+1+1
                                    o_data_ready <= (~^(data_buffer[7:0])) & data_buffer[8];
                                ODD_PARITY: // 7+1+1
                                    o_data_ready <= (^(data_buffer[7:0])) & data_buffer[8];
                            endcase
                        end else begin // 7+1
                            o_data_ready <= data_buffer[7];
                        end 
                    end
                end
                EIGHT_DATA: begin
                    if(stop_bit_state)begin // two stop bits
                        if(parity_enable) begin
                            case(parity_type)
                                EVEN_PARITY: // 8+1+2
                                    o_data_ready <= (~^(data_buffer[8:0])) & data_buffer[10] & data_buffer[9];
                                ODD_PARITY: // 8+1+2
                                    o_data_ready <= (^(data_buffer[8:0])) & data_buffer[10] & data_buffer[9];
                            endcase
                        end else begin // 8+2
                            o_data_ready <= data_buffer[9] & data_buffer[8];
                        end 
                    end else begin // one stop bit
                        if(parity_enable) begin
                            case(parity_type)
                                EVEN_PARITY: // 8+1+1
                                    o_data_ready <= (~^(data_buffer[8:0])) & data_buffer[9];
                                ODD_PARITY: // 8+1+1
                                    o_data_ready <= (^(data_buffer[8:0])) & data_buffer[9];
                            endcase
                        end else begin // 8+1
                            o_data_ready <= data_buffer[8];
                        end 
                    end
                end
            endcase
        end
    end
end


always @(posedge i_clk or posedge i_rst)
begin

    /*
    Calculate the package size:
        cfg = |xx|xx|x| = |DATABITS_COUNT|PARITY|STOPBITS|

        00->5 bits of data|00->no parity   0|1->1 stop bits  1
        01->6 bits of data|01->even parity 1|1->2 stop bits  2
        10->7 bits of data|10->no parity   0|
        11->8 bits of data|11->odd parity  1|

        (5,6,7,8) and (0,1) and (1,2)

        501 6
        502 7
        511 7
        512 8

        601 7
        602 8
        611 8
        612 9

        701 8
        702 9
        711 9
        712 10

        801 9
        802 10
        811 10
        812 11

    */
    casex(cfg)
        5'b00_?0_0:                                     // 6bit
            packet_size <= 4'd6;
        5'b00_?0_1, 5'b00_?1_0, 5'b01_?0_0:             // 7bit
            packet_size <= 4'd7;
        5'b00_?1_1, 5'b01_?0_1, 5'b01_?1_0,5'b10_?0_0:  // 8bit
            packet_size <= 4'd8;
        5'b01_?1_1, 5'b10_?0_1, 5'b10_?1_0, 5'b11_?0_0: // 9bit
            packet_size <= 4'd9;
        5'b10_?1_1, 5'b11_?0_1, 5'b11_?1_0:             // 10bit
            packet_size <= 4'd10;
        5'b11_?1_1:                                     // 11bit
            packet_size <= 4'd11;
        default:
            packet_size <= 4'd8;
    endcase
end

endmodule
