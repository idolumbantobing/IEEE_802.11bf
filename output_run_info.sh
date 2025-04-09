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

# Change / Fill this variable for the save location in local ! 
export Save_loc="/home/manuel/HiWi"

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

##                                                    ##
####        Testing variable : nStation              ###
##                                                    ##
if [ "$option" == "nStation" ]; then
    counter=0
    for nSta in "${nStations[@]}"; do
        for nseed in "${seed[@]}"; do
            echo "Processing file $counter: nStations = $nSta, Seed = $nseed"

            # Run each iteration in the background for parallel processing
            (
                ./ns3 run "examples/wireless/wifi-bf-network.cc --nStations=$nSta --seed=$nseed --simulationTime=60.0 --radius=1 --sensingInterval=1000" --no-build> $Save_loc/Result/Result_nStations_based_SU/"nSta=$nSta"/nolog_nSta=$nSta,seed=$nseed.out 2>&1
            ) &
            ((counter++))

            # Limit the number of parallel processes (adjust as needed)
            if ((counter % 4 == 0)); then
                wait
            fi
        done
    done
fi

##                                                    ##
####        Testing variable : nStation MU           ###
##                                                    ##
if [ "$option" == "nStationMU" ]; then
    counter=0
    for nSta in "${nStations[@]}"; do
        for nseed in "${seed[@]}"; do
            echo "Processing file $counter: nStations = $nSta, Seed = $nseed"

            # Run each iteration in the background for parallel processing
            (
                ./ns3 run "examples/wireless/wifi-bf-network.cc --nStations=$nSta --seed=$nseed --soundingtype=2 --simulationTime=60.0 --radius=2 --sensingInterval=1000" --no-build > /$Save_loc/Result/Result_nStations_based_MU/"nSta=$nSta"/nolog_nSta=$nSta,seed=$nseed.out 2>&1
            ) &
            ((counter++))

            # Limit the number of parallel processes (adjust as needed)
            if ((counter % 4 == 0)); then
                wait
            fi
        done
    done
fi

##                                                    ##
####        Testing variable : frequency              ###
##                                                    ##
frequency_test=("2.4" "5" "6")
if [ "$option" == "frequency" ]; then
    counter=0
    for nSta in "${nStations[@]}"; do
        for freq in "${frequency_test[@]}"; do
            for nseed in "${seed[@]}"; do
                echo "Processing file $counter: nStations = $nSta, Seed = $nseed"

                # Run each iteration in the background for parallel processing
                (
                   ./ns3 run "examples/wireless/wifi-bf-network.cc --frequency=$freq --nStations=$nSta --seed=$nseed --simulationTime=60.0 --radius=1 --sensingInterval=1000" --no-build>/$Save_loc/Result/Result_frequency-based/"freq=$freq"/"nSta=$nSta"/nolog_nSta=$nSta,seed=$nseed.out 2>&1
                ) &
                ((counter++))

                # # Limit the number of parallel processes (adjust as needed)
                if ((counter % 4 == 0)); then
                    wait
                fi
            done
        done
    done
fi

##                                                    ##
####          Testing variable : cfpMax              ###
##                                                    ##
if [ "$option" == "cfpMax" ]; then
    counter=0
    for s in "${seed[@]}"; do
        for cfp in "${cfpMaxDuration[@]}"; do
            echo "Processing file $counter: cfpMaxDuration = $cfp, Seed = $s"

            # Run each iteration in the background for parallel processing
            (
                ./ns3 run "examples/wireless/wifi-bf-network.cc --cfpMaxDuration=$cfp --seed=$s" >/$Save_loc/Result/Result_maxcfp-based/maxCFP=$cfp/nolog_maxCFP=$cfp,seed=$s.out 2>&1
            ) &

            ((counter++))

            # Limit the number of parallel processes (adjust as needed)
            if ((counter % 4 == 0)); then
                wait
            fi
        done
    done
    wait
fi

