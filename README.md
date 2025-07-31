compute_acceleration() – performance benchmark
----------------------------------------------

Description:
-------------
This benchmark measures the performance of the custom x86-64 assembly implementation
of `compute_acceleration()`, which calculates acceleration for a list of cars using
the formula:

    a = (Vf - Vi) / T

Where:
  - Vi, Vf are velocities in km/h (converted to m/s)
  - T is time in seconds
  - a is rounded to the nearest integer (m/s²)

Each input row contains three floats: Vi, Vf, and T.
Output is a corresponding array of integers.

Benchmark Setup:
----------------
- Processor: Ryzen 7 4700u
- OS: Windows
- Timing method: `QueryPerformanceCounter()` with microsecond precision
- Each matrix size tested over 1000 runs
- All output values consumed and checksummed to prevent dead-code elimination
- Timing excludes setup and verification

Results:
--------
| Rows      | Avg time/run (µs) | Time/element (ns) |
|-----------|-------------------|-------------------|
| 10        |       0.023       |     2.260         |
| 100       |       0.203       |     2.031         |
| 1,000     |       2.053       |     2.053         |
| 10,000    |      19.093       |     1.909         |
| 100,000   |     256.820       |     2.568         |

Checksum: 765000  (verifies output used)

Analysis:
---------
The time per element remains consistent across input sizes, averaging ~2 ns/element.
This matches expectations for scalar floating-point throughput on a modern CPU:

- `divss` (FP divide) is the bottleneck at ~7–8 cycles throughput
- Total loop body spans ≈ 8–10 cycles per row
- At 4.3 GHz (0.233 ns per cycle), that gives ~1.9–2.3 ns/row

The small dip at n = 10,000 reflects optimal instruction cache behavior and branch prediction.
At n = 100,000, L1/L2 cache pressure slightly increases latency.

Conclusion:
-----------
The `compute_acceleration()` function executes near the theoretical minimum for scalar FP code,
with timings fully consistent with modern CPU architecture limits. The profiling setup using
QPC with batched runs and full result consumption produces accurate, stable measurements.

<img width="1116" height="625" alt="image" src="https://github.com/user-attachments/assets/2d960dfa-e305-4c56-b9da-6ff133477a7d" />
<img width="1114" height="618" alt="image" src="https://github.com/user-attachments/assets/5c03c02f-3878-4c8e-97a4-fd93b77c8a53" />
