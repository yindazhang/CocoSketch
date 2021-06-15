`timescale 1ns / 1ps


module CocoSketch(
	input 			sys_clk	,		//system clock
	input 			rst_n 	,		//reset signal,active low
	input [31:0] 	e 		,		//element to insert
	input 			e_valid			//element valid signal
    );

	parameter LENGTH = 64 * 1024;
	//parameter LENGTH = 32 ;


	wire[31:0] hash							;	//the hash of e
	wire 	   hash_valid 					;	//hash valid

	wire[31:0] random_num 					;	//the random number

	//generate a random number using the current time
	reg [63:0] timer 						;	//timer register

	//the counter ram signal
	wire[31:0] counter_ram_douta			;  	//douta of counter ram port a
	
	reg 	   counter_ram_web				;   //wea of counter ram port a
	reg [15:0] counter_ram_addrb			;  	//addra of counter ram port a
	reg [31:0] counter_ram_dinb				;   //dina of counter ram port a
	wire[31:0] counter_ram_doutb			;  	//douta of counter ram port a

	//the register to cache the counter_ram_douta
	reg [31:0] counter_ram_douta_reg		;

	//the ID ram signal
	reg 	   ID_ram_wea					;   //wea
	reg [15:0] ID_ram_addra					;  	//addra
	reg [31:0] ID_ram_dina					;   //dina
	wire[31:0] ID_ram_douta					;  	//douta

	//the cache register
	reg [31:0] e_reg 						;	//cache the e value
	reg [31:0] e_reg_0 						;
	reg [31:0] e_reg_1 						;
	reg [31:0] e_reg_2 						;
	reg [31:0] e_reg_3 						; 
	reg [31:0] e_reg_4 						;  
	reg [15:0] counter_ram_addra_0 			;
	reg [15:0] counter_ram_addra_1 			;
	reg [15:0] counter_ram_addra_2 			;
	reg [15:0] counter_ram_addra_3 			;
	reg [15:0] counter_ram_addra_4 			;
	reg 	   valid_0 						;	
	reg 	   valid_1 						;
	reg 	   valid_2 						;
	reg 	   valid_3 						;
	reg 	   valid_4 						;

	integer i ;

	//record the current time in timer register
	//cache the e value in e_reg
	always @ (posedge sys_clk or negedge rst_n) begin
	    if (~rst_n) begin
	        timer 					<= 64'b0;
	        counter_ram_douta_reg	<= 32'b0;
	    end
	    else begin
	    	timer <= timer + 64'b1;
	    	counter_ram_douta_reg <= counter_ram_douta;
	    end
	end

	//cache the address, e and valid signal 
	always @(posedge sys_clk or negedge rst_n)begin
		if (~rst_n) begin
			e_reg 				<= 32'b0				;
	        e_reg_0 			<= 32'b0				;
			e_reg_1 			<= 32'b0				;
			e_reg_2 			<= 32'b0				;
			e_reg_3 			<= 32'b0				;
			e_reg_4 			<= 32'b0				;
			counter_ram_addra_0	<= 16'b0 				;
			counter_ram_addra_1	<= 16'b0 				;
			counter_ram_addra_2	<= 16'b0 				;
			counter_ram_addra_3	<= 16'b0 				;
			counter_ram_addra_4	<= 16'b0 				;
			valid_0				<= 1'b0 				;	
			valid_1				<= 1'b0 				;
			valid_2				<= 1'b0 				;
			valid_3				<= 1'b0 				;
			valid_4				<= 1'b0 				;
	    end	
	    else begin	
	    	e_reg 				<= e 					;
	        e_reg_0 			<= e_reg				;
			e_reg_1 			<= e_reg_0				;
			e_reg_2 			<= e_reg_1				;
			e_reg_3 			<= e_reg_2				;
			e_reg_4 			<= e_reg_3				;
			counter_ram_addra_0	<= hash % LENGTH 		;
			counter_ram_addra_1	<= counter_ram_addra_0 	;
			counter_ram_addra_2	<= counter_ram_addra_1	;
			counter_ram_addra_3	<= counter_ram_addra_2	;
			counter_ram_addra_4	<= counter_ram_addra_3	;
			valid_0				<= hash_valid 			;	
			valid_1				<= valid_0 				;
			valid_2				<= valid_1 				;
			valid_3				<= valid_2 				;
			valid_4				<= valid_3 				;
	    end
	end


	always @(posedge sys_clk or negedge rst_n)begin
		if (~rst_n) begin
			counter_ram_web	 		<= 1'b0		;
			counter_ram_addrb		<= 16'b0	;
			counter_ram_dinb		<= 64'b0	;

			ID_ram_wea	 			<= 1'b0		; 
			ID_ram_addra			<= 16'b0	; 
			ID_ram_dina				<= 64'b0	;
		end
		else begin
			if (valid_4 == 1'b1)  begin
				counter_ram_web	 		<= 1'b1							;
				counter_ram_addrb		<= counter_ram_addra_4			;
				counter_ram_dinb		<= counter_ram_douta_reg + 64'b1	;
				if (random_num[31:0] * counter_ram_douta_reg[31:0] < 2**32) begin
					ID_ram_wea	 			<= 1'b1						; 
					ID_ram_addra			<= counter_ram_addra_4		; 
					ID_ram_dina				<= e_reg_4 					;
				end
				else begin
					ID_ram_wea	 			<= 1'b0						; 
					ID_ram_addra			<= counter_ram_addra_4					; 
					ID_ram_dina				<= e_reg_4 					;
				end
			end
			else begin
				counter_ram_web	 		<= 1'b0		;
				counter_ram_addrb		<= 16'b0	;
				counter_ram_dinb		<= 64'b0	;

				ID_ram_wea	 			<= 1'b0		; 
				ID_ram_addra			<= 16'b0	; 
				ID_ram_dina				<= 64'b0	;
			end			
		end
	end

	//generate the random number
	crc32_64bit_gen random_num_gen(
    .clk		(sys_clk			), 	//system reset
	.reset_n	(rst_n 				),	//reset signal, active low
	.data		(timer 				),	//insert element
	.datavalid	(1'b1     			),	//element valid
	.checksum	(random_num			),
	.crcvalid	(					)
	);


	//generate the hash
	crc32_64bit_gen hash_gen(
    .clk		(sys_clk			), 	//system reset
	.reset_n	(rst_n 				),	//reset signal, active low
	.data		({32'b0,e} 			),	//insert element
	.datavalid	(e_valid			),	//element valid
	.checksum	(hash				),
	.crcvalid	(hash_valid			)
	);

	//the ram to save the counter array
	ram_32b_64K counter_ram (
	  .clka	(sys_clk			),    	// input wire clka
	  .ena	(1'b1           	),      // input wire ena
	  .wea	(1'b0            	),      // input wire [0 : 0] wea
	  .addra(hash % LENGTH    	),  	// input wire [15 : 0] addra
	  .dina	(64'b0          	),    	// input wire [63 : 0] dina
	  .douta(counter_ram_douta	),  	// output wire [63 : 0] douta
	  .clkb	(sys_clk			),    	// input wire clkb
	  .enb	(1'b1            	),      // input wire enb
	  .web	(counter_ram_web	),      // input wire [0 : 0] web
	  .addrb(counter_ram_addrb	),  	// input wire [15 : 0] addrb
	  .dinb	(counter_ram_dinb	),    	// input wire [63 : 0] dinb
	  .doutb(counter_ram_doutb	)  		// output wire [63 : 0] doutb
	);

	//the ram to save the ID array
	ram_32b_64K ID_ram (
	  .clka(sys_clk),    // input wire clka
	  .ena(1'b1),      // input wire ena
	  .wea(ID_ram_wea),      // input wire [0 : 0] wea
	  .addra(ID_ram_addra),  // input wire [15 : 0] addra
	  .dina(ID_ram_dina),    // input wire [31 : 0] dina
	  .douta(ID_ram_douta),  // output wire [31 : 0] douta
	  .clkb(sys_clk),    // input wire clkb
	  .enb(1'b1),      // input wire enb
	  .web(1'b0),      // input wire [0 : 0] web
	  .addrb(16'b0),  // input wire [15 : 0] addrb
	  .dinb(32'b0),    // input wire [31 : 0] dinb
	  .doutb()  // output wire [31 : 0] doutb
	);


endmodule
