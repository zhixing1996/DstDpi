#/bin/bash

BOSS=$1

SOURCE_PATH=/besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/rootfile/data
ANA_PATH=/besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/anaroot/data
LOG_PATH=/besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/logfile
mkdir -p $ANA_PATH
mkdir -p $LOG_PATH
if [ "$BOSS" = "703" ]; then
    SAMPLES=("4600")
    ECMS=("4.59953")
fi

if [ "$BOSS" = "705" ]; then
    SAMPLES=("4620" "4640" "4660" "4680" "4700")
    ECMS=("4.6260" "4.6400" "4.6600" "4.6800" "4.7000")
fi

COUNT=0
FILENAME="Get_Info_Data_"$BOSS
echo "#!/usr/bin/env bash" > $FILENAME
echo "cd /besfs/groups/cal/dedx/$USER/bes/DstDpi" >> $FILENAME 
for SAMPLE in ${SAMPLES[@]}; do
    mkdir -p $ANA_PATH/$SAMPLE
    echo "./python/get_info.py $SOURCE_PATH/$SAMPLE/DstDpi_data_${SAMPLE}.root $ANA_PATH/$SAMPLE/data_${SAMPLE}_DstDpi_Dplus.root ${ECMS[$COUNT]} Dplus raw" >> $FILENAME
    echo "./python/get_info.py $SOURCE_PATH/$SAMPLE/DstDpi_data_${SAMPLE}.root $ANA_PATH/$SAMPLE/data_${SAMPLE}_DstDpi_D0.root ${ECMS[$COUNT]} D0 raw" >> $FILENAME
    echo "./python/get_info.py $SOURCE_PATH/$SAMPLE/DstDpi_data_${SAMPLE}.root $ANA_PATH/$SAMPLE/data_${SAMPLE}_DstDpi_Dplus_D_signal.root ${ECMS[$COUNT]} Dplus D signal" >> $FILENAME
    echo "./python/get_info.py $SOURCE_PATH/$SAMPLE/DstDpi_data_${SAMPLE}.root $ANA_PATH/$SAMPLE/data_${SAMPLE}_DstDpi_Dplus_D_background.root ${ECMS[$COUNT]} Dplus D background" >> $FILENAME
    echo "./python/get_info.py $SOURCE_PATH/$SAMPLE/DstDpi_data_${SAMPLE}.root $ANA_PATH/$SAMPLE/data_${SAMPLE}_DstDpi_Dplus_Dst_signal.root ${ECMS[$COUNT]} Dplus Dst signal" >> $FILENAME
    echo "./python/get_info.py $SOURCE_PATH/$SAMPLE/DstDpi_data_${SAMPLE}.root $ANA_PATH/$SAMPLE/data_${SAMPLE}_DstDpi_Dplus_Dst_background.root ${ECMS[$COUNT]} Dplus Dst background" >> $FILENAME
    echo "./python/get_info.py $SOURCE_PATH/$SAMPLE/DstDpi_data_${SAMPLE}.root $ANA_PATH/$SAMPLE/data_${SAMPLE}_DstDpi_D0_D_signal.root ${ECMS[$COUNT]} D0 D signal" >> $FILENAME
    echo "./python/get_info.py $SOURCE_PATH/$SAMPLE/DstDpi_data_${SAMPLE}.root $ANA_PATH/$SAMPLE/data_${SAMPLE}_DstDpi_D0_D_background.root ${ECMS[$COUNT]} D0 D background" >> $FILENAME
    echo "./python/get_info.py $SOURCE_PATH/$SAMPLE/DstDpi_data_${SAMPLE}.root $ANA_PATH/$SAMPLE/data_${SAMPLE}_DstDpi_D0_Dst_signal.root ${ECMS[$COUNT]} D0 Dst signal" >> $FILENAME
    echo "./python/get_info.py $SOURCE_PATH/$SAMPLE/DstDpi_data_${SAMPLE}.root $ANA_PATH/$SAMPLE/data_${SAMPLE}_DstDpi_D0_Dst_background.root ${ECMS[$COUNT]} D0 Dst background" >> $FILENAME
    COUNT=$COUNT+1
done
