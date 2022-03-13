import json
import sys
import binascii


ID_table = bfrt.cocoSketch.pipe.Ingress.counter_ID
ID_text = ID_table.dump(json=True, from_hw=True)
IDs = json.loads(ID_text)

counter_table = bfrt.cocoSketch.pipe.Ingress.counter_count
counter_text = counter_table.dump(json=True, from_hw=True)
counters = json.loads(counter_text)

with open('/root/cocosketch/input.txt', 'r') as f:
    lines = f.readlines()
    for line in lines:
        print('Query ID: ' + line.strip())
        flow_id = int(line.strip())
        hash_code = binascii.crc32(flow_id.to_bytes(4, byteorder='big'), 0x00000000) & 0xFFFFFFFF
        pos = hash_code % 65536
        
        if IDs[pos]['data']['Ingress.counter_ID.f1'][0] == flow_id:
            print('pipe 0: ' + str(counters[pos]['data']['Ingress.counter_count.f1'][0]))
        else:
            print('pipe 0: 0')

        if IDs[pos]['data']['Ingress.counter_ID.f1'][1] == flow_id:
            print('pipe 1: ' + str(counters[pos]['data']['Ingress.counter_count.f1'][1]))
        else:
            print('pipe 1: 0')



