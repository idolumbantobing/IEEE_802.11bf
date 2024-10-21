# IEEE_802.11bf
Implementation project of IEEE 802.11bf in ns-3

## Keywords :
- WiFi Sensing, MU-MIMO, PCF, Channel Sounding, ns3

### Tips : 
- Shell script for output: output_run_info.sh
    - Build the ns3: ./output_run_info.sh build (type: debug, optimized)
        - example: ./output_run_info.sh build optimized
    - Fast run: ./output_run_info.sh investigate (logging type: no_log, log_info, log_function) (parameter) (logging file name)
        - example: ./output_run_info.sh investigate no_log "--nBss=2" twoBss
    - Scenario run: ./output_run_info.sh (scenario name)
        - example: ./output_run_info.sh investigate multipleBssBf
