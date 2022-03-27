// synthesis translate_off
`timescale 1ns / 1ps
// synthesis translate_on
`default_nettype none


module simple_memory_tb();


parameter ADDRESS_SIZE = 4;
parameter DELAY = 5;
integer C_DATA[ADDRESS_SIZE-1:0];
integer mismatch_error = 0;

integer var;
initial begin
  $display("Initializing test data randomly");
  for(var=0; var<ADDRESS_SIZE; var = var + 1) begin
    C_DATA[var] = $urandom();
  end
end

reg [ADDRESS_SIZE-1: 0] address;
reg          chipselect;
reg          clk;
reg          reset_n;
reg          write_n;
reg [ 31: 0] writedata;
wire [ 31: 0] readdata;

simple_memory #(.ADDRESS_SIZE(ADDRESS_SIZE)) sm_0 (
    .address(address),
    .chipselect(chipselect),
    .clk(clk),
    .reset_n(reset_n),
    .write_n(write_n),
    .writedata(writedata),
    .readdata(readdata)
);

parameter i_prescalar = 111;
reg i_rx;
wire o_tx;
reg [4:0]i_cfg = 5'b01_11_1;
wire o_data_ready;
wire [10:0]o_data;
uart uart_rx_0(
    .i_clk(clk),
    .i_rst(!reset_n),
    .i_prescalar(i_prescalar),
    .i_rx(o_tx),
    .i_cfg(i_cfg), 
    .o_data_ready(o_data_ready),
    .o_data(o_data)
);

reg [7:0] i_data_send;
wire o_busy;
uart_tx uart_tx_0(
    .i_clk(clk),
    .i_rst(!reset_n),
    .i_prescalar(i_prescalar),
    .i_data_send(i_data_send),
    .i_data(8'b01010101),
    .i_cfg(i_cfg), 
    .o_busy(o_busy),
    .o_tx(o_tx)
);



always  #10 clk <= !clk;

always @(posedge clk)
begin
    i_rx = $urandom();
end

always @(posedge clk)
begin
    i_data_send = $urandom();
end




integer i;
initial begin
    $display("Initializing inputs");
    address = 0;
    chipselect = 0;
    clk = 0;
    reset_n = 0;
    write_n = 1;
    writedata = 0;

    // wait for couple of cycles
    repeat (10) @(posedge clk);
    reset_n = 1;

    $display("Writing to memory");
    for(i=0;i<ADDRESS_SIZE;i=i+1)begin
        address = i;
        writedata = C_DATA[i];
        chipselect = 1;
        write_n = 0; // write operation
        @(posedge clk);
    end

    $display("Reading from memory");
    for(i=0;i<ADDRESS_SIZE;i=i+1)begin
        address = i;
        chipselect = 1; 
        write_n = 1; // Read operation
        @(posedge clk);
        if(C_DATA[i] != readdata) begin
            $display("RW Not equal, time: %t, read: %h, real: %h",$time,C_DATA[i],readdata);
        end
    end
    if(mismatch_error)
        $display("FAILED");
    else
        $display("SUCCESS");
    //$stop;
end

endmodule
