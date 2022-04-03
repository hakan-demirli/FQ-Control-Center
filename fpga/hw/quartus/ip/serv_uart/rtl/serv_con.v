// synthesis translate_off
`timescale 1ns / 1ps
// synthesis translate_on

module serv_con #(
    parameter MEM_SIZE = 16
)(
    //AVALON INTERFACE
    input wire [ 3: 0] address,
    input wire          chipselect,
    input wire          reset_n,
    input wire          write_n,
    input wire [ 31: 0] writedata,
    output reg [ 31: 0] readdata,

    //WISHBONE INTERFACE
    input wire          i_wb_clk,
    input wire          i_wb_rst,
    input wire [  3: 0] i_wb_adr,
    input wire [ 31: 0] i_wb_dat,
    input wire          i_wb_we,
    input wire          i_wb_stb,
    output reg [ 31: 0] o_wb_rdt,
    output reg          o_wb_ack,

    //SERV CONTROL INTERFACE
    output reg [ 7: 0] serv_con
);  
    reg [31:0] mem_wb[MEM_SIZE-1:0];

    always @(posedge i_wb_clk or negedge reset_n)
    begin
        if (reset_n == 0)
            serv_con <= 0;
        else begin
            readdata <= mem_wb[address];
            if (chipselect && ~write_n && (address==4'b0))
                serv_con <= writedata;
        end
    end

    always @(posedge i_wb_clk) begin
        if (i_wb_rst) begin
            o_wb_ack <= 1'b0;
        end else begin
            o_wb_ack <= i_wb_stb & !o_wb_ack;
            o_wb_rdt <= mem_wb[i_wb_adr];
            if(i_wb_stb & i_wb_we)
                mem_wb[i_wb_adr] <= i_wb_dat;
        end
    end
endmodule