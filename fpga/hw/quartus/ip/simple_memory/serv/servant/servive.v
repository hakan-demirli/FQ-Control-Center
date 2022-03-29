`default_nettype none
module servive(
    input wire 	   i_clk,
    input wire 	   i_rst_n,
    output wire [7:0]	   q
);
    parameter memfile = "demo_acc_adr.hex";
    parameter memsize = 12740;
    parameter NUM_GPIO = 8;
    parameter ADR_WIDTH_GPIO = 3;
    parameter reset_strategy = "MINI";
    parameter sim = 0;
    parameter with_csr = 1;

    wire      wb_clk;
    wire      wb_rst;

    servive_clock_gen clock_gen
    (.i_clk (i_clk),
    .i_rst (!i_rst_n),
    .o_clk (wb_clk),
    .o_rst (wb_rst));

    servant
    #(.memfile (memfile),
    .memsize (memsize),
    .ADR_WIDTH_GPIO(ADR_WIDTH_GPIO),
    .NUM_GPIO(NUM_GPIO),
    .reset_strategy(reset_strategy),
    .sim(sim),
    .with_csr(with_csr))
    servant
    (.wb_clk (wb_clk),
    .wb_rst (wb_rst),
    .q      (q));

endmodule
