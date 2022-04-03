`timescale 1ns/1ps
`default_nettype none
module subservient_wrapped_av_wrapped_tb();

    parameter memfile = "entry_point.hex";
    parameter memsize = 2048;
    parameter with_csr = 0;
    parameter aw       = $clog2(memsize);

    reg clk = 1'b0;
    reg reset_n = 1'b0;

    wire        wb_dbg_ack ;

    reg  [  3: 0] wb_dbg_sel;
    reg  [ 31: 0] address;
    reg           chipselect = 0;
    reg           chipselect_sc = 0;
    reg           write_n = 1;
    reg  [ 31: 0] writedata;
    wire [ 31: 0] readdata;

    wire [7:0] q;

    always  #5 clk <= !clk;
    initial #62 reset_n <= 1'b1;


    reg [1023:0] firmware_file = memfile;
    integer  idx = 0;
    reg [7:0] 	 mem [0:memsize-1];

    task wb_dbg_write32(input [31:0] adr, input [31:0] dat);
    begin
        @ (posedge clk) begin
            address <= adr;
            writedata <= dat;
            wb_dbg_sel <= 4'b1111;
            write_n  <= 1'b0;
            chipselect <= 1'b1;
        end
        while (!wb_dbg_ack)
        @ (posedge clk);
        chipselect <= 1'b0;
    end
    endtask

    reg [31:0] tmp_dat;
    integer    adr;
    reg [1:0]  bsel;

    initial begin
        $display("Setting debug mode");
        address <= 12'b1000_0000_0000;
        writedata <= {24'b0,8'hFF};
        wb_dbg_sel <= 4'b1111;
        write_n  <= 1'b0;
        chipselect <= 1'b1;

        repeat (10) @(posedge clk);

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
        address <= 12'b1000_0000_0000;
        writedata <= {24'b0,8'h00};
        wb_dbg_sel <= 4'b1111;
        write_n  <= 1'b0;
        chipselect <= 1'b1;
        repeat (10) @(posedge clk);
        chipselect <= 1'b0;
    end

    initial begin
        // access SRAM directly while serv is in the middle of something
        #1000000;
        $display("Setting debug mode for DMA");
        chipselect_sc = 1;
        write_n = 0;
        writedata <= 1; // debug mode 1
        repeat (40) @(posedge clk);
        chipselect_sc = 0;
        write_n = 1;

        // read dma mem
        address <= 32'h0000_07C0;
        repeat (64) @(posedge clk);
        $display("current dma_mem: %h",readdata);

        address <= 32'h0000_07C0;
        writedata <= 32'hAAAA_FFFF;
        wb_dbg_sel <= 4'b1111;
        chipselect <= 1'b1;
        repeat (5) @(posedge clk);
        write_n  <= 1'b0;
        while (!wb_dbg_ack)
        @ (posedge clk);
        chipselect <= 1'b0;
        write_n  <= 1'b1;
        repeat (64) @(posedge clk);
        $display("edited dma_mem");

        // read dma mem
        address <= 32'h0000_07C0;
        repeat (64) @(posedge clk);
        $display("current dma_mem: %h",readdata);

        repeat (1000) @(posedge clk); // do a long lasting job

        chipselect_sc = 1;
        write_n = 0;
        writedata <= 0; // debug mode 0
        repeat (64) @(posedge clk);
        chipselect_sc = 0;
        write_n = 1;
        $display("debug mode sabotage off");
    end

    initial begin
        forever begin
            @(negedge clk);
            case (readdata[7:0])
                8'b1000_0000:begin
                    #100;
                    $display("\nFAIL: Error bit is checked\n");
                    $fflush;
                    $finish;
                end
                8'b0000_1000:begin
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


    wire uart_echo;
    uart_decoder uad (uart_echo);
    
    wire debug_mode;
    subservient_wrapped_av_wrapped swaw (
        .wb_dbg_ack(wb_dbg_ack),
        .debug_mode_i(debug_mode),
        .address(address),
        .chipselect(chipselect),
        .clk(clk),
        .reset_n(reset_n),
        .write_n(write_n),
        .writedata(writedata),
        .readdata(readdata),
        .q(q),
        .o_tx(uart_echo),
        .i_rx(uart_echo)
    );
endmodule


module uart_decoder
  #(parameter BAUD_RATE = 115200)
   (input wire rx);

   localparam T = 1000000000/BAUD_RATE;

   integer i;
   reg [7:0] ch;

   initial forever begin
      @(negedge rx);
      #(T/2) ch = 0;
      for (i=0;i<8;i=i+1)
	#T ch[i] = rx;
      $write("%c",ch);
      $fflush;
   end
endmodule