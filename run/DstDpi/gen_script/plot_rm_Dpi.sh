#/bin/bash

ECMS=("4470" "4530" "4575" "4600" "4620" "4640" "4660" "4680" "4700")

FILENAME="Plot_rm_Dpi"
echo "#!/usr/bin/env bash" > $FILENAME
echo "cd /besfs/groups/cal/dedx/$USER/bes/DstDpi" >> $FILENAME 
for ECM in ${ECMS[@]}; do
    echo "./python/plot_rm_Dpi.py $ECM Dplus" >> $FILENAME
    echo "./python/plot_rm_Dpi.py $ECM D0" >> $FILENAME
done