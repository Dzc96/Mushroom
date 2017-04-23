  __  __             _
 |  \/  |           | |
 | \  / |_   _  ___ | |____  ___ ___   ___  _    _
 | |\/| | | | |/ _ \| |__  \/ _// _ \ / _ \| \  / |
 | |  | | |_| |\_|_ | |  | _ / | (_) | (_) | |\/| |
 |_|  |_|\__,_| _\_||_|  |_|_|  \___/ \___/|_|  |_|


## Mushroom: Concurrent B<sup>link</sup> Tree Index
[中文版 README](./README.md)

[![Author](https://img.shields.io/badge/Author-UncP-brightgreen.svg)](./LICENSE)
[![Version](https://img.shields.io/badge/Version-0.7.0-blue.svg)]()
[![Build](https://img.shields.io/badge/Build-Passing-brightgreen.svg)](https://travis-ci.org/UncP/Mushroom)
[![License](https://img.shields.io/badge/License-BSD-red.svg)](./LICENSE)

### Behold, the power of Mushroom!

### Feature
+ Concurrent B<sup>link</sup> Tree Index
+ Log-Structured Merge Tree
+ Prefix Compression (Lazy)
+ Two-Phase Hashing Page Manager
+ Mulit-Thread (Latch Manager, Thread Pool, Bounded Thread-Safe Queue)
+ Multi-Process (Shared Memory Mapping)

******

### B<sup>link</sup> Tree BenchFuck
|total key|key length| total size |     CPU    | Mem |
|:-------:|:--------:|:---------:|:--------------:|:----:|
|10 million | 16 bytes | 160 M| Intel i3 2.1 GHz 4 cores|4 G|

| Version | Multi-Thread | Multi-Process | Put | Get |       Improvements       |
|:------:|:-------:|:---------:|:-----------:|:------:|:--------------------------:|
| 0.1.0  |  No   |  No  |16.00    | \ ||
| 0.2.0  |  Yes  |  No  |12.32    | \ |   two-phase lock based concurrent index   |
| 0.2.1  |  Yes  |  No  |11.28    | \ |         latch manager optimization         |
| 0.3.0  |  Yes  |  No  |10.94    | \ | implement  prefix compaction, reducing B<sup>link</sup> tree memory about 9.1 % |
| 0.4.0  |  Yes  |  No  |11.44    | \ |  second-time mapping mulit-thread queue, reducing total program memory up to 50 %|
| 0.4.1  |  Yes  |  No  |\    | \ | merge latch manager and page manager，reduce 1 lock per operation |
| 0.4.2  |  Yes  |  No  |\    | \ | change the way B link tree root split |
| 0.4.3  |  Yes  |  No  |\   | \ |  new test strategy, threads conduct operations without going through the queue |
| 0.4.4  |  Yes  |  No  |\   | \ |  latch manager refactoring |
| 0.5.0  |  Yes  |  No  |11.70 / 9.00   | \ | fix **BUG**(atomic operation bug) that exists from 0.4.1 to 0.4.4 |
| 0.6.0  |  Yes  |  Yes  | \  | \ | multi-process supported, fix B<sup>link</sup> tree search **bug**|
| 0.6.1  |  Yes  |  No |18.69 / 13.04| \ | two-phase hashing page manager, implement lazy page allocation|
| 0.6.2  |  Yes  |  No |11.89 / 8.14| \ | reduce dependency on standard library, speed up compile, reduce program size about 42.1%|
| 0.6.4  |  Yes  |  No  |10.56 / 7.58|9.89 / 7.53|using posix spin lock, Optimize MushroomDB and BLinkTree structure|
| 0.7.0  |  Yes  |  No  | \ | \ |Log-Structured Merge Tree|

### Extreme Test
| Version | Sorting Time(s) |       Improvements       |
|:------:|:------:|:-----------------------------------:|
| 0.6.1 | 142.5 | 100 million keys, 16 bytes each, B<sup>link</sup> tree occupies memory about 2.7G |

### Other
+ Version 0.6.0 is the first stable version
