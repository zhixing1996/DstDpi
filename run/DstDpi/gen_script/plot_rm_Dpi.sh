#/bin/bash

ECMS=("4620" "4640" "4660" "4680" "4700")

FILENAME="Plot_rm_Dpi"
echo "#!/usr/bin/env bash" > $FILENAME
echo "cd /besfs/groups/cal/dedx/$USER/bes/DstDpi/python" >> $FILENAME 
for ECM in ${ECMS[@]}; do
    echo "./plot_rm_Dpi.py $ECM Dplus" >> $FILENAME
    echo "./plot_rm_Dpi.py $ECM D0" >> $FILENAME
done
