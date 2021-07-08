# CocoSketch: High-Performance Sketch-based Measurement over Arbitrary Partial Key Query

## Introduction

**CocoSketch** is a sketch-based algorithm supporting **arbitrary partial key query**. It casts arbitrary partial key queries to the subset sum estimation problem and makes the theoretical tools for subset sum estimation practical. To realize desirable resource-accuracy tradeoffs in software and hardware platforms, we propose two techniques: (1) stochastic variance minimization to significantly reduce per-packet update delay, and (2) removing circular dependencies in the per-packet update logic to make the implementation hardware-friendly. We implement CocoSketch on four popular platforms (CPU, Open vSwitch, P4, and FPGA) and show that compared to baselines that use traditional single-key sketches, CocoSketch improves average packet processing throughput by 27.2× and accuracy by 10.4× when measuring six flow keys.

[*Our paper*](https://yindazhang.github.io/files/CocoSketch.pdf) will appear in SIGCOMM 2021.

## About this repo

- `CPU`: CocoSketch and other algorithms implemented on CPU
- `OVS`: CocoSketch sketch implemented on OVS
- `FPGA`: CocoSketch sketch implemented on FPGA
- `P4`: CocoSketch sketch implemented on P4
- more details can be found above four folders.

