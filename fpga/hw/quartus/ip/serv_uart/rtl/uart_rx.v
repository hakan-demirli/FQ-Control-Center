// synthesis translate_off
`timescale 1ns / 1ps
// synthesis translate_on

module uart_rx (
    input wire         i_clk,
    input wire         i_rst,
    input wire [15: 0] i_prescalar,
    input wire         i_rx, //incoming physical rx wire
    input wire [ 4: 0] i_cfg, 
    output reg         o_data_ready, // if set data is ready to read
    output reg [ 10: 0] o_data // data 
);

wire [14: 0] h_prescalar;
assign h_prescalar = i_prescalar[15: 1];

reg [ 10: 0] data_buffer;
reg [15:0] counter;
reg [3:0] packet_size;
reg [1:0] uart_clk;

reg [1:0]top_state;
localparam WAIT = 2'b00;
localparam READ = 2'b01;
localparam PAUSE = 2'b10;
localparam CONTROL = 2'b11;

wire [1:0] data_state;
assign data_state = i_cfg[4:3];
assign parity_type = i_cfg[2];
assign parity_enable = i_cfg[1];
assign stop_bit_state = i_cfg[0];

localparam FIVE_DATA = 2'b00;
localparam SIX_DATA = 2'b01;
localparam SEVEN_DATA = 2'b10;
localparam EIGHT_DATA = 2'b11;

localparam EVEN_PARITY = 1'b0;
localparam ODD_PARITY = 1'b1;

localparam ONE_STOP = 1'b0;
localparam TWO_STOP = 1'b1;

reg [3:0]start_pattern;
reg letsgooo;
always @(posedge i_clk or posedge i_rst)
begin
    /*
    generate desired baud rate
    */
    if (i_rst == 1) begin
        counter <= 0;
        uart_clk <= 0;
    end else if(counter == h_prescalar-1) begin
        counter <= 0;
        uart_clk <= uart_clk + 1;
        start_pattern[0] <= i_rx;
        start_pattern[1] <= start_pattern[0];
        start_pattern[2] <= start_pattern[1];
        start_pattern[3] <= start_pattern[2];
        if(start_pattern == 4'b1100)  // detected start bit
            letsgooo = 1;
        else
            if(uart_clk[1]) // keep signal up for one uart period
                letsgooo = 0;
    end else begin
        counter <= counter + 1;
    end
end


reg [3:0] data_counter;
reg data_get;
always @(posedge uart_clk[1] or posedge i_rst)
begin
    /*
    Wait for the start bit. 
    sample the specified amount of bits
    save if stop bit(s) exist and parity is right discard if doesn't
    */
    if (i_rst == 1) begin
        top_state <= WAIT;
        o_data_ready <= 0;
    end else begin
        case(top_state)
            WAIT: begin
                data_counter <= 0;
                o_data_ready <= 0;
                if(letsgooo == 1) // detected start bit
                    top_state <= READ;
                else
                    top_state <= WAIT;
            end
            READ: begin // read packet sized number of bits after start bit
                data_buffer[data_counter] <= i_rx;
                if(data_counter == packet_size-1)begin
                    top_state <= PAUSE;
                end else
                    data_counter <= data_counter + 1;
            end
            PAUSE: begin // wait for parity and stop bit check
                    top_state <= CONTROL;
            end
            CONTROL: begin // if data is legit announce it
                if(data_get)begin
                    top_state <= WAIT;
                    o_data_ready <= 1;
                    o_data <= data_buffer;
                end else
                    top_state <= WAIT;
            end
        endcase
    end
end
reg dbg;

always @(posedge i_clk or posedge i_rst)
begin
    /*
    check if parity and stop bits are correct
    */
    if (i_rst == 1) begin
        data_get <= 0;
    end else begin
        if(1)begin
            case(data_state)
                FIVE_DATA: begin
                    case(stop_bit_state)
                        TWO_STOP: begin
                            if(parity_enable) begin
                                case(parity_type)
                                    EVEN_PARITY: begin// 5+1+2
                                        data_get <= (~^(data_buffer[5:0])) & data_buffer[7] & data_buffer[6];
                                    end
                                    ODD_PARITY: begin// 5+1+2
                                        data_get <= (^(data_buffer[5:0])) & data_buffer[7] & data_buffer[6];
                                    end
                                endcase
                            end else begin // 5+2
                                data_get <= data_buffer[6] & data_buffer[5];
                            end 
                        end
                        ONE_STOP: begin
                            if(parity_enable) begin
                                case(parity_type)
                                    EVEN_PARITY: // 5+1+1
                                        data_get <= (~^(data_buffer[5:0])) & data_buffer[6];
                                    ODD_PARITY: // 5+1+1
                                        data_get <= (^(data_buffer[5:0])) & data_buffer[6];
                                endcase
                            end else begin // 5+1
                                data_get <= data_buffer[5];
                            end 
                        end
                    endcase
                end
                SIX_DATA: begin
                    case(stop_bit_state)
                        TWO_STOP: begin
                            if(parity_enable) begin
                                case(parity_type)
                                    EVEN_PARITY: // 6+1+2
                                        data_get <= (~^(data_buffer[6:0])) & data_buffer[8] & data_buffer[7];
                                    ODD_PARITY: // 6+1+2
                                        data_get <= (^(data_buffer[6:0])) & data_buffer[8] & data_buffer[7];
                                endcase
                            end else begin // 6+2
                                data_get <= data_buffer[7] & data_buffer[6];
                            end 
                        end
                        ONE_STOP: begin
                            if(parity_enable) begin
                                case(parity_type)
                                    EVEN_PARITY: // 6+1+1
                                        data_get <= (~^(data_buffer[6:0])) & data_buffer[7];
                                    ODD_PARITY: // 6+1+1
                                        data_get <= (^(data_buffer[6:0])) & data_buffer[7];
                                endcase
                            end else begin // 6+1
                                data_get <= data_buffer[6];
                            end
                        end
                    endcase
                end
                SEVEN_DATA: begin
                    case(stop_bit_state)
                        TWO_STOP: begin
                            if(parity_enable) begin
                                case(parity_type)
                                    EVEN_PARITY: // 7+1+2
                                        data_get <= (~^(data_buffer[7:0])) & data_buffer[9] & data_buffer[8];
                                    ODD_PARITY: // 7+1+2
                                        data_get <= (^(data_buffer[7:0])) & data_buffer[9] & data_buffer[8];
                                endcase
                            end else begin // 7+2
                                data_get <= data_buffer[8] & data_buffer[7];
                            end
                        end
                        ONE_STOP: begin
                            if(parity_enable) begin
                                case(parity_type)
                                    EVEN_PARITY: // 7+1+1
                                        data_get <= (~^(data_buffer[7:0])) & data_buffer[8];
                                    ODD_PARITY: // 7+1+1
                                        data_get <= (^(data_buffer[7:0])) & data_buffer[8];
                                endcase
                            end else begin // 7+1
                                data_get <= data_buffer[7];
                            end
                        end
                    endcase
                end
                EIGHT_DATA: begin
                    case(stop_bit_state)
                        TWO_STOP: begin
                            if(parity_enable) begin
                                case(parity_type)
                                    EVEN_PARITY: // 8+1+2
                                        data_get <= (~^(data_buffer[8:0])) & data_buffer[10] & data_buffer[9];
                                    ODD_PARITY: begin // 8+1+2
                                        dbg <= (^(data_buffer[8:0]));
                                        data_get <= (^(data_buffer[8:0])) & data_buffer[10] & data_buffer[9];
                                    end
                                endcase
                            end else begin // 8+2
                                data_get <= data_buffer[9] & data_buffer[8];
                            end 
                        end
                        ONE_STOP: begin
                            if(parity_enable) begin
                                case(parity_type)
                                    EVEN_PARITY: // 8+1+1
                                        data_get <= (~^(data_buffer[8:0])) & data_buffer[9];
                                    ODD_PARITY: // 8+1+1
                                        data_get <= (^(data_buffer[8:0])) & data_buffer[9];
                                endcase
                            end else begin // 8+1
                                data_get <= data_buffer[8];
                            end 
                        end
                    endcase
                end
            endcase
        end
    end
end


always @(*)
begin

    /*
    Calculate the package size:
        i_cfg = |xx|xx|x| = |DATABITS_COUNT|PARITY|STOPBITS|

        00->5 bits of data|00->no parity   0|0->1 stop bits  1
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
    casex(i_cfg)
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
