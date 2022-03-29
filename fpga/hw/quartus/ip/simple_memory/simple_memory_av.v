// synthesis translate_off
`timescale 1ns / 1ps
// synthesis translate_on

module simple_memory_av #(
    parameter MEM_SIZE = 16
)(
    input wire [ 31: 0] address,
    input wire          chipselect,
    input wire          clk,
    input wire          reset_n,
    input wire          write_n,
    input wire [ 31: 0] writedata,
    output reg [ 31: 0] readdata
);

reg [ 31: 0] mem[MEM_SIZE-1:0];

always @(posedge clk or negedge reset_n)
begin
    if (reset_n == 0)
        mem[0] <= 0;
    else begin
        readdata <= mem[address];
        if (chipselect && ~write_n && (address<=(MEM_SIZE-1)))
            mem[address] <= writedata;
    end
end

endmodule