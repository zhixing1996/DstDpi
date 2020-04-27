#/bin/bash

BOSS=$1

if [ "$BOSS" = "703" ]; then
    ECMS=("4470" "4530" "4575" "4600")
fi

if [ "$BOSS" = "705" ]; then
    ECMS=("4620" "4640" "4660" "4680" "4700")
fi

for ECM in ${ECMS[@]}; do
    cd /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/rootfile/data/$ECM
    if [ "$ECM" == "4600" ]; then
        rm -rf DstDpi_data_4600-0.root
    fi
    if [ "$ECM" == "4620" ]; then
        rm -rf DstDpi_data_4620-0.root
    fi
    rm -rf DstDpi_data_${ECM}.root
    hadd DstDpi_data_${ECM}.root *.root
done