##                                                    ##
####        Testing variable : multipleBss           ###
##                                                    ##
if [ "$option" == "multipleBssAx" ]; then
    counter=0
    for BssNum in "${nBss[@]}"; do
        for s in "${seed[@]}"; do
            nAxBss=$((BssNum - 1))
            echo "Processing file $counter: nBss = $BssNum, Seed = $s"

            # Run each iteration in the background for parallel processing
            (
                ./ns3 run "examples/wireless/wifi-bf-network.cc --nBss=$BssNum --nAxBss=$nAxBss --seed=$s --sensingInterval=1000 --simulationTime=10.0" > /$Save_loc/Result/Result_multipleBSS/MultipleAX/"nBss=$BssNum"/nolog_multipleBss=$BssNum,seed=$s.txt 2>&1
            ) &

            ((counter++))

            # Limit the number of parallel processes (adjust as needed)
            if ((counter % 20 == 0)); then
                wait
            fi
        done
    done
fi

if [ "$option" == "multipleBssBf" ]; then
    counter=0
    for BssNum in "${nBss[@]}"; do

        for s in "${seed[@]}"; do
            echo "Processing file $counter: nBss = $BssNum, Seed = $s"
            # Run each iteration in the background for parallel processing
            (
                ./ns3 run "examples/wireless/wifi-bf-network.cc --nBss=$BssNum --nAxBss=1 --seed=$s --simulationTime=10.0" >/$Save_loc/Result/Result_multipleBSS/MultipleBF/"nBss=$BssNum"/nolog_multipleBss=$BssNum,seed=$s.out 2>&1
            ) &

            ((counter++))

            # Limit the number of parallel processes (adjust as needed)
            if ((counter % 20 == 0)); then
                wait
            fi

        done

    done
fi

nBss=("2" "3" "4" "5" "6")
if [ "$option" == "multipleBssResidential" ]; then
    counter=0
    for dense in "${residntialDensity[@]}"; do
        for s in "${seed[@]}"; do
            echo "Processing file $counter: dense = $dense, Seed = $s"
            # Run each iteration in the background for parallel processing
            (
                ./ns3 run "examples/wireless/wifi-bf-network.cc --residentialDensity=$dense --scenario=3 --seed=$s --simulationTime=10.0" >/$Save_loc/Result/Result_multipleBSS/Residential/"dense=$dense"/Result/nolog_dense=$dense,seed=$s.out 2>&1

            ) &

            ((counter++))

            # Limit the number of parallel processes (adjust as needed)
            if ((counter % 4 == 0)); then
                wait
            fi
        done
    done
fi

if [ "$option" == "multipleBssBfmulSta" ]; then
    counter=0
    for nSta in "${nStationsMultiBss[@]}"; do
        for BssNum in "${nBssMulSta[@]}"; do
            for s in "${seed[@]}"; do
                echo "Processing file $counter: nSta=$nSta, nBss = $BssNum, Seed = $s"

                # Run each iteration in the background for parallel processing
                (
                    forRatio=$((2 * BssNum))
                    ./ns3 run "examples/wireless/wifi-bf-network.cc --nBss=$forRatio --nAxBss=$BssNum --nStations=$nSta --seed=$s " >/$Save_loc/Result/Result_multipleBSS/MultipleRatioMultipleSta/"sta=$nSta"/"nBss=$BssNum"/nolog_multipleBss=$BssNum,seed=$s.out 2>&1
                ) &

                ((counter++))

                # Limit the number of parallel processes (adjust as needed)
                if ((counter % 4 == 0)); then
                    wait
                fi

            done
        done
    done
fi

if [ "$option" == "multipleBssRatio" ]; then
    BssNum=("12")
    counter=0
    for numer in "${numerator[@]}"; do
        for s in "${seed[@]}"; do
            nAxBss=$((BssNum - 1))
            echo "Processing file $counter: nBss = $BssNum, Seed = $s"

            # Run each iteration in the background for parallel processing
            (
                ./ns3 run "examples/wireless/wifi-bf-network.cc --nBss=$BssNum --numerator=$numer --seed=$s" >/$Save_loc/Result/Result_multipleBSS/Ratio/"ratio=$numer""to12"/nolog_multipleBss=seed=$s.out 2>&1

            ) &

            ((counter++))

            # Limit the number of parallel processes (adjust as needed)
            if ((counter % 4 == 0)); then
                wait
            fi

        done
    done
