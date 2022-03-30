`default_nettype none

module ff_ram #(parameter memsize = 1024,
                parameter aw = $clog2(memsize))
              (
            `ifdef USE_POWER_PINS
                inout vccd1,	// User area 1 1.8V supply
                inout vssd1,	// User area 1 digital ground
            `endif
            input wire reset,
            input wire clk0,
            input wire clk1,
            input wire csb0,
            input wire [aw-1:0] addr0,
            input wire [7:0] din0,
            input wire csb1,
            input wire [aw-1:0] addr1,
            output reg [7:0] dout1);
    
    parameter DATA_WIDTH = 8 ;
    parameter ADDR_WIDTH = aw ;
    
    // Port 0
    reg  csb0_reg;
    reg [ADDR_WIDTH-1:0]  addr0_reg;
    reg [DATA_WIDTH-1:0]  din0_reg;
    
    // Port 1
    reg  csb1_reg;
    reg [ADDR_WIDTH-1:0]  addr1_reg;
    
    // Memory
    reg [DATA_WIDTH-1:0] mem[0:memsize-1];
    
    integer i;
    
    always @(posedge clk0) begin
        csb0_reg  = csb0;
        addr0_reg = addr0;
        din0_reg  = din0;
        
        if (reset) begin
            csb0_reg  = 0;
            addr0_reg = 0;
            din0_reg  = 0;
        end
    end
    
    always @(posedge clk0) begin
        csb1_reg  = csb1;
        addr1_reg = addr1;
        
        if (reset) begin
            csb1_reg  = 0;
            addr1_reg = 0;
        end
    end
    
    always @ (negedge clk0)
        begin : MEM_WRITE0
        // Memory write block port 0
        if (!csb0_reg) begin
            mem[addr0_reg] = din0_reg;
        end
    
        if (reset) begin
            mem[0] = 0;
        end
    end
    
    // Memory Read Block Port 1
    // Read Operation : When web1 = 1, csb1 = 0
    always @ (negedge clk0)
        begin : MEM_READ1
        if (!csb1_reg) begin
            dout1 <= mem[addr1_reg];
        end
    
        if (reset) begin
            dout1 <= 0;
        end
    end
    
endmodule

`default_nettype wire