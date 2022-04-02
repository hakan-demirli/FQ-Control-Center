// synthesis translate_off
`timescale 1ns / 1ps
// synthesis translate_on

module serv_con (
    input wire           chipselect,
    input wire           clk,
    input wire           reset_n,
    input wire           write_n,
    input wire  [ 7: 0] writedata,
    output wire [ 7: 0] readdata,
    output wire [ 7: 0] serv_con
);

reg [ 7: 0] mem;

assign readdata = mem;
assign serv_con = mem;

always @(posedge clk or negedge reset_n)
begin
    if (reset_n == 0)
        mem <= 0;
    else begin
        if (chipselect && ~write_n)
            mem <= writedata;
    end
end

endmodule