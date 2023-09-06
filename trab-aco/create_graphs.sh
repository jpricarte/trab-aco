# Created by Jordi Pujol
# usage: ./create_graphs.sh NUM_NODES NUM_EDGES INITIAL_SEED

#!/bin/bash

for i in {1..10}
do
    ./graph-gen $1 $2 $(($3 + $i - 1)) >> "./instances/jpr_$1_$2_$i"
done