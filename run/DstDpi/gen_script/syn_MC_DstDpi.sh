#/bin/bash

BOSS=$1
TYPE=$2

if [ "$BOSS" = "703" ]; then
    ECMS=("4600")
fi

if [ "$BOSS" = "705" ]; then
    ECMS=("4620" "4640" "4660" "4680" "4700")
fi

for ECM in ${ECMS[@]}; do
    cd /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/rootfile/mc/$TYPE/$ECM
    rm -rf DstDpi_mc_${TYPE}_${ECM}.root
    hadd DstDpi_mc_${TYPE}_${ECM}.root *.root
done