fi

sensingInterval=("10" "50" "100" "500" "1000")
if [ "$option" == "multipleBssBfInterval" ]; then
    counter=0
    for Interval in "${sensingInterval[@]}"; do
        for BssNum in "${nBssMulSta[@]}"; do
            for s in "${seed[@]}"; do
                echo "Processing file $counter: Interval=$Interval, nBss = $BssNum, Seed = $s"

                # Run each iteration in the background for parallel processing
                (
                    ./ns3 run "examples/wireless/wifi-bf-network.cc --nBss=$BssNum --nAxBss=1 --simulationTime=$Interval --sensingInterval=$Interval --seed=$s" --no-build >/$Save_loc/Result/Result_multipleBSS/multipleBFInterval/"Interval=$Interval"/"nBss=$BssNum"/nolog_multipleBss=$BssNum,seed=$s.out 2>&1
                ) &

                ((counter++))

                # Limit the number of parallel processes (adjust as needed)
                if ((counter % 4 == 0)); then
                    wait
                fi

            done
        done
    done
fi

sensingInterval=("10" "20" "30" "40" "50" "100" "500" "1000")
if [ "$option" == "multipleBssBfIntervalOffice" ]; then
    counter=0
    for Interval in "${sensingInterval[@]}"; do
        for s in "${seed[@]}"; do
            echo "Processing file $counter: Interval=$Interval, Seed = $s"

            # Run each iteration in the background for parallel processing
            (
                ./ns3 run "examples/wireless/wifi-bf-network.cc --scenario=5 --nBfBss=6 --simulationTime=60.0 --sensingInterval=$Interval --seed=$s" --no-build >/$Save_loc/Result/Result_multipleBSS/multipleBFInterval/"Interval=$Interval"//nolog_multipleBss_seed=$s.out 2>&1
            ) &

            ((counter++))

            # Limit the number of parallel processes (adjust as needed)
            if ((counter % 4 == 0)); then
                wait
            fi

        done
    done
fi

##                                                    ##
####             Testing variable : mcs              ###
##                                                    ##
if [ "$option" == "mcs" ]; then
    counter=0
    for nMcs in "${mcs[@]}"; do
        for nseed in "${seed[@]}"; do
            echo "Processing file $counter: mcs = $nMcs, Seed = $nseed"

            # Run each iteration in the background for parallel processing
            (
               ./ns3 run "examples/wireless/wifi-bf-network.cc --mcs=$nMcs --seed=$nseed" >/$Save_loc/Result/Result_mcs-based/"mcs=$nMcs"/nolog_nMcs=$nMcs,seed=$nseed.out 2>&1
            ) &
            
            ((counter++))

            # Limit the number of parallel processes (adjust as needed)
            if ((counter % 4 == 0)); then
                wait
            fi
        done
    done
fi

##                                                    ##
####     Testing variable : radius single BSS        ###
##                                                    ##
if [ "$option" == "radius" ]; then
    counter=0
    for rad in "${radius[@]}"; do
        for nseed in "${seed[@]}"; do
            echo "Processing file $counter: radius = $rad, Seed = $nseed"

            # Run each iteration in the background for parallel processing
            (
                ./ns3 run "examples/wireless/wifi-bf-network.cc --nStations=4 --radius=$rad --seed=$nseed --simulationTime=1.0" >/$Save_loc/Result/Result_distance-based/"r=$rad"/nolog_radius=$rad,seed=$nseed.out 2>&1

            ) &
            
            ((counter++))

            # Limit the number of parallel processes (adjust as needed)
            if ((counter % 4 == 0)); then
                wait
            fi
        done
    done
fi

