#!/bin/bash

export IDO_LOG_INFO="ApWifiMac=level_info|prefix_time:StaWifiMac=level_info|prefix_time:ChannelAccessManager=level_info|prefix_time:FrameExchangeManager=level_info|prefix_time:HeFrameExchangeManager=level_info|prefix_time:InfrastructureWifiMac=level_info|prefix_time:WifiPhy=level_info|prefix_time"
export IDO_LOG_FUNCTION="ApWifiMac=level_function|prefix_time:StaWifiMac=level_function|prefix_time:Txop=level_function|prefix_time:ChannelAccessManager=level_function|prefix_time:FrameExchangeManager=level_function|prefix_time:HeFrameExchangeManager=level_function|prefix_time:InfrastructureWifiMac=level_function|prefix_time:WifiPhy=level_info|prefix_time:ChannelSounding=level_function|prefix_time:MacRxMiddle=level_function|prefix_time:QosTxop=level_function|prefix_time:QosFrameExchangeManager=level_function|prefix_time:RrMultiUserScheduler=level_function|prefix_time:MultiUserScheduler=level_function|prefix_time"

################################################################################################
#                                 Setting testing varibles                                     #
################################################################################################
# defined test variables
seed=("347" "722" "876" "59" "463" "389" "815" "213" "630" "498" "174" "954" "320" "282" "791" "637" "450" "198" "903" "577" "764" "232" "344" "982" "615" "79" "829" "201" "941" "705" "364" "628" "110" "543" "976" "47" "489" "357" "698" "255" "921" "402" "676" "123" "861" "534" "777" "239" "997" "582"
"367" "153" "802" "438" "294" "631" "580" "824" "139" "670" "911" "276" "48" "501" "112" "703" "980" "405" "762" "391" "17" "708" "242" "886" "56" "604" "755" "669" "318" "459" "821" "115" "497" "239" "711" "841" "692" "303" "950" "638" "553" "764" "406" "929" "871" "611" "473" "109" "921" "324") # 100 seeds
# seed=("347" "722" "876" "59" "463" "389" "815" "213" "630" "498" "174" "954" "320" "282" "791" "637" "450" "198" "903" "577" "764" "232" "344" "982" "615" "79" "829" "201" "941" "705" "364" "628" "110" "543" "976" "47" "489" "357" "698" "255" "921" "402" "676" "123" "861" "534" "777" "239" "997" "582") # 50 seed values
# seed=("347" "722" "876" "59" "463" "389" "815" "213" "630" "498" "174" "954" "320" "282" "791" "637" "450" "198" "903" "577" "764" "232" "344" "79" "829") # 25 seed values
# seed=("347" "722" "876" "59" "463" "389" "815" "213" "630" "498") # 10 seed values
# seed=("347" "722" "876" "59") # 4 seed values
# seed=("347") # 1 seed values
radius=("1" "2" "3" "4" "5" "10" "15" "20" "25" "30") # 10 radius
distance=("5" "10" "15" "20" "25" "30")
frequency=("5" "6")
mcs=("0" "1" "2" "3" "4" "5" "6" "7" "8" "9" "10" "11")
nBss=("2" "3" "4" "5" "6")                      # 5 BSS
nBssMulSta=("2" "3" "4" "5" "6")                # 5 BSS
nStations=("1" "2" "3" "4" "5" "6" "7" "8" "9") # 9 stations
nStationsMultiBss=("1" "2" "3" "4")             # 4 stations
multipleBss=("true" "false")
cfpMaxDuration=("1" "5" "10" "15" "20" "25" "50" "100")
numerator=("0" "1" "2" "3" "4" "5" "6" "7" "8" "9" "10" "11") # 12 numerator
senPriority=("0" "1" "2" "3")                                 # 4 sensing priority
residntialDensity=("0" "1")
Bandwidth=("20" "40" "80" "160")
size=${#seed[@]}

# Change and fill this variable for the save location in local, for example /home/ns3/WifiSensing ! 
export Save_loc="/home/manuel/testing"

# Take the number of CPU cores available / manually change for the number of cores you want to use
core_num=$(nproc --all)-5 # Get the number of CPU cores available
if [ "$core_num" -lt 1 ]; then
    core_num=1 # Ensure at least one core is used
fi

################################################################################################
#                          Run the program with different variable                             #
################################################################################################
option=$1

##                                                    ##
####            change ns3 build mode                ###
##                                                    ##
if [ "$option" == "build" ]; then
    build_mode=$2
    if [ "$build_mode" == "debug" ]; then
        ./ns3 clean
        ./ns3 configure --build-profile=debug --enable-examples --enable-tests --disable-werror
        ./ns3 build
    elif [ "$build_mode" == "optimized" ]; then
        ./ns3 clean
        ./ns3 configure --build-profile=optimized --enable-examples --enable-tests --disable-werror
        ./ns3 build
    fi
fi

if [ "$option" == "run" ]; then
    ./ns3
fi

##                                                    ##
####     immediate testing and result logging        ###
##                                                    ##
if [ "$option" == "quick" ]; then
    logging=$2
    if [ "$logging" == "no_log" ]; then
        ./ns3 run examples/wireless/wifi-bf-network.cc --no-build
    elif [ "$logging" == "log_info" ]; then
        (./ns3 run examples/wireless/wifi-bf-network.cc) &
        NS_LOG=$IDO_LOG_INFO ./ns3 run examples/wireless/wifi-bf-network.cc --no-build >log.txt 2>&1
        wait
    elif [ "$logging" == "log_function" ]; then
        (./ns3 run examples/wireless/wifi-bf-network.cc) &
        (NS_LOG=$IDO_LOG_INFO ./ns3 run examples/wireless/wifi-bf-network.cc --no-build >log.out 2>&1) &
        NS_LOG=$IDO_LOG_FUNCTION ./ns3 run examples/wireless/wifi-bf-network.cc --no-build >log1.out 2>&1
        wait
    fi
fi

##                                                    ##
####   investigate the influence of each parameter   ###
##                                                    ##
if [ "$option" == "investigate" ]; then
    logging=$2
    argument=$3
    namefile=$4
    if [ "$logging" == "no_log" ]; then
        ./ns3 run "examples/wireless/wifi-bf-network.cc $argument" 
    elif [ "$logging" == "log_info" ]; then
        (./ns3 run "examples/wireless/wifi-bf-network.cc $argument") &
        NS_LOG=$IDO_LOG_INFO ./ns3 run "examples/wireless/wifi-bf-network.cc $argument" --no-build >log$namefile.txt 2>&1
        wait
    elif [ "$logging" == "log_function" ]; then
        (./ns3 run "examples/wireless/wifi-bf-network.cc $argument") &
        (NS_LOG=$IDO_LOG_INFO ./ns3 run "examples/wireless/wifi-bf-network.cc $argument" --no-build >log_info$namefile.txt 2>&1) &
        NS_LOG=$IDO_LOG_FUNCTION ./ns3 run "examples/wireless/wifi-bf-network.cc $argument" --no-build >log_func$namefile.txt 2>&1
        wait
    fi
fi

if [ "$option" == "debug" ]; then
    argument=$2
    ./ns3 run --gdb "examples/wireless/wifi-bf-network.cc $argument"
fi

################################################################################################
#                                   ns3 Workshop                                               #
################################################################################################
# This is the script for the ns3 workshop. 
# The script is used to run the program with different parameters and save the results in the local directory.
seed=("347" "722" "876" "59" "463" "389" "815" "213" "630" "498" "174" "954" "320" "282" "791" "637" "450" "198" "903" "577" "764" "232" "344" "982" "615" "79" "829" "201" "941" "705" "364" "628" "110" "543" "976" "47" "489" "357" "698" "255" "921" "402" "676" "123" "861" "534" "777" "239" "997" "582"
"367" "153" "802" "438" "294" "631" "580" "824" "139" "670" "911" "276" "48" "501" "112" "703" "980" "405" "762" "391" "17" "708" "242" "886" "56" "604" "755" "669" "318" "459" "821" "115" "497" "239" "711" "841" "692" "303" "950" "638" "553" "764" "406" "929" "871" "611" "473" "109" "921" "324") # 100 seeds

# 1. scenario - Network Density
# using scenario 4 with area of 25 m squared
nStations=("1" "2" "3" "4" "5" "6" "7" "8") # 8 stations
soundingtype=("0" "2")
numberBss=("1" "3" "5") 
if [ "$option" == "density_workshop" ]; then
    # Create base directory if it doesn't exist
    mkdir -p "$Save_loc"

    echo "Creating directory structure..."
    for nBss in "${numberBss[@]}"; do
        for nSta in "${nStations[@]}"; do
            for sounding in "${soundingtype[@]}"; do
                # Create directory structure (without seed-level folders)
                mkdir -p "$Save_loc/Network_density/AP=$nBss/sounding=$sounding/nSta=$nSta"
            done
        done
    done

    counter=0
    for nBss in "${numberBss[@]}"; do
        for nSta in "${nStations[@]}"; do
            for sounding in "${soundingtype[@]}"; do
                for nseed in "${seed[@]}"; do
                    echo "Processing file $counter: nBss=$nBss, soundingType=$sounding, nStations = $nSta, Seed = $nseed"

                    # Run each iteration in the background for parallel processing
                    (
                        ./ns3 run "examples/wireless/wifi-bf-network.cc --nBss=$nBss --nBfBss=$nBss --scenario=1 --radius=25.0 --nStations=$nSta --soundingtype=$sounding --seed=$nseed --simulationTime=10.0 --sensingInterval=100" --no-build> $Save_loc/Network_density/"AP=$nBss"/"sounding=$sounding"/"nSta=$nSta"/nolog_nSta=$nSta,seed=$nseed.out 2>&1
                    ) &
                    ((counter++))

                    # Limit the number of parallel processes (adjust as needed)
                    if ((counter % core_num == 0)); then
                        wait
                    fi
                done
            done
        done
    done
    
    # Wait for all background jobs to finish
    wait
    SOURCE_FILE="python_tools/NetworkDensity_DataExtractor.py"
    # Run DataExtractor.py once per AP/sounding folder
    for nBss in "${numberBss[@]}"; do
        for sounding in "${soundingtype[@]}"; do
            TARGET_DIR="$Save_loc/Network_density/AP=$nBss/sounding=$sounding"

            # Copy the Python script
            cp -n "$SOURCE_FILE" "$TARGET_DIR/"

            # Run the script in that directory
            (cd "$TARGET_DIR" && python3 NetworkDensity_DataExtractor.py) &
        done
    done

    wait
fi



# 2. scenario - Sensing Interval
# using scenario 4 with area of 25 m squared
sensingInterval=("1" "2" "3" "4" "5" "6" "7" "8" "9" "11" "12" "13" "14" "15" "16" "17" "18" "19" "10" "20" "30" "40" "50") # 21 sensing intervals
nBfBss=("1" "3" "5") # 3 BSS
# Total simulation: 21 x 3 x 100 = 6300 simulations
if [ "$option" == "interval_workshop" ]; then
    mkdir -p "$Save_loc"

    echo "Creating directory structure..."
    for nBss in "${numberBss[@]}"; do
        for interval in "${sensingInterval[@]}"; do       
            # Create directory structure (without seed-level folders)
            mkdir -p "$Save_loc/Sensing_interval/AP=$nBss/rate=$interval"
        done
    done

    counter=0
    for nBss in "${nBfBss[@]}"; do
        for Interval in "${sensingInterval[@]}"; do
            for s in "${seed[@]}"; do
                echo "Processing file $counter: nBss=$nBss, Interval=$Interval, Seed = $s"

                # Run each iteration in the background for parallel processing
                (
                    ./ns3 run "examples/wireless/wifi-bf-network.cc --nBss=$nBss --scenario=1 --nStations=8 --soundingtype=2 --radius=25.0 --sensingInterval=$Interval --simulationTime=10.0 --seed=$s" --no-build >/$Save_loc/Sensing_interval/"AP=$nBss"/"rate=$Interval"/nolog_multipleBss_seed=$s.out 2>&1
                ) &

                ((counter++))

                # Limit the number of parallel processes (adjust as needed)
                if ((counter % core_num == 0)); then
                    wait
                fi
            done
        done
    done

    # Wait for all background jobs to finish
    wait

    SOURCE_FILE="python_tools/SensingInterval_DataExtractor.py"
    SOURCE_FILE2="python_tools/SensingInterval_FailPercentage_GraphMaker.py"
    TARGET_DIR="$Save_loc/Sensing_interval"

    # Copy the Python script
    cp -n "$SOURCE_FILE2" "$TARGET_DIR/"
    # Run DataExtractor.py once per AP/sounding folder
    for nBss in "${numberBss[@]}"; do
        TARGET_DIR="$Save_loc/Sensing_interval/AP=$nBss"

        echo "Copying and running DataExtractor.py in $TARGET_DIR..."

        # Copy the Python script
        cp -n "$SOURCE_FILE" "$TARGET_DIR/"

        # Run the script in that directory
        (cd "$TARGET_DIR" && python3 SensingInterval_DataExtractor.py) &

    done

    # Wait for all background jobs to finish
    wait
    cd ..
    # Run the graph maker script
    python3 SensingInterval_FailPercentage_GraphMaker.py

fi