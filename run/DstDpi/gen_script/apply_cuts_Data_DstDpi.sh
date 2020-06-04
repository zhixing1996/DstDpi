#/bin/bash

BOSS=$1
PATH=/besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/anaroot/data

if [ "$BOSS" = "703" ]; then
    ECMS=("4600")
fi

if [ "$BOSS" = "705" ]; then
    ECMS=("4620" "4640" "4660" "4680" "4700")
fi

FILENAME="Apply_Cuts_Data_"$BOSS
echo "#!/usr/bin/env bash" > $FILENAME
echo "cd /besfs/groups/cal/dedx/$USER/bes/DstDpi/python" >> $FILENAME 
for ECM in ${ECMS[@]}; do
    echo "./apply_cuts.py $PATH/$ECM/data_${ECM}_DstDpi_Dplus.root $PATH/$ECM/data_${ECM}_DstDpi_Dplus_raw_before.root Dplus raw" >> $FILENAME
    echo "./apply_cuts.py $PATH/$ECM/data_${ECM}_DstDpi_D0.root $PATH/$ECM/data_${ECM}_DstDpi_D0_raw_before.root D0 raw" >> $FILENAME
    echo "./apply_cuts.py $PATH/$ECM/data_${ECM}_DstDpi_Dplus_D_signal.root $PATH/$ECM/data_${ECM}_DstDpi_Dplus_signal_missing_before.root Dplus missing" >> $FILENAME
    echo "./apply_cuts.py $PATH/$ECM/data_${ECM}_DstDpi_Dplus_D_signal.root $PATH/$ECM/data_${ECM}_DstDpi_Dplus_D0_signal_before.root Dplus D" >> $FILENAME
    echo "./apply_cuts.py $PATH/$ECM/data_${ECM}_DstDpi_Dplus_Dst_signal.root $PATH/$ECM/data_${ECM}_DstDpi_Dplus_D0st_signal_before.root Dplus Dst" >> $FILENAME
    echo "./apply_cuts.py $PATH/$ECM/data_${ECM}_DstDpi_Dplus_D_background.root $PATH/$ECM/data_${ECM}_DstDpi_Dplus_D0_background_before.root Dplus D" >> $FILENAME
    echo "./apply_cuts.py $PATH/$ECM/data_${ECM}_DstDpi_Dplus_Dst_background.root $PATH/$ECM/data_${ECM}_DstDpi_Dplus_D0st_background_before.root Dplus Dst" >> $FILENAME
    echo "./apply_cuts.py $PATH/$ECM/data_${ECM}_DstDpi_D0_D_signal.root $PATH/$ECM/data_${ECM}_DstDpi_D0_Dplus_signal_before.root D0 D" >> $FILENAME
    echo "./apply_cuts.py $PATH/$ECM/data_${ECM}_DstDpi_D0_Dst_signal.root $PATH/$ECM/data_${ECM}_DstDpi_D0_Dplusst_signal_before.root D0 Dst" >> $FILENAME
    echo "./apply_cuts.py $PATH/$ECM/data_${ECM}_DstDpi_D0_D_background.root $PATH/$ECM/data_${ECM}_DstDpi_D0_Dplus_background_before.root D0 D" >> $FILENAME
    echo "./apply_cuts.py $PATH/$ECM/data_${ECM}_DstDpi_D0_Dst_background.root $PATH/$ECM/data_${ECM}_DstDpi_D0_Dplusst_background_before.root D0 Dst" >> $FILENAME
done
