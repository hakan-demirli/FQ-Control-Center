/*
 * subservient_tb.v : Verilog testbench for the subservient SoC
 *
 * SPDX-FileCopyrightText: 2021 Olof Kindgren <olof.kindgren@gmail.com>
 * SPDX-License-Identifier: Apache-2.0
 */
`timescale 1ns/1ps
`default_nettype none
module subservient_wrapped_tb();

    parameter memfile = "hello.hex";
    parameter memsize = 1024;
    parameter with_csr = 0;
    parameter aw       = $clog2(memsize);

    reg clk = 1'b0;
    reg rst = 1'b1;

    reg         debug_mode;
    reg [31:0]  wb_dbg_adr;
    reg [31:0]  wb_dbg_dat;
    reg [3:0]   wb_dbg_sel;
    reg         wb_dbg_we;
    reg         wb_dbg_stb = 1'b0;
    wire [31:0] wb_dbg_rdt;
    wire        wb_dbg_ack;

    wire [37:0] q;

    always  #5 clk <= !clk;
    initial #62 rst <= 1'b0;


    reg [1023:0] firmware_file = memfile;
    integer  idx = 0;
    reg [7:0] 	 mem [0:memsize-1];

    task wb_dbg_write32(input [31:0] adr, input [31:0] dat);
    begin
     @ (posedge clk) begin
        wb_dbg_adr <= adr;
        wb_dbg_dat <= dat;
        wb_dbg_sel <= 4'b1111;
        wb_dbg_we  <= 1'b1;
        wb_dbg_stb <= 1'b1;
     end
     while (!wb_dbg_ack)
       @ (posedge clk);
     wb_dbg_stb <= 1'b0;
    end
    endtask

    reg [31:0] tmp_dat;
    integer    adr;
    reg [1:0]  bsel;

    initial begin
        $display("Setting debug mode");
        debug_mode <= 1'b1;

        $display("Writing %0s to SRAM", firmware_file);
        $readmemh(firmware_file, mem);

        repeat (10) @(posedge clk);

        //Write full 32-bit words
        while ((mem[idx] !== 8'bxxxxxxxx) && (idx < memsize)) begin
            adr = (idx >> 2)*4;
            bsel = idx[1:0];
            tmp_dat[bsel*8+:8] = mem[idx];
            if (bsel == 2'd3)
                wb_dbg_write32(adr, tmp_dat);
            idx = idx + 1;
        end

        //Zero-pad final word if required
        if (idx[1:0]) begin
            adr = (idx >> 2)*4;
            bsel = idx[1:0];
            if (bsel == 1) tmp_dat[31:8]  = 24'd0;
            if (bsel == 2) tmp_dat[31:16] = 16'd0;
            if (bsel == 3) tmp_dat[31:24] = 8'd0;
            wb_dbg_write32(adr, tmp_dat);
        end
        repeat (10) @(posedge clk);

        $display("Done writing %0d bytes to SRAM. Turning off debug mode", idx);
        debug_mode <= 1'b0;
   end

    initial begin
        forever begin
            @(negedge clk);
            case (q[37:36])
                2'b10:begin
                    #100;
                    $display("\nFAIL: Error bit is checked\n");
                    $fflush;
                    $finish;
                end
                2'b01:begin
                    #100;
                    $display("\PASSED: \\(^o^)/ \n");
                    $fflush;
                    $finish;
                end
            endcase
        end
    end
    initial begin
        //$dumpfile("subservient_alone.vcd");
        //$dumpvars(0, subservient_alone_tb);
    end

    subservient_wrapped dut
    (
    // Clock & reset
    .wb_clk_i (clk),
    .wb_rst_i (rst),

    //Debug interface
    .debug_mode_i (debug_mode),
    .wbs_adr_i (wb_dbg_adr),
    .wbs_dat_i (wb_dbg_dat),
    .wbs_sel_i (wb_dbg_sel),
    .wbs_we_i  (wb_dbg_we),
    .wbs_stb_i (wb_dbg_stb),
    .wbs_dat_o (wb_dbg_rdt),
    .wbs_ack_o (wb_dbg_ack),
    
    // External I/O
    .io_out (q)
    );
endmodule
