#/bin/bash

BOSS=$1
TYPE=$2

if [ "$BOSS" = "703" ]; then
    ECMS=("4600")
fi

if [ "$BOSS" = "705" ]; then
    ECMS=("4680")
fi

for ECM in ${ECMS[@]}; do
    cd /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/rootfile/mc/$TYPE/$ECM
    if [ "$ECM" == "4600" ]; then
        if [ "$TYPE" == "DD" ]; then
            rm -rf DstDpi_incMC_DD_4600-0.root
        fi
    fi
    rm -rf DstDpi_incMC_${TYPE}_${ECM}.root
    hadd DstDpi_incMC_${TYPE}_${ECM}.root *.root
done
