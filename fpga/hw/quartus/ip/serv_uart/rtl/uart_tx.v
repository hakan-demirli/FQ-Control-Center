// synthesis translate_off
`timescale 1ns / 1ps
// synthesis translate_on

module uart_tx #(
    parameter  [ 4: 0] UART_CONFIG = 5'b11000  // uart config
)(
    input wire         i_clk,
    input wire         i_uart_clk,
    input wire         i_rst,
    input wire         i_start, //set to initiate transmit
    input wire [ 7: 0] i_data, // data to send
    output wire        o_busy, // tx is busy when high
    output reg         o_tx    // physical tx wire
);

localparam [3:0] DATA_SIZE = UART_CONFIG[4:3] == 2'b00 ? 4'd5 :
                            UART_CONFIG[4:3] == 2'b01 ? 4'd6 :
                            UART_CONFIG[4:3] == 2'b10 ? 4'd7 :
                            UART_CONFIG[4:3] == 2'b11 ? 4'd8 :
                            4'd8;

localparam WAIT = 3'd0;
localparam START_BIT = 3'd1;
localparam DATA_BITS = 3'd2;
localparam PARITY_BITS = 3'd3;
localparam STOP_BIT_0 = 3'd4;
localparam STOP_BIT_1 = 3'd5;

localparam PARITY_TYPE = UART_CONFIG[2];
localparam PARITY_ENABLE = UART_CONFIG[1];
localparam STOP_BIT_STATE = UART_CONFIG[0];

localparam EVEN_PARITY = 1'b0;
localparam ODD_PARITY = 1'b1;

localparam ONE_STOP = 1'b0;
localparam TWO_STOP = 1'b1;


reg finished;
reg finished_p;
reg i_start_p;
reg wait_i_start;
reg parity_bit;
reg [2:0] top_state;
reg [3:0] data_counter;

assign o_busy = ~wait_i_start;

always @(posedge i_clk or posedge i_rst) begin
    i_start_p <= i_start;
    finished_p <= finished;
    if (i_rst == 1) begin
        wait_i_start <= 1;
    end else begin
        if({i_start_p,i_start} == 2'b10) //negedge of i_start
            wait_i_start <= 1'b0;
        else begin
            if({finished_p,finished}==2'b01) begin//posedge of finished
                wait_i_start <= 1'b1;
            end
        end
    end
end



always @(posedge i_uart_clk or posedge i_rst)
begin
    if (i_rst == 1) begin
        finished <= 1;
        parity_bit <= 0;
        o_tx <= 1;
        top_state <= WAIT;
    end else begin
        case(top_state)
            WAIT: begin
                if(wait_i_start) begin
                    o_tx <= 1;
                    top_state <= WAIT;
                end else begin
                    top_state <= START_BIT;
                    finished <= 1'b0;
                end
            end
            START_BIT: begin
                parity_bit <= 0;
                data_counter <= 0;
                o_tx <= 0;
                top_state <= DATA_BITS;
            end
            DATA_BITS: begin
                if(data_counter != (DATA_SIZE-1))begin
                    o_tx <= i_data[data_counter];
                    data_counter <= data_counter + 1;
                    parity_bit <= parity_bit ^ i_data[data_counter];
                    top_state <= DATA_BITS;
                end else begin
                    o_tx <= i_data[data_counter];
                    data_counter <= 0;  
                    parity_bit <= parity_bit ^ i_data[data_counter];
                    if(PARITY_ENABLE)
                        top_state <= PARITY_BITS;
                    else begin
                        case(STOP_BIT_STATE)
                            TWO_STOP:
                                top_state <= STOP_BIT_0;
                            ONE_STOP:
                                top_state <= STOP_BIT_1;
                        endcase
                    end
                end
            end
            PARITY_BITS: begin
                case(PARITY_TYPE)
                    EVEN_PARITY:
                        o_tx <= (parity_bit);
                    ODD_PARITY:
                        o_tx <= (~(parity_bit));
                endcase
                case(STOP_BIT_STATE)
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
                finished <= 1'b1;
                top_state <= WAIT;
            end
            default:
                top_state <= WAIT;
        endcase
    end
end

endmodule
