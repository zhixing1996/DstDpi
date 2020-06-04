#/bin/bash

ECMS=("4620" "4640" "4660" "4680" "4700")

FILENAME="Scatter_rm_D_m_Dpi"
echo "#!/usr/bin/env bash" > $FILENAME
echo "cd /besfs/groups/cal/dedx/$USER/bes/DstDpi/python" >> $FILENAME 
for ECM in ${ECMS[@]}; do
    echo "./scatter_rm_D_m_Dpi.py $ECM Dplus D" >> $FILENAME
    echo "./scatter_rm_D_m_Dpi.py $ECM Dplus Dst" >> $FILENAME
    echo "./scatter_rm_D_m_Dpi.py $ECM D0 D" >> $FILENAME
    echo "./scatter_rm_D_m_Dpi.py $ECM D0 Dst" >> $FILENAME
done
