# Implementation of IEEE 802.11bf Sub-7GHz MAC in ns-3 #
## Introduction

  This repository contains implementation and extenstion parts of IEEE 802.11bf Wi-Fi Sensing to ns-3 version ns-3.40. As the standard IEEE 802.11bf is currently developing, this repository provides ns-3 implementation to simulate and evaluate the sensing functionality in Wi-Fi technolgy. 
  
## Main Features
Key features of the implementation are as follows:
  - Implementation of Wi-Fi sensing in sub-7 GHz
     - TB Sensing Measurement Instance

To demonstrate the implementation please refers to the file in [here](/examples/wireless/wifi-bf-network.cc). The simulation code includes setup of the network using IEEE 802.11bf standard and several scenario to evaluate its performance and capabilities. The output from the simulation code are mainly one of sensing KPI, namely the sensing latency. Following parameters are configurable for the simulation: 

| Name | Description |
| --- | --- |
| Network Structure | Number of AP and stations |
| Network layout | simple and 3GPP indoor office layout | 
| Sensing Parameters | frequency, bandwidth, CFP , instance number, sensing transmission types, sensing priorities |

## Keywords 
- WiFi Sensing, MU-MIMO, PCF, Channel Sounding, ns3

### Usage example 
- Shell script for output: output_run_info.sh
    - Build the ns3: ./output_run_info.sh build (type: debug, optimized)
        - example: ./output_run_info.sh build optimized
    - Fast run: ./output_run_info.sh investigate (logging type: no_log, log_info, log_function) (parameter) (logging file name)
        - example: ./output_run_info.sh investigate no_log "--nBss=2" twoBss
    - Scenario run: ./output_run_info.sh (scenario name)
        - example: ./output_run_info.sh investigate multipleBssBf

## References

## Future Work

## License
This software is licensed under the terms of the GNU GPLv2, as like as ns-3. See the LICENSE file for more details.
