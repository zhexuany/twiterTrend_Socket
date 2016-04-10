#!/bin/bash

for i in $(seq 1 4) ; do
        echo "Creating client$i";
        ./twitterTrend_client localhost 30391 client$i.in ;
done
