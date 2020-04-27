#/bin/bash

BOSS=$1

if [ "$BOSS" = "703" ]; then
    ECMS=("4470" "4530" "4575" "4600")
fi

if [ "$BOSS" = "705" ]; then
    ECMS=("4620" "4640" "4660" "4680" "4700")
fi

for ECM in ${ECMS[@]}; do
    cd /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/jobs_text/data/$ECM
    find . -name "*.bosslog" | xargs rm
    find . -name "*.bosserr" | xargs rm
    NUM_UP=$(ls -l | grep "txt" | wc -l)
    boss.condor -g physics -n $NUM_UP jobOptions_DstDpi_data_$ECM-%{ProcId}.txt
done
