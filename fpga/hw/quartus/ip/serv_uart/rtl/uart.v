module uart (
    input wire [31:0] i_wb_adr,
    input wire i_wb_clk,
    input wire i_wb_rst,
    input wire [31:0] i_wb_dat,
    input wire i_wb_we,
    input wire i_wb_stb,
    output reg [31:0] o_wb_rdt,
    output reg o_wb_ack,
    output wire o_tx,
    input wire i_rx
);

    reg [15:0]prescalar;
    reg [4:0]cfg;
    wire busy;
    reg data_send;
    reg [7:0] tx_data;
    wire o_data_ready;
    wire [10: 0] o_data;

    uart_rx uart_rx_0(
        .i_clk(i_wb_clk),
        .i_rst(i_wb_rst),
        .i_prescalar(prescalar),
        .i_rx(i_rx),
        .i_cfg(cfg), 
        .o_data_ready(o_data_ready),
        .o_data(o_data)
    );

    uart_tx uart_tx_0(
        .i_clk(i_wb_clk),
        .i_rst(i_wb_rst),
        .i_prescalar(prescalar),
        .i_data_send(data_send),
        .i_data(tx_data),
        .i_cfg(cfg), 
        .o_busy(busy),
        .o_tx(o_tx)
    );

/*
memory map

0w: prescalar
1w: cfg
2r: busy
3w: data_send
4w: tx_data
5r: o_data_ready
6r: o_data
7r:
*/

    always @(posedge i_wb_clk) begin
        if (i_wb_rst) begin
            o_wb_ack <= 1'b0;
        end else begin
            o_wb_ack <= i_wb_stb & !o_wb_ack;
            case(i_wb_adr[2:0])
                3'd0: begin
                    if(i_wb_stb & i_wb_we)
                        prescalar <= i_wb_dat;
                end
                3'd1: begin
                    if(i_wb_stb & i_wb_we)
                        cfg <= i_wb_dat;
                end
                3'd2: begin
                    o_wb_rdt <= {31'd0,busy};
                end
                3'd3: begin
                    if(i_wb_stb & i_wb_we)
                        data_send <= i_wb_dat;
                end
                3'd4: begin
                    if(i_wb_stb & i_wb_we)
                        tx_data <= i_wb_dat;
                end
                3'd5: begin
                    o_wb_rdt <= o_data_ready;
                end
                3'd6: begin
                    o_wb_rdt <= {31'd0,o_data};
                end
                default: begin
                    o_wb_rdt <= 32'd0;
                end
            endcase
        end
    end

endmodule
