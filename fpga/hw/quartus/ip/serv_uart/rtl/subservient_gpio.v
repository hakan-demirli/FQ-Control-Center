/*
 * subservient_gpio.v : Single-bit GPIO for the subservient SoC
 *
 * SPDX-FileCopyrightText: 2021 Olof Kindgren <olof.kindgren@gmail.com>
 * SPDX-License-Identifier: Apache-2.0
 */

module subservient_gpio
(   input wire i_wb_clk,
    input wire i_wb_rst,
    input wire i_wb_adr,
    input wire [31:0] i_wb_dat,
    input wire i_wb_we,
    input wire i_wb_stb,
    output reg [31:0] o_wb_rdt,
    output reg o_wb_ack,
    output [37:0] o_gpio);

    reg [31:0] gpio_0;
    reg [5:0] gpio_1;
    assign o_gpio ={gpio_1,gpio_0};
    always @(posedge i_wb_clk) begin
        o_wb_ack <= i_wb_stb & !o_wb_ack;
        case(i_wb_adr)
                1'b0: begin
                    o_wb_rdt <= gpio_0;
                    if(i_wb_stb & i_wb_we)
                        gpio_0 <= i_wb_dat;
                end
                1'b1: begin
                    o_wb_rdt <= {26'b0,gpio_1};
                    if(i_wb_stb & i_wb_we)
                        gpio_1 <= i_wb_dat[5:0];
                end
        endcase
        if (i_wb_rst) begin
            o_wb_ack <= 1'b0;
            gpio_0   <= 32'b0;
            gpio_1   <= 6'b0;
        end
    end
endmodule
