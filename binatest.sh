#!/bin/bash

for i in $(seq 1 4) ; do
        echo "Creating client$i";
        pa4/./twitterTrend_client kh4250-12.cselabs.umn.edu 30397 client$i.in ;
done
echo "extra credits"
        ./twitterTrend_client kh4250-12.cselabs.umn.edu 30397 client1.in client2.in  client3.in  client4.in  ;
