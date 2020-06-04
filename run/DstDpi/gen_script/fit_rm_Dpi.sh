#/bin/bash

ECMS=("4620" "4640" "4660" "4680" "4700")

FILENAME="Fit_rm_Dpi"
echo "#!/usr/bin/env bash" > $FILENAME
echo "cd /besfs/groups/cal/dedx/$USER/bes/DstDpi/python" >> $FILENAME 
for ECM in ${ECMS[@]}; do
    echo "./fit_rm_Dpi.py $ECM Dplus D" >> $FILENAME
    echo "./fit_rm_Dpi.py $ECM Dplus Dst" >> $FILENAME
    echo "./fit_rm_Dpi.py $ECM D0 D" >> $FILENAME
    echo "./fit_rm_Dpi.py $ECM D0 Dst" >> $FILENAME
done
