#/bin/bash

BOSS=$1
TYPE=$2
PATH=/besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/anaroot/mc/$2

if [ "$BOSS" = "703" ]; then
    ECMS=("4600")
fi

if [ "$BOSS" = "705" ]; then
    ECMS=("4620" "4640" "4660" "4680" "4700")
fi

FILENAME="Apply_Cuts_${TYPE}_"$BOSS
echo "#!/usr/bin/env bash" > $FILENAME
echo "cd /besfs/groups/cal/dedx/$USER/bes/DstDpi/python" >> $FILENAME 
for ECM in ${ECMS[@]}; do
    echo "./apply_cuts.py $PATH/$ECM/mc_${TYPE}_${ECM}_DstDpi_Dplus.root $PATH/$ECM/mc_${TYPE}_${ECM}_DstDpi_Dplus_raw_before.root Dplus raw" >> $FILENAME
    echo "./apply_cuts.py $PATH/$ECM/mc_${TYPE}_${ECM}_DstDpi_Dplus_D_signal.root $PATH/$ECM/mc_${TYPE}_${ECM}_DstDpi_Dplus_D0_signal_before.root Dplus D" >> $FILENAME
    echo "./apply_cuts.py $PATH/$ECM/mc_${TYPE}_${ECM}_DstDpi_Dplus_D_background.root $PATH/$ECM/mc_${TYPE}_${ECM}_DstDpi_Dplus_D0_background_before.root Dplus D" >> $FILENAME
done
