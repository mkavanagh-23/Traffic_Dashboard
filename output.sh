#!/bin/bash

# List of sources and corresponding filenames
sources=("NYSDOT" "ONGOV" "MCNY" "ONMT" "OTT" "MTL")
filenames=("nysdot.txt" "ongov.txt" "mcny.txt" "onmt.txt" "ott.txt" "mtl.txt")

while true
do
    # Loop through each source
    for ((i=0; i<${#sources[@]}; i++))
    do
        curl -v "localhost:6969/events?source=${sources[i]}" | jq | grep description >> "temp_processing/${filenames[i]}"
        sort "temp_processing/${filenames[i]}" | uniq > temp.txt && mv temp.txt "temp_processing/${filenames[i]}"
    done
    
    sleep 120
done