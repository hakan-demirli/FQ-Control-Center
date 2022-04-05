// synthesis translate_off
`timescale 1ns / 1ps
// synthesis translate_on

module uart #(
    parameter  [ 4: 0] UART_CONFIG = 5'b11000  // uart config
)(
    input wire [31:0] i_wb_adr,
    input wire i_wb_clk,
    input wire i_wb_rst,
    input wire [31:0] i_wb_dat,
    input wire i_wb_we,
    input wire i_wb_stb,
    output reg [31:0] o_wb_rdt,
    output reg o_wb_ack,
    output wire o_tx,
    input wire i_rx
);

    wire tx_busy;
    wire o_data_ready;
    wire [10: 0] o_data;

    reg rx_data_read;
    reg [15: 0] counter;
    reg [15: 0] prescalar;
    reg [ 1: 0] uart_clk;
    reg tx_start;
    reg [ 7: 0] tx_data;
    reg new_prescalar;
    reg [ 3: 0] start_pattern;
    reg rx_start;

    uart_rx #(.UART_CONFIG(UART_CONFIG)) uart_rx_0(
        .i_clk(i_wb_clk),
        .i_uart_clk(uart_clk[1]),
        .i_rst(i_wb_rst),
        .i_start(rx_start),
        .i_rx(i_rx),
        .o_data_ready(o_data_ready),
        .o_data(o_data),
        .i_data_read(rx_data_read)
    );

    uart_tx #(.UART_CONFIG(UART_CONFIG)) uart_tx_0(
        .i_clk(i_wb_clk),
        .i_uart_clk(uart_clk[1]),
        .i_rst(i_wb_rst),
        .i_start(tx_start),
        .i_data(tx_data),
        .o_busy(tx_busy),
        .o_tx(o_tx)
    );

/*
memory map

0w: prescalar
1w: cfg
2r: tx_busy
3w: data_send
4w: tx_data
5r: o_data_ready
6r: o_data
7r:
*/

    always @(posedge i_wb_clk) begin
        // idle signal values
        tx_start <= 1'b0;     //high for one cycle if host writes to tx_data
        rx_data_read <= 1'b0; //high for one cycle to inform data has been read
        new_prescalar <= 1'b0;

        if (i_wb_rst) begin
            o_wb_ack <= 1'b0;
        end else begin
            o_wb_ack <= i_wb_stb & !o_wb_ack;
            case(i_wb_adr[4:2])
                3'b000: begin
                    if(i_wb_stb & i_wb_we) begin
                        prescalar <= i_wb_dat;
                        new_prescalar <= 1'b1;
                    end
                end
                3'b001: begin
                    if(i_wb_stb & i_wb_we)
                        rx_data_read <= 1'b1; //high for one cycle
                end
                3'b010: begin
                    o_wb_rdt <= {31'd0,tx_busy};
                end
                3'b011: begin
                    //if(i_wb_stb & i_wb_we)
                        
                end
                3'b100: begin
                    if(i_wb_stb & i_wb_we) begin
                        tx_data <= i_wb_dat;
                        tx_start <= 1'b1; // high for one cycle
                    end
                end
                3'b101: begin
                    o_wb_rdt <= o_data_ready;
                end
                3'b110: begin
                    o_wb_rdt <= o_data;
                end
                default: begin
                    o_wb_rdt <= 32'd0;
                end
            endcase
        end
    end

always @(posedge i_wb_clk)
begin
    /*
    generate desired baud rate and detect start bit
    */
    if ((i_wb_rst == 1'b1) || (new_prescalar == 1'b1)) begin
        counter <= 0;
        uart_clk <= 0;
    end else if(counter == prescalar-1) begin
        counter <= 0;
        uart_clk <= uart_clk + 1;
        start_pattern[0] <= i_rx;
        start_pattern[1] <= start_pattern[0];
        start_pattern[2] <= start_pattern[1];
        start_pattern[3] <= start_pattern[2];
        if(start_pattern == 4'b1100)  // detected start bit
            rx_start = 1;
        else
            if(uart_clk[1]) // keep signal up for one uart period
                rx_start = 0;
    end else begin
        counter <= counter + 1;
    end
end
endmodule
