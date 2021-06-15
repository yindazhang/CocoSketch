# Open vSwitch Integration

## Environment
We have tested on
 - Open vSwitch 2.12.1
 - DPDK 18.11.10
 - Linux kernel version 3.10.0-1160.6.1.el7.x86_64

## How to install
 - suppose Open vSwitch source code is in ```$OVS_DIR```
 - copy ```ovs-dpif-netdev/\*.h``` to ```$OVS_DIR/lib/```
 - patch ```ovs-dpif-netdev/dpif-netdev.patch``` to ```dpif-netdev.c```
 - re-compile Open vSwitch and link it with DPDK

## How to start measurement
 - uncomment $SIGCOMM_MEASUREMENT$ macro in $OVS_DIR/lib/ringbuffer.h (line 21)
 - re-compile Open vSwitch and link it with DPDK
 - start Open vSwitch with DPDK, set the number of pmd thread = $d$ ($d$=1,2,4,8,...)
 - Open user_reader/user_reader.cpp
	- Set #MAX_RINGBUFFER_NUM to $d + 1$
	- `$ make`
	- run `./user_reader`