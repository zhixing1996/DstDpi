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
    echo "./python/apply_cuts.py $PATH/$ECM/data_${ECM}_DstDpi_Dplus.root $PATH/$ECM/data_${ECM}_DstDpi_Dplus_D0_before.root Dplus D" >> $FILENAME
    echo "./python/apply_cuts.py $PATH/$ECM/data_${ECM}_DstDpi_Dplus.root $PATH/$ECM/data_${ECM}_DstDpi_Dplus_D0st_before.root Dplus Dst" >> $FILENAME
    echo "./python/apply_cuts.py $PATH/$ECM/data_${ECM}_DstDpi_D0.root $PATH/$ECM/data_${ECM}_DstDpi_D0_Dplus_before.root D0 D" >> $FILENAME
    echo "./python/apply_cuts.py $PATH/$ECM/data_${ECM}_DstDpi_D0.root $PATH/$ECM/data_${ECM}_DstDpi_D0_Dplusst_before.root D0 Dst" >> $FILENAME
    echo "./python/apply_cuts.py $PATH/$ECM/data_${ECM}_DstDpi_Dplus.root $PATH/$ECM/data_${ECM}_DstDpi_Dplus_D0_sideband_before.root Dplus D_sideband" >> $FILENAME
    echo "./python/apply_cuts.py $PATH/$ECM/data_${ECM}_DstDpi_Dplus.root $PATH/$ECM/data_${ECM}_DstDpi_Dplus_D0st_sideband_before.root Dplus Dst_sideband" >> $FILENAME
    echo "./python/apply_cuts.py $PATH/$ECM/data_${ECM}_DstDpi_D0.root $PATH/$ECM/data_${ECM}_DstDpi_D0_Dplus_sideband_before.root D0 D_sideband" >> $FILENAME
    echo "./python/apply_cuts.py $PATH/$ECM/data_${ECM}_DstDpi_D0.root $PATH/$ECM/data_${ECM}_DstDpi_D0_Dplusst_sideband_before.root D0 Dst_sideband" >> $FILENAME
done
