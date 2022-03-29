 /*
 Taken from: https://github.com/olofk/serv
 
 ****************************************
 Change_log:
 -  13_07_2021__13_44:
	By :https://github.com/hakan-demirli
		o_wb_gpio_adr variable created to increase the number of GPIO pins
		ADR_WIDTH_GPIO parameter created to change the width of the gpio_adr
      NUM_GPIO parameter created to change the number of gpio registers
		Size of the GPIO registers are increased
		Info:
			GPIO_BASE address is 0x40000000
			There are 8 gpio registers by default, address offset is four
			first gpio address = 0x4000_0004
			...
			seventh gpio address = 0x4000_0018
			eighth gpio address = 0x4000_001C
			To Change the number of Gpio pins edit: ADR_WIDTH_GPIO of servant_mux.v and servant_gpio.v
 ****************************************
 */

`default_nettype none

module servant_gpio
  #(parameter NUM_GPIO = 8,
    parameter ADR_WIDTH_GPIO = 3)
  (input wire i_wb_clk,
   input wire i_wb_dat,
   input wire i_wb_we,
   input wire i_wb_cyc,
   input wire [ADR_WIDTH_GPIO-1:0] i_wb_gpio_adr,
   output reg o_wb_rdt,
   output reg [(NUM_GPIO-1):0] o_gpio);
   
   reg [(NUM_GPIO-1):0] enable_gpio_in_1;
   wire [(NUM_GPIO-1):0] enable_gpio_in_0;
   
   //assign  enable_gpio_in_0 = i_wb_cyc ? (8'b0000_0001 << i_wb_gpio_adr) : (8'b0000_0000);
	assign  enable_gpio_in_0 = i_wb_cyc ? ({{(NUM_GPIO-1){1'b0}}, 1'b1} << i_wb_gpio_adr) : {{(NUM_GPIO){1'b0}}};
	
   
   integer i;
   
   always @* begin
     for(i=0; i<(NUM_GPIO); i=i+1) begin
       enable_gpio_in_1[i] = i_wb_we & enable_gpio_in_0[i];
     end
   end
   
   always @(posedge i_wb_clk) begin
     o_wb_rdt <= o_gpio[i_wb_gpio_adr];
     for(i=0; i<(NUM_GPIO); i=i+1) begin
       if (enable_gpio_in_1[i]) begin
         o_gpio[i] <= i_wb_dat;
       end
     end
   end
   
endmodule
