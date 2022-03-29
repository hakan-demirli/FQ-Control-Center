`default_nettype none
module servant_tb;

   parameter memfile = "zephyr_hello.hex";
   parameter memsize = 42192;
   parameter sim = 1;
   parameter with_csr = 1;
   parameter NUM_GPIO = 8;
   parameter ADR_WIDTH_GPIO = 3;
   parameter reset_strategy = "MINI";

   reg wb_clk = 1'b0;
   reg wb_rst = 1'b1;

   wire [NUM_GPIO-1:0]q;

   always  #31 wb_clk <= !wb_clk;
   initial #62 wb_rst <= 1'b0;

   uart_decoder #(57600) uart_decoder (q[0]);

   servant_sim
	#( .memfile  (memfile),
      .memsize  (memsize),
      .sim      (sim),
      .with_csr (with_csr),
	   .NUM_GPIO (NUM_GPIO),
	   .ADR_WIDTH_GPIO(ADR_WIDTH_GPIO),
	   .reset_strategy(reset_strategy))
   dut(wb_clk, wb_rst, q);

endmodule
