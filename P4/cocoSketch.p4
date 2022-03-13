#include<core.p4>
#if __TARGET_TOFINO__ == 2
#include<t2na.p4>
#else
#include<tna.p4>
#endif

#define COUNTER_LEN 65536
#define COUNTER_HASH_LEN 16

/* header definitions */
header Ethernet {
	bit<48> dstAddr;
	bit<48> srcAddr;
	bit<16> etherType;
}

header Ipv4{
	bit<4> version;
	bit<4> ihl;
	bit<8> diffserv;
    bit<16> total_len;
	bit<16> identification;
	bit<3> flags;
	bit<13> fragOffset;
	bit<8> ttl;
    bit<8> protocol;
	bit<16> checksum;
	bit<32> srcAddr;
	bit<32> dstAddr;
}  
header MyFlow{
	bit<32> id;
}

struct ingress_headers_t{
	Ethernet ethernet;
	Ipv4 ipv4;
	MyFlow myflow;
}

struct ingress_metadata_t{
	bit<32> count;
  	bit<16> rng;
	bit<32> cond;
}

struct egress_headers_t {}
struct egress_metadata_t {}

enum bit<16> ether_type_t {
    IPV4    = 0x0800,
    ARP     = 0x0806
}

enum bit<8> ip_proto_t {
    ICMP    = 1,
    IGMP    = 2,
    TCP     = 6,
    UDP     = 17
}

/* parser processing */
@pa_atomic("ingress", "metadata.rng")
@pa_atomic("ingress", "metadata.cond")
parser IngressParser(packet_in pkt,
	out ingress_headers_t hdr,
	out ingress_metadata_t metadata,
	out ingress_intrinsic_metadata_t ig_intr_md)
{
	state start{
		pkt.extract(ig_intr_md);
		pkt.advance(PORT_METADATA_SIZE);
		transition parse_ethernet;
	}

	state parse_ethernet{
		pkt.extract(hdr.ethernet);
        transition select((bit<16>)hdr.ethernet.etherType) {
            (bit<16>)ether_type_t.IPV4      : parse_ipv4;
            (bit<16>)ether_type_t.ARP       : accept;
            default : accept;
        }
	}

	state parse_ipv4{
		pkt.extract(hdr.ipv4);
        transition select(hdr.ipv4.protocol) {
            (bit<8>)ip_proto_t.ICMP             : accept;
            (bit<8>)ip_proto_t.IGMP             : accept;
            (bit<8>)ip_proto_t.TCP              : parse_myflow;
            (bit<8>)ip_proto_t.UDP              : parse_myflow;
            default : accept;
        }
	}

	state parse_myflow{
		pkt.extract(hdr.myflow);
		transition accept;
	}

}


/* ingress */
control Ingress(inout ingress_headers_t hdr,
		inout ingress_metadata_t meta,
		in ingress_intrinsic_metadata_t ig_intr_md,
		in ingress_intrinsic_metadata_from_parser_t ig_prsr_md,
		inout ingress_intrinsic_metadata_for_deparser_t ig_dprsr_md,
		inout ingress_intrinsic_metadata_for_tm_t ig_tm_md)
{
    CRCPolynomial<bit<32>>(coeff=0x04C11DB7,reversed=true, msb=false, extended=false, init=0xFFFFFFFF, xor=0xFFFFFFFF) crc32;

	Hash<bit<COUNTER_HASH_LEN>>(HashAlgorithm_t.CUSTOM, crc32) counter_hash;
	
	Register<bit<32>,bit<16>>(COUNTER_LEN) counter_count;

	RegisterAction<bit<32>,bit<16>,bit<32>>(counter_count) counter_count_alu={
		void apply(inout bit<32> register_data, out bit<32> alu_data){
			register_data = register_data + 1;
			alu_data = register_data;
		}
	};

	action check_counter_count(){
		meta.count = counter_count_alu.execute(counter_hash.get({hdr.myflow.id}));
	}

	table counter_count_table{
		actions = {
			check_counter_count;
		}
		size = 1;
		const default_action = check_counter_count();
	}	

	Random<bit<16>>() random_generator;

	action generate_random_number(){
		meta.rng = random_generator.get();
	}

	table random_number_table{
		actions = {
			generate_random_number;
		}
		size = 1;
		const default_action = generate_random_number();
	}

	Register<bit<32>,bit<1>>(1) num_32;

	MathUnit<bit<32>>(true,0,9,{68,73,78,85,93,102,113,128,0,0,0,0,0,0,0,0}) prog_64K_div_mu;

	RegisterAction<bit<32>,bit<1>,bit<32>>(num_32) prog_64K_div_x = {
		void apply(inout bit<32> register_data, out bit<32> mau_value){
			register_data = prog_64K_div_mu.execute(meta.count);
            mau_value = register_data;
		}
	};
	
	action calc_cond_pre(){
		meta.cond = prog_64K_div_x.execute(0);
	}

	table calc_cond_table_pre{
		actions = {
			calc_cond_pre;
		}
		size = 1;
		const default_action = calc_cond_pre();
	}
	 
	action calc_cond(){
		meta.cond = (bit<32>)meta.rng - meta.cond;
	}

	table calc_cond_table{
		actions = {
			calc_cond;
		}
		size = 1;
		const default_action = calc_cond();
	}

	Register<bit<32>,bit<16>>(COUNTER_LEN) counter_ID;

	RegisterAction<bit<32>,bit<16>,bit<32>>(counter_ID) counter_ID_alu = {
		void apply(inout bit<32> register_data){
			register_data = hdr.myflow.id;
		}
	};

	action check_counter_ID(){
		counter_ID_alu.execute(counter_hash.get({hdr.myflow.id}));
	}

	table counter_ID_table{
		actions = {
			check_counter_ID;
		}
		size = 1;
		const default_action = check_counter_ID();
	}


	/* ingress processing*/
	apply{
        if (hdr.myflow.isValid()) {
		    counter_count_table.apply();
		    random_number_table.apply();
		    calc_cond_table_pre.apply();
		    calc_cond_table.apply();
		    if(meta.cond < 65536){
		    	counter_ID_table.apply();
		    }
        }
	}
}

control IngressDeparser(packet_out pkt,
	inout ingress_headers_t hdr,
	in ingress_metadata_t meta,
	in ingress_intrinsic_metadata_for_deparser_t ig_dprtr_md)
{
	apply{
		pkt.emit(hdr);
	}
}


/* egress */
parser EgressParser(packet_in pkt,
	out egress_headers_t hdr,
	out egress_metadata_t meta,
	out egress_intrinsic_metadata_t eg_intr_md)
{
	state start{
		pkt.extract(eg_intr_md);
		transition accept;
	}
}

control Egress(inout egress_headers_t hdr,
	inout egress_metadata_t meta,
	in egress_intrinsic_metadata_t eg_intr_md,
	in egress_intrinsic_metadata_from_parser_t eg_prsr_md,
	inout egress_intrinsic_metadata_for_deparser_t eg_dprsr_md,
	inout egress_intrinsic_metadata_for_output_port_t eg_oport_md)
{
	apply{}
}

control EgressDeparser(packet_out pkt,
	inout egress_headers_t hdr,
	in egress_metadata_t meta,
	in egress_intrinsic_metadata_for_deparser_t eg_dprsr_md)
{
	apply{
		pkt.emit(hdr);
	}
}


/* main */
Pipeline(IngressParser(),Ingress(),IngressDeparser(),
EgressParser(),Egress(),EgressDeparser()) pipe;

Switch(pipe) main;
