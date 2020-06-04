#/bin/bash

ECMS=("4620" "4640" "4660" "4680" "4700")

FILENAME="Plot_rm_D"
echo "#!/usr/bin/env bash" > $FILENAME
echo "cd /besfs/groups/cal/dedx/$USER/bes/DstDpi/python" >> $FILENAME 
for ECM in ${ECMS[@]}; do
    echo "./plot_rm_D.py $ECM Dplus D" >> $FILENAME
    echo "./plot_rm_D.py $ECM Dplus Dst" >> $FILENAME
    echo "./plot_rm_D.py $ECM D0 D" >> $FILENAME
    echo "./plot_rm_D.py $ECM D0 Dst" >> $FILENAME
done
