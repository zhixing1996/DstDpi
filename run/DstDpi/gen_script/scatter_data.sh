#/bin/bash

ECMS=("4470" "4530" "4575" "4600" "4620" "4640" "4660" "4680" "4700")

FILENAME="Scatter_Data"
echo "#!/usr/bin/env bash" > $FILENAME
echo "cd /besfs/groups/cal/dedx/$USER/bes/DstDpi" >> $FILENAME 
for ECM in ${ECMS[@]}; do
    echo "./python/scatter_data.py $ECM Dplus D" >> $FILENAME
    echo "./python/scatter_data.py $ECM Dplus Dst" >> $FILENAME
    echo "./python/scatter_data.py $ECM D0 D" >> $FILENAME
    echo "./python/scatter_data.py $ECM D0 Dst" >> $FILENAME
done
