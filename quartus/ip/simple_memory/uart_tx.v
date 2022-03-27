// synthesis translate_off
`timescale 1ns / 1ps
// synthesis translate_on

module uart_tx (
    input wire         i_clk,
    input wire         i_rst,
    input wire [16: 0] i_prescalar,
    input wire         i_data_send, //set to initiate transmit
    input wire [ 7: 0] i_data, // data to send
    input wire [ 4: 0] i_cfg,  // uart config
    output reg         o_busy, // tx is busy when high
    output reg         o_tx    // physical tx wire
);

reg [ 10: 0] data_buffer;
reg [15:0] counter;
reg [3:0] data_size;
reg uart_clk;
reg parity_bit;

reg [2:0] top_state;
localparam WAIT = 3'd0;
localparam START_BIT = 3'd1;
localparam DATA_BITS = 3'd2;
localparam PARITY_BITS = 3'd3;
localparam STOP_BIT_0 = 3'd4;
localparam STOP_BIT_1 = 3'd5;

assign data_state = i_cfg[4:3];
assign parity_type = i_cfg[2];
assign parity_enable = i_cfg[1];
assign stop_bit_state = i_cfg[0];

localparam EVEN_PARITY = 1'b0;
localparam ODD_PARITY = 1'b1;

localparam ONE_STOP = 1'b0;
localparam TWO_STOP = 1'b1;

always @(posedge i_clk or posedge i_rst)
begin
    /*
    generate desired baud rate
    */
    if (i_rst == 1) begin
        counter <= 0;
        uart_clk <= 0;
    end else if(counter == i_prescalar-1) begin
        counter <= 0;
        uart_clk = !uart_clk;
    end else begin
        counter <= counter + 1;
    end
end


reg [3:0] data_counter;
always @(posedge uart_clk or posedge i_rst)
begin
    if (i_rst == 1) begin
        o_busy <= 0;
        parity_bit <= 0;
        o_tx <= 1;
        top_state <= WAIT;
    end else begin
        case(top_state)
            WAIT: begin
                if(i_data_send)
                    top_state <= START_BIT;
                else begin
                    o_tx <= 1;
                    o_busy <= 0;
                    top_state <= WAIT;
                end
            end
            START_BIT: begin
                parity_bit <= 0;
                o_busy <= 1;
                data_counter <= 0;
                o_tx <= 0;
                top_state <= DATA_BITS;
            end
            DATA_BITS: begin
                if(data_counter != (data_size-1))begin
                    o_tx <= i_data[data_counter];
                    data_counter = data_counter + 1;
                    parity_bit = parity_bit ^ i_data[data_counter];
                    top_state <= DATA_BITS;
                end else begin
                    o_tx <= i_data[data_counter];
                    data_counter <= 0;  
                    parity_bit = parity_bit ^ i_data[data_counter];
                    if(parity_enable)
                        top_state <= PARITY_BITS;
                    else begin
                        case(stop_bit_state)
                            TWO_STOP:
                                top_state <= STOP_BIT_0;
                            ONE_STOP:
                                top_state <= STOP_BIT_1;
                        endcase
                    end
                end
            end
            PARITY_BITS: begin
                case(parity_type)
                    EVEN_PARITY:
                        o_tx <= (parity_bit);
                    ODD_PARITY:
                        o_tx <= (~parity_bit);
                endcase
                case(stop_bit_state)
                    TWO_STOP:
                        top_state <= STOP_BIT_0;
                    ONE_STOP:
                        top_state <= STOP_BIT_1;
                endcase
            end
            STOP_BIT_0: begin
                o_tx <= 1'b1;
                top_state <= STOP_BIT_1;
            end
            STOP_BIT_1: begin
                o_tx <= 1'b1;
                top_state <= WAIT;
            end
            default:
                top_state <= WAIT;
        endcase
    end
end

always @(*)
begin
    /*
    Calculate the package data section size:
        i_cfg = |xx|xx|x| = |DATABITS_COUNT|PARITY|STOPBITS|

        00->5 bits of data|00->no parity   0|1->1 stop bits  1
        01->6 bits of data|01->even parity 1|1->2 stop bits  2
        10->7 bits of data|10->no parity   0|
        11->8 bits of data|11->odd parity  1|

    */
    casex(i_cfg[4:3])
        2'b00:
            data_size <= 4'd5;
        2'b01:
            data_size <= 4'd6;
        2'b10:
            data_size <= 4'd7;
        2'b11:
            data_size <= 4'd8;
    endcase
end

endmodule
