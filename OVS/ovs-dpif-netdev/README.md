### Introduction
This folder includes files that extract packet headers from ovs-datapath to ring buffers.

#### file description
 - **rb_util.h**: some macro definition
 - **tuple.h**: define the structure of packet header
 - **ringbuffer.h**: define the structure of ring-buffer
 - **dpif-netdev.patch**: patch file for dpif-netdev.c

#### how to make
 - patch dpif-netdev.patch to lib/dpif-netdev.c
 - recompile OVS
 	- make && make install
