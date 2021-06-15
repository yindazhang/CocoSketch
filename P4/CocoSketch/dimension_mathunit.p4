/* -*- P4_14 -*- */
#ifdef __TARGET_TOFINO__
#include <tofino/constants.p4>
#include <tofino/intrinsic_metadata.p4>
#include <tofino/primitives.p4>
//Include the blackbox definition
#include <tofino/stateful_alu_blackbox.p4>
#else
#warning This program is intended for Tofino P4 architecture only
#endif

#define COUNTER_LEN 65536
#define COUNTER_HASH_LEN 16

/*--*--* HEADERS *--*--*/
header_type Ethernet {
    fields {
        dstAddr : 48;
        srcAddr : 48;
        etherType : 16;
    }
}

header_type Ipv4 {
    fields {
        version : 4;
        ihl : 4;
        diffserv : 8;
        totalLen : 16;
        identification : 16;
        flags : 3;
        fragOffset : 13;
        ttl : 8;
        protocol : 8;
        hdrChecksum : 16;
        srcAddr : 32;
        dstAddr : 32;
    }
}

header_type MyFlow {
    // header_type for parsing flow packets
    fields {
        id: 32;
    }
}

header Ethernet ethernet;
header Ipv4 ipv4;
header MyFlow myflow;

header_type user_metadata_t {
    fields {
        tstamp : 32;
        count: 32;
        rng: 32;
        cond: 32;
    }
}
metadata user_metadata_t md;

/*--*--* PARSERS *--*--*/
parser start {
    return parse_ethernet;
}

parser parse_ethernet {
    extract(ethernet);
    return parse_ipv4;
}

parser parse_ipv4 {
    extract(ipv4);
    return parse_myflow;
}

parser parse_myflow {
    extract(myflow);
    return ingress;
}

/******************************************************************************
 *
 * insertion
 *
 *****************************************************************************/
 //field
field_list flow_id_list{
    myflow.id;
}

field_list_calculation counter_hash {
    input { flow_id_list; }
    algorithm: random;
    output_width: COUNTER_HASH_LEN;
}
// register
register counter_count {
    width : 32;
    instance_count: COUNTER_LEN;
}

//table counter_count_table
blackbox stateful_alu counter_count_alu {
    reg: counter_count;

    update_lo_1_value: register_lo + 1;

    output_value : alu_lo;
    output_dst : md.count;
}
action check_counter_count() {
    counter_count_alu.execute_stateful_alu_from_hash(counter_hash);
}
table counter_count_table {
    actions { check_counter_count; }
    size : 1;
    default_action : check_counter_count();
}

//table random_number_table
action generate_random_number() {
    modify_field_rng_uniform(md.rng, 0, 65535);
}
table random_number_table {
    actions { generate_random_number; }
    size : 1;
    default_action : generate_random_number();
}

//table calc_cond_table
register num_32 {
   width : 32;
   instance_count: 1;
}
blackbox stateful_alu prog_64K_div_x {
    reg: num_32;

    update_lo_2_value : math_unit;
    output_value : alu_lo;
    output_dst : md.cond;

    math_unit_input : register_lo;
    math_unit_exponent_shift : 0;
    math_unit_exponent_invert : true;
    math_unit_output_scale : 9;
    math_unit_lookup_table : 68 73 78 85 93 102 113 128
                              0  0  0  0  0   0   0   0;
}
action calc_cond() {
    prog_64K_div_x.execute_stateful_alu(0);
    subtract(md.cond, md.cond, md.rng);
}
table calc_cond_table {
    actions { calc_cond; }
    size : 1;
    default_action: calc_cond();
}

//table calc_cond_table
register counter_ID {
    width : 32;
    instance_count: COUNTER_LEN;
}

blackbox stateful_alu counter_ID_alu {
    reg: counter_ID;

    update_lo_1_value: myflow.id;
}
action check_counter_ID() {
    counter_ID_alu.execute_stateful_alu_from_hash(counter_hash);
}
table counter_ID_table {
    actions { check_counter_ID; }
    size : 1;
    default_action: check_counter_ID();
}


/******************************************************************************
 *
 * Ingress
 *
 *****************************************************************************/

control ingress {
    apply(counter_count_table);
    apply(random_number_table);
    apply(calc_cond_table);
    if(md.cond < 65536)
    {
        apply(counter_ID_table);
    }
}

control egress {
}
