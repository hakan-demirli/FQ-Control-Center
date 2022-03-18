// synthesis translate_off
`timescale 1ns / 1ps
// synthesis translate_on

module simple_memory #(
    parameter ADDRESS_SIZE = 4 // how many unique pointers do you want
)(
    input [  ADDRESS_SIZE-1: 0] address,
    input          chipselect,
    input          clk,
    input          reset_n,
    input          write_n,
    input [ 31: 0] writedata,

    output [ 31: 0] readdata
);

reg   [ 31: 0] data_out[ADDRESS_SIZE-1:0];

always @(posedge clk or negedge reset_n)
begin
    if (reset_n == 0)
        data_out[0] <= 0;
    else if (chipselect && ~write_n && (address<=(ADDRESS_SIZE-1)))
        data_out[address] <= writedata;
end

assign readdata = data_out[address];

endmodule

