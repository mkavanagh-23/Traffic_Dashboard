#!/bin/bash

while true
do
    curl -v "localhost:6969/events?source=NYSDOT" | jq | grep description >> nysdot.txt
    curl -v "localhost:6969/events?source=ONGOV" | jq | grep description >> ongov.txt
    curl -v "localhost:6969/events?source=MCNY" | jq | grep description >> mcny.txt
    curl -v "localhost:6969/events?source=ONMT" | jq | grep description >> onmt.txt
    curl -v "localhost:6969/events?source=OTT" | jq | grep description >> ott.txt
    curl -v "localhost:6969/events?source=MTL" | jq | grep description >> mtl.txt
    sleep 120
done