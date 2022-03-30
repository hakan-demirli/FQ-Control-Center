// synthesis translate_off
`timescale 1ns / 1ps
// synthesis translate_on

module subservient_wrapped_av_wrapped (
    input wire debug_mode_i,
    output wire wb_dbg_ack,
    input wire  [ 31: 0] address,
    input wire           chipselect,
    input wire           clk,
    input wire           reset_n,
    input wire           write_n,
    input wire  [ 31: 0] writedata,
    output wire [ 31: 0] readdata,
    output wire [ 37: 0] q,
    output wire          o_tx,
    input wire           i_rx
);

    subservient_wrapped subw
    (
    // Clock & reset
    .wb_clk_i (clk),
    .wb_rst_i (!reset_n),

    //Debug interface
    .debug_mode_i (debug_mode_i),
    .wbs_adr_i (address),
    .wbs_dat_i (writedata),
    .wbs_sel_i (4'b1111),
    .wbs_we_i  (~write_n),
    .wbs_stb_i (chipselect && ~write_n),
    .wbs_dat_o (readdata),
    .wbs_ack_o (wb_dbg_ack),
    
    // External I/O
    .o_gpio (q),
    .o_tx(o_tx),
    .i_rx(i_rx)
    );

endmodule