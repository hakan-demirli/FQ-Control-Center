// synthesis translate_off
`timescale 1ns / 1ps
// synthesis translate_on

module simple_memory_wb #(
    parameter MEM_SIZE = 16
)
(   input wire          i_wb_clk,
    input wire          i_wb_rst,
    input wire [ 31: 0] i_wb_adr,
    input wire [ 31: 0] i_wb_dat,
    input wire          i_wb_we,
    input wire          i_wb_stb,
    output reg [ 31: 0] o_wb_rdt,
    output reg          o_wb_ack);

    reg [31:0] mem[MEM_SIZE-1:0];

    always @(posedge i_wb_clk) begin
        if (i_wb_rst) begin
            o_wb_ack <= 1'b0;
        end else begin
            o_wb_ack <= i_wb_stb & !o_wb_ack;
            o_wb_rdt <= mem[i_wb_adr];
            if(i_wb_stb & i_wb_we)
                mem[i_wb_adr] <= i_wb_dat;
        end
    end
endmodule
