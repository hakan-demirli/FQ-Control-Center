// synthesis translate_off
`timescale 1ns / 1ps
// synthesis translate_on
`default_nettype none


module simple_memory_tb();


parameter MEM_SIZE = 4;
parameter DELAY = 5;
integer C_DATA[MEM_SIZE-1:0];
integer mismatch_wb = 0;
integer mismatch_av = 0;

integer var;
initial begin
  $display("Initializing test data randomly");
  for(var=0; var<MEM_SIZE; var = var + 1) begin
    C_DATA[var] = $urandom();
  end
end

reg  [ 31: 0] address;
reg           chipselect;
reg           clk;
reg           reset_n;
reg           write_n;
reg  [ 31: 0] writedata;
wire [ 31: 0] readdata;

simple_memory_av #(.MEM_SIZE(MEM_SIZE)) avm (
    .address(address),
    .chipselect(chipselect),
    .clk(clk),
    .reset_n(reset_n),
    .write_n(write_n),
    .writedata(writedata),
    .readdata(readdata)
);
wire ack;
wire [ 31: 0] wb_rdt;
simple_memory_wb #(.MEM_SIZE(MEM_SIZE)) wbm (
    .i_wb_clk(clk),
    .i_wb_rst(~reset_n),
    .i_wb_adr(address),
    .i_wb_dat(writedata),
    .i_wb_we(~write_n),
    .i_wb_stb(chipselect && ~write_n),
    .o_wb_rdt(wb_rdt)
);

always  #10 clk <= !clk;

integer i;
initial begin
    $display("Initializing inputs");
    address = 0;
    chipselect = 0;
    clk = 0;
    reset_n = 1;
    write_n = 1;
    writedata = 0;

    // wait for couple of cycles
    repeat (10) @(posedge clk);

    $display("Writing to memory");
    for(i=0;i<MEM_SIZE;i=i+1)begin
        address = i;
        writedata = C_DATA[i];
        chipselect = 1;
        write_n = 0; // write operation
        @(posedge clk);
    end

    $display("Reading from memory");
    for(i=0;i<MEM_SIZE;i=i+1)begin
        address = i;
        chipselect = 1; 
        write_n = 1; 
        @(posedge clk);// Read operation
        @(posedge clk);// Control
        if(C_DATA[i] != readdata) begin
            $display("AVALON: RW Not equal, time: %t, read: %h, real: %h",$time,C_DATA[i],readdata);
            mismatch_av = 1;
        end
        if(C_DATA[i] != wb_rdt) begin
            $display("WISHBONE: RW Not equal, time: %t, read: %h, real: %h",$time,C_DATA[i],readdata);
            mismatch_wb = 1;
        end
    end
    if((mismatch_wb | mismatch_av))
        $display("FAILED");
    else
        $display("SUCCESS");
    $stop;
end

endmodule