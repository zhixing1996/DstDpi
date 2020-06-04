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
    cd /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/jobs_text/mc/$TYPE/$ECM
    find . -name "*.bosslog" | xargs rm
    find . -name "*.bosserr" | xargs rm
    jobs=`ls *.txt`
    for job in $jobs
    do
        boss.condor $job
    done
done