##                                                    ##
####  Testing variable : sensing priority            ###
##                                                    ##
if [ "$option" == "idealSensPrioScenario" ]; then
    counter=0
    for prio in "${senPriority[@]}"; do
        for nseed in "${seed[@]}"; do
            echo "Processing file $counter: prio = $prio, Seed = $nseed"

            # Run each iteration in the background for parallel processing
            (
                ./ns3 run "examples/wireless/wifi-bf-network.cc --sensingPriority=$prio --seed=$nseed --nBss=2 --nAxBss=1 --simulationTime=60.0" >/$Save_loc/Result/Result_sensPrio/Result_sensPrio_Ideal/"prio=$prio"/nolog_priority=seed=$nseed.out 2>&1

            ) &
            
            ((counter++))

            # Limit the number of parallel processes (adjust as needed)
            if ((counter % 4 == 0)); then
                wait
            fi
        done
    done
fi

##                                                    ##
####  Testing variable : sensing priority            ###
##                                                    ##
if [ "$option" == "simpleSensingIntervals" ]; then
    counter=0
    for Interval in "${simpleSensingIntervals[@]}"; do
        for nseed in "${seed[@]}"; do
            echo "Processing file $counter: Interval = $Interval, Seed = $nseed"

            # Run each iteration in the background for parallel processing
            (
                ./ns3 run "examples/wireless/wifi-bf-network.cc --sensingInterval=$Interval --seed=$nseed --nBss=2 --nAxBss=1 --simulationTime=60.0" >/$Save_loc/Result/Result_sensInterval/"Interval=$Interval"/nolog_priority=seed=$nseed.out 2>&1

            ) &
            
            ((counter++))

            # Limit the number of parallel processes (adjust as needed)
            if ((counter % 4 == 0)); then
                wait
            fi
        done
    done
fi

##                                                    ##
####  Testing variable : residential scenario        ###
##                                                    ##
if [ "$option" == "residentialSensPrioScenario" ]; then
    counter=0
    for prio in "${senPriority[@]}"; do
        for nseed in "${seed[@]}"; do
            echo "Processing file $counter: prio = $prio, Seed = $nseed"

            # Run each iteration in the background for parallel processing
            (
                ./ns3 run "examples/wireless/wifi-bf-network.cc --sensingPriority=$prio --seed=$nseed --scenario=3 --simulationTime=60.0" >/$Save_loc/Result/Result_sensPrio/Result_sensPrio_Residential/"prio=$prio"/nolog_priority=seed=$nseed.out 2>&1

            ) &
            
            ((counter++))

            # Limit the number of parallel processes (adjust as needed)
            if ((counter % 4 == 0)); then
                wait
            fi
        done
    done
fi

##                                                    ##
####  Testing variable : Bandwidth scenario        ###
##                                                    ##
if [ "$option" == "Bandwidth" ]; then
    counter=0
    for band in "${Bandwidth[@]}"; do
        for nseed in "${seed[@]}"; do
            echo "Processing file $counter: Bandwidth = $band, Seed = $nseed"

            # Run each iteration in the background for parallel processing
            (
                ./ns3 run "examples/wireless/wifi-bf-network.cc --channelWidth=$band --seed=$nseed --nBss=2 --nAxBss=1 --nStations=4 --simulationTime=10.0" >/$Save_loc/Result/Result_bandwidth_based/"b=$band"/nolog_bandwidth=seed=$nseed.out 2>&1

            ) &
            
            ((counter++))

            # Limit the number of parallel processes (adjust as needed)
            if ((counter % 4 == 0)); then
                wait
            fi
        done
    done
fi

##                                                    ##
####  Testing variable : radius single BSS (1 seed)  ###
##                                                    ##
seed=("347")
radius=("1" "2" "3" "4" "5" "6" "7" "8" "9" "10" "11" "12" "13" "14" "15" "16" "17" "18" "19" "20" "21" "22" "23" "24" "25" "26" "27" "28" "29" "30")
if [ "$option" == "radiusSingle" ]; then
    counter=0
    for rad in "${radius[@]}"; do
        for nseed in "${seed[@]}"; do
            echo "Processing file $counter: radius = $rad, Seed = $nseed"

            # Run each iteration in the background for parallel processing
            (
                ./ns3 run "examples/wireless/wifi-bf-network.cc --radius=$rad --seed=$nseed --nStations=9 --simulationTime=1.0" >/$Save_loc/Result/Result_distance-1seedbased/"r=$rad"/nolog_radius=$rad,seed=$nseed.out 2>&1

            ) &
            ((counter++))

            # Limit the number of parallel processes (adjust as needed)
            if ((counter % 4 == 0)); then
                wait
            fi
        done
    done
