module crc32_64bit_gen(
  input 			     clk			  , 	//system reset
	input 			     reset_n		,	  //reset signal, active low
	input [63:0] 	   data		    ,	  //insert element
	input 			     datavalid	,	  //element valid
	output[31:0] 	   checksum	  ,	  //the CRC32 result
	output 	reg		     crcvalid		    //the CRC32 result valid signal
);

wire [31:0] nextCRC32_D64 	;
wire [63:0] data_tmp		    ;
reg [31:0] 	crc_result		  ;


reg  [31:0] checksum_tmp 	  ;
/************************************code**********************************/ 
assign data_tmp = {data[56],data[57],data[58],data[59],data[60],data[61],data[62],data[63],
                   data[48],data[49],data[50],data[51],data[52],data[53],data[54],data[55],
                   data[40],data[41],data[42],data[43],data[44],data[45],data[46],data[47],
                   data[32],data[33],data[34],data[35],data[36],data[37],data[38],data[39],
                   data[24],data[25],data[26],data[27],data[28],data[29],data[30],data[31],
                   data[16],data[17],data[18],data[19],data[20],data[21],data[22],data[23],
                   data[8],data[9],data[10],data[11],data[12],data[13],data[14],data[15],
                   data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7]
                   };

always @ (posedge clk or negedge reset_n)
begin
    if (~reset_n) begin
        checksum_tmp <= 32'b0;
        crcvalid <= 1'b0;
    end
    else if(datavalid) begin
    	crcvalid <= 1'b1;
    	checksum_tmp <= nextCRC32_D64;
    end
    else begin
    	checksum_tmp <= 32'b0;
        crcvalid <= 1'b0;
    end
end

assign checksum = ~{checksum_tmp[24],checksum_tmp[25],checksum_tmp[26],checksum_tmp[27],checksum_tmp[28],checksum_tmp[29],checksum_tmp[30],checksum_tmp[31],
                    checksum_tmp[16],checksum_tmp[17],checksum_tmp[18],checksum_tmp[19],checksum_tmp[20],checksum_tmp[21],checksum_tmp[22],checksum_tmp[23],
                    checksum_tmp[8],checksum_tmp[9],checksum_tmp[10],checksum_tmp[11],checksum_tmp[12],checksum_tmp[13],checksum_tmp[14],checksum_tmp[15],
                    checksum_tmp[0],checksum_tmp[1],checksum_tmp[2],checksum_tmp[3],checksum_tmp[4],checksum_tmp[5],checksum_tmp[6],checksum_tmp[7]};

always @ (posedge clk or negedge reset_n)
begin
    if (~reset_n)
        crc_result <= 32'hffffffff;
    else if(datavalid)
        crc_result <= 32'hffffffff;
    else if(datavalid)
        crc_result <= nextCRC32_D64;
end

CRC32_D64  CRC32_D64_u  (.data_in(data_tmp[63:00]),.crc_last(crc_result),.crc_out(nextCRC32_D64 ));   
    
endmodule
