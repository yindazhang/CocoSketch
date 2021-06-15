### Introduction
This folder includes files that help user read packet headers from ring buffer shared by OVS-datapath.

#### file description
 - **util.h**: some macro definition
 - **tuple.h**: define the structure of packet header
 - **ringbuffer.h**: define the structure of ring-buffer
 - **ringbuffer_user.cpp**: implement some functions on ringbuffer
 - **user_reader.cpp**: the main cpp file

#### how to measurement
The current **user_reader.cpp** only extract packet headers from ringbuffer. You have to modify this file to implement your measurement algorithms.
 - line 71: create your measurement structure here
 - line 86: insert the packet header (variable $t$) into your structure


#### how to run
 - root: ```sudo su```
 - compile: ```make```
 - execute: ```./user_reader```

#### result show
The ```user_reader``` program will tell you how many packets are processed per seconds, so you can see the throughput.
