module UART_Rx(
    input clk,
    input RST,
    input Rx,
    output logic frame_ready,
    output logic [7:0] frame);

    logic [2:0] bit_counter;

    typedef enum {IDLE, RECEIVE} STATES;
    STATES NS, PS;

    always_ff @ (posedge CLK)
    begin
       if (RST)
           PS <= IDLE;
       else
           PS <= NS;
    end

    always_comb
    begin
        //initialize all outputs to zero
        frame = 0; bit_counter = 0;
        case (PS)
            IDLE: begin
                if (Rx == 1)
                    NS = IDLE;
                else if (Rx == 0) begin
                    NS = RECEIVE;
                    bit_counter = 0;
                end
            end
            RECEIVE: begin
                if (bit_counter >= 7)
                    NS = IDLE;
                    frame_ready = 1;
                else NS = RECEIVE;
                if (bit_counter < 1)
                    frame = Rx;
                else frame = {Rx, frame};
                bit_counter = bit_counter + 1;
            end
            default: NS = IDLE;

        endcase
    end

endmodule