fi


##                                                    ##
###  Testing variable : nAxBss for checking latency  ###
##                                                    ##
seed=("347" "722" "876" "59" "463" "389" "815" "213" "630" "498") # 10 seed values
nAxBss=("1" "2" "3" "4" "5" "6" "7" "8" "9" "10" "11" "12" "13" "14" "15" "16" "17" "18" "19" "20" "21" "22" "23" "24" "25" "26" "27" "28" "29" "30" "31" "32" "33" "34" "35" "36" "37" "38" "39" "40" "41" "42" "43" "44" "45" "46" "47" "48" "49" "50" "51" "52" "53" "54" "55" "56" "57" "58" "59" "60" "61" "62" "63" "64" "65" "66" "67" "68" "69" "70" "71" "72" "73" "74" "75" "76" "77" "78" "79" "80" "81" "82" "83" "84" "85" "86" "87" "88" "89" "90" "91" "92" "93" "94" "95" "96" "97" "98" "99" "100")
if [ "$option" == "checkAxLatency" ]; then
    counter=0
    for nBss in "${nAxBss[@]}"; do
        for nseed in "${seed[@]}"; do
            echo "Processing file $counter: nAxBss = $nBss, Seed = $nseed"

            # Run each iteration in the background for parallel processing
            (
               ./ns3 run "examples/wireless/wifi-bf-network.cc --scenario=4 --enableFrameAggregation=1 --seed=$nseed --nBss=$nBss --nAxBss=$nBss --simulationTime=10.0" > /$Save_loc/Result/Result_AxLatency/"nAxBss=$nBss"/nolog_nAxBss=$nBss,seed=$nseed.out 2>&1

            ) &
            
            ((counter++))

            # Limit the number of parallel processes (adjust as needed)
            if ((counter % 20 == 0)); then
                wait
            fi
        done
    done
fi

##                                                    ##
###  Testing variable : nAxBss for checking latency  ###
##                                                    ##
seed=("347" "722" "876" "59" "463" "389" "815" "213" "630" "498") # 10 seed values
nAxBss=("1" "2" "3" "4" "5" "6" "7" "8" "9" "10" "11" "12" "13" "14" "15" "16" "17" "18" "19" "20" "21" "22" "23" "24" "25" "26" "27" "28" "29" "30" "31" "32" "33" "34" "35" "36" "37" "38" "39" "40" "41" "42" "43" "44" "45" "46" "47" "48" "49" "50" "51" "52" "53" "54" "55" "56" "57" "58" "59" "60" "61" "62" "63" "64" "65" "66" "67" "68" "69" "70" "71" "72" "73" "74" "75" "76" "77" "78" "79" "80" "81" "82" "83" "84" "85" "86" "87" "88" "89" "90" "91" "92" "93" "94" "95" "96" "97" "98" "99" "100")
if [ "$option" == "checkAxLatency" ]; then
    counter=0
    for nBss in "${nAxBss[@]}"; do
        for nseed in "${seed[@]}"; do
            echo "Processing file $counter: nAxBss = $nBss, Seed = $nseed"

            # Run each iteration in the background for parallel processing
            (
               ./ns3 run "examples/wireless/wifi-bf-network.cc --scenario=4 --enableFrameAggregation=1 --seed=$nseed --nBss=$nBss --nAxBss=$nBss --simulationTime=10.0" > /$Save_loc/Result/Result_AxLatency/"nAxBss=$nBss"/nolog_nAxBss=$nBss,seed=$nseed.out 2>&1

            ) &
            
            ((counter++))

            # Limit the number of parallel processes (adjust as needed)
            if ((counter % 20 == 0)); then
                wait
            fi
        done
    done
fi
