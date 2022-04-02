// synthesis translate_off
`timescale 1ns / 1ps
// synthesis translate_on

module uart_rx #(
    parameter  [ 4: 0] UART_CONFIG = 5'b11000  // uart config
)(
    input wire         i_clk,
    input wire         i_uart_clk,
    input wire         i_rst,
    input wire         i_start,
    input wire         i_rx, //incoming physical rx wire
    output reg         o_data_ready, // if set data is ready to read
    output reg [10: 0] o_data, // data 
    input wire         i_data_read
);

localparam [3:0] DATA_SIZE = UART_CONFIG[4:3] == 2'b00 ? 4'd5 :
                            UART_CONFIG[4:3] == 2'b01 ? 4'd6 :
                            UART_CONFIG[4:3] == 2'b10 ? 4'd7 :
                            UART_CONFIG[4:3] == 2'b11 ? 4'd8 :
                            4'd8;

localparam WAIT = 3'd0;
localparam DATA_BITS = 3'd1;
localparam PARITY_BITS = 3'd2;
localparam STOP_BIT_0 = 3'd3;
localparam STOP_BIT_1 = 3'd4;

localparam PARITY_TYPE = UART_CONFIG[2];
localparam PARITY_ENABLE = UART_CONFIG[1];
localparam STOP_BIT_STATE = UART_CONFIG[0];

localparam EVEN_PARITY = 1'b0;
localparam ODD_PARITY = 1'b1;

localparam ONE_STOP = 1'b0;
localparam TWO_STOP = 1'b1;


reg [2:0] top_state;
reg parity_bit;
reg data_corrupted;
reg finished;
reg finished_p;
reg i_start_p;
reg i_data_read_p;
reg wait_i_start;
reg [3:0] data_counter;
reg [7:0] data_buffer;

always @(posedge i_clk or posedge i_rst) begin
    i_start_p <= i_start;
    finished_p <= finished;
    i_data_read_p <= i_data_read;
    if (i_rst == 1) begin
        wait_i_start <= 1'b1;
        o_data_ready <= 1'b0;
    end else begin
        if(i_start) // start bit is detected
            wait_i_start <= 1'b0;
        else begin
            if((~data_corrupted) && ({finished_p,finished}==2'b01)) begin//posedge of finished
                wait_i_start <= 1'b1;
                o_data_ready <= 1'b1;
            end
        end
    end
    if((~data_corrupted) && ({i_data_read_p,i_data_read}==2'b10)) begin//negedge of i_data_read
        o_data_ready <= 1'b0; // Host have seen the data. No more new data
    end
end



always @(posedge i_uart_clk or posedge i_rst)
begin
    if (i_rst == 1) begin
        finished <= 1;
        parity_bit <= 0;
        data_counter <= 0;
        top_state <= WAIT;
        data_corrupted <= 1'b1;
    end else begin
        case(top_state)
            WAIT: begin
                if(wait_i_start) begin
                    top_state <= WAIT;
                end else begin
                    top_state <= DATA_BITS;
                    finished <= 1'b0;
                end

                if(~data_corrupted)
                    o_data <= data_buffer;
            end
            DATA_BITS: begin
                data_corrupted <= 1'b0;
                if(data_counter != (DATA_SIZE-1))begin
                    data_buffer[data_counter] = i_rx;
                    data_counter <= data_counter + 1;
                    parity_bit <= parity_bit ^ data_buffer[data_counter];
                    top_state <= DATA_BITS;
                end else begin
                    data_buffer[data_counter] = i_rx;
                    data_counter <= 0;  
                    parity_bit <= parity_bit ^ data_buffer[data_counter];
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
                        data_corrupted <= (data_corrupted || (~( parity_bit==i_rx)));
                    ODD_PARITY:
                        data_corrupted <= (data_corrupted || (~(~parity_bit==i_rx)));
                endcase
                case(STOP_BIT_STATE)
                    TWO_STOP:
                        top_state <= STOP_BIT_0;
                    ONE_STOP:
                        top_state <= STOP_BIT_1;
                endcase
            end
            STOP_BIT_0: begin
                data_corrupted <= (data_corrupted || (~(1'b1==i_rx)));
                top_state <= STOP_BIT_1;
            end
            STOP_BIT_1: begin
                data_corrupted <= (data_corrupted || (~(1'b1==i_rx)));
                finished <= 1'b1;
                top_state <= WAIT;
            end
            default:
                top_state <= WAIT;
        endcase
    end
end

endmodule
