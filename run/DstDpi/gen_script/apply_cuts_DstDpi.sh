#/bin/bash

BOSS=$1
PATH=/besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/anaroot/data

if [ "$BOSS" = "703" ]; then
    ECMS=("4470" "4530" "4575" "4600")
fi

if [ "$BOSS" = "705" ]; then
    ECMS=("4620" "4640" "4660" "4680" "4700")
fi

FILENAME="Apply_Cuts_"$BOSS
echo "#!/usr/bin/env bash" > $FILENAME
echo "cd /besfs/groups/cal/dedx/$USER/bes/DstDpi" >> $FILENAME 
for ECM in ${ECMS[@]}; do
    echo "./python/apply_cuts.py $PATH/$ECM/data_${ECM}_DstDpi_raw.root $PATH/$ECM/data_${ECM}_DstDpi_before.root $ECM before raw_signal" >> $FILENAME
done
