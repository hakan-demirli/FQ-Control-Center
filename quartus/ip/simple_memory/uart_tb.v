// synthesis translate_off
`timescale 1ns / 1ps
// synthesis translate_on
`default_nettype none


module simple_memory_tb();

reg          clk;
reg          reset_n;

reg [15:0]prescalar;
wire tx;
reg [4:0]cfg;
wire o_data_ready;
wire [10: 0] o_data;
reg  [ 7: 0] i_data;
uart_rx uart_rx_0(
    .i_clk(clk),
    .i_rst(!reset_n),
    .i_prescalar(prescalar),
    .i_rx(tx),
    .i_cfg(cfg), 
    .o_data_ready(o_data_ready),
    .o_data(o_data)
);

reg [7:0] random_data;
wire busy;
reg data_send;
uart_tx uart_tx_0(
    .i_clk(clk),
    .i_rst(!reset_n),
    .i_prescalar(prescalar),
    .i_data_send(data_send),
    .i_data(random_data),
    .i_cfg(cfg), 
    .o_busy(busy),
    .o_tx(tx)
);

always  #10 clk <= !clk;

integer i;
initial begin


    $display("Initializing inputs");
    clk = 0;
    reset_n = 0;

    cfg = 5'b11_11_1;
    prescalar = 677;
    data_send = 0;

    // wait for couple of cycles
    repeat (10) @(posedge clk);
    reset_n = 1;

    $display("Writing to memory");
    random_data = 10'b01_0101_1000;
    data_send = 1;
    @(posedge busy); // wait until tx starts
    data_send = 0;
    
    @(posedge o_data_ready); // wait until tx stops
    $display("random_data: %b",random_data);
    $display("o_data: %b",o_data);
    if(o_data[7:0] == random_data[7:0])
        $display("SUCCESS");
    else
        $display("FAILED");


    $stop;
end

endmodule
