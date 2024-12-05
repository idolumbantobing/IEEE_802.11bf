# Implementation of IEEE 802.11bf Sub-7GHz MAC in ns-3 #
## Introduction

  This repository contains an ns-3 implementation and module extension of IEEE 802.11bf Wi-Fi Sensing version ns-3.40. As the standard IEEE 802.11bf is currently developing, this repository provides an ns-3 implementation to simulate and evaluate the sensing functionality in Wi-Fi technology based on the draft detailed in [this paper](https://ieeexplore.ieee.org/document/10467185)[^1]. Detailed information and description about the implementation and some theoretical background can be found in [this publication](https://publications.rwth-aachen.de/record/998149).

[^1]: T. Ropitault et al., "IEEE 802.11bf WLAN Sensing Procedure: Enabling the Widespread Adoption of WiFi Sensing," in IEEE Communications Standards Magazine, vol. 8, no. 1, pp. 58-64, March 2024, doi: 10.1109/MCOMSTD.0004.2200062.
keywords: {Location awareness;Sensors;Object recognition;Task analysis;Wireless fidelity;IEEE 802.11 Standard;Communication standards;Motion detection;Human activity recognition;Wireless LAN;Client-server systems}

## Main Features
The implementation of Wi-Fi sensing in sub-7 GHz currently only supports the TB sensing measurement instance. Complete implementation is still ongoing while waiting for the final draft of IEEE 802.11bf. The main features of the current implementation are as follows:
- Facilitates simulation of Wi-Fi sensing and extracts some performance parameters (e.g., sensing latency)
- Facilitates the coexistence simulation with other Wi-Fi legacy protocols

To demonstrate the implementation please refers to this [file](/examples/wireless/wifi-bf-network.cc). This simulation code includes the setup of the network using IEEE 802.11bf standard and several scenarios to evaluate its performance and capabilities. The output from the simulation code is mainly one of the sensing KPIs, i.e. sensing latency. The following parameters are configurable for the simulation: 

| Name | Description |
| --- | --- |
| Network Structure | Number of AP and stations |
| Network layout | simple and 3GPP indoor office layout | 
| Sensing Parameters | frequency, bandwidth, CFP , instance number, sensing transmission types, sensing priorities |

## Keywords 
- WiFi Sensing, MU-MIMO, PCF, Channel Sounding, ns3

## Usage example 
There is a [shell script](/output_run_info.sh) defined by the author to automate the output of the simulation code. Please refer to this file for understanding how to efficiently run the simulation. These are few tips for running the program:
- Build the ns3: ./output_run_info.sh build (type: debug, optimized)
  - example: ./output_run_info.sh build optimized
- Fast run: ./output_run_info.sh investigate (logging type: no_log, log_info, log_function) ("parameter") (logging file name)
  - example: ./output_run_info.sh investigate no_log "--nBss=2" twoBss
- Scenario run: ./output_run_info.sh (scenario name)
  - example: ./output_run_info.sh investigate multipleBssBf

Additional Python tools to extract the data and perform calculations can be found in [this folder](/Python\_tools).

## References
If you use this module in your research, please cite following:
<details>
<summary>Bibtex</summary>


```bibtex8
@MASTERSTHESIS{Lumbantobing:998149,
      author       = {Lumbantobing, Ido Manuel},
      othercontributors = {Petrova, Marina and Simic, Ljiljana and Keshtiarast, Navid
                          and Bishoyi, Pradyumna Kumar},
      title        = {{I}mplementation and evaluation of {IEEE} 802.11bf {MAC} in
                      ns-3},
      school       = {Rheinisch-Westfälische Technische Hochschule Aachen},
      type         = {Bachelorarbeit},
      address      = {Aachen},
      publisher    = {RWTH Aachen University},
      reportid     = {RWTH-2024-11170},
      pages        = {1 Online-Ressource: Illustrationen},
      year         = {2024},
      note         = {Veröffentlicht auf dem Publikationsserver der RWTH Aachen
                      University; Bachelorarbeit, Rheinisch-Westfälische
                      Technische Hochschule Aachen, 2024},
      cin          = {615720},
      ddc          = {621.3},
      cid          = {$I:(DE-82)615720_20201104$},
      typ          = {PUB:(DE-HGF)2},
      doi          = {10.18154/RWTH-2024-11170},
      url          = {https://publications.rwth-aachen.de/record/998149},
}
```


</details>

## Future Work
The author of this repository is actively extending the implementation and developing new features for the upcoming IEEE 802.11bf standards. Several agendas are as follows:
- Extending the implementation of Wi-Fi sensing based on the draft
- Following upcoming features in the ns-3 and maintaining the implementation version
- Incorporating machine learning to Wi-Fi MAC for Wi-Fi sensing adaptability

## License
This software is licensed under the terms of the GNU GPLv2, as like as ns-3. See the [LICENSE](/LICENSE) file for more details.
