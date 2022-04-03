// synthesis translate_off
`timescale 1ns / 1ps
// synthesis translate_on

module subservient_wrapped_av_wrapped (
    output wire readdatavalid,
    input wire  [ 11: 0] address,
    input wire           chipselect,
    input wire           clk,
    input wire           reset_n,
    input wire           write_n,
    input wire  [ 31: 0] writedata,
    output wire [ 31: 0] readdata,
    output wire [ 7: 0] q,
    output wire          o_tx,
    input wire           i_rx
);
    wire chipselect_serv;
    wire chipselect_con;

    assign chipselect_serv = address[11] ? 0 : chipselect;
    assign chipselect_con = address[11] ? chipselect : 0;

    subservient_wrapped subw
    (
    // Clock & reset
    .wb_clk_i (clk),
    .wb_rst_i (!reset_n),

    //Debug interface
    .wbs_adr_i ({20'b0,address}),
    .wbs_dat_i (writedata),
    .wbs_sel_i (4'b1111),
    .wbs_we_i  (~write_n),
    .wbs_stb_i (chipselect_serv && ~write_n),
    .wbs_dat_o (),
    .wbs_ack_o (readdatavalid),

    //AVALON INTERFACE
    .address(address),
    .chipselect(chipselect_con),
    .reset_n(reset_n),
    .write_n(write_n),
    .writedata(writedata),
    .readdata(readdata),
    
    // External I/O
    .serv_con (q),
    .o_tx(o_tx),
    .i_rx(i_rx)
    );

endmodule