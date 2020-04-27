#!/bin/bash

BOSS=$1
if [ "$BOSS" = "703" ]; then
    ECMS=("4470" "4530" "4575" "4600")
fi

if [ "$BOSS" = "705" ]; then
    ECMS=("4620" "4640" "4660" "4680" "4700")
fi

for ECM in ${ECMS[@]}; do
    NUM_UP=$(ls -l /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/samples/$ECM | grep "txt" | wc -l)
    JobText_SaveDir=/besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/jobs_text/data/$ECM
    ROOT_SaveDir=/besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/rootfile/data/$ECM
    mkdir -p $JobText_SaveDir
    mkdir -p $ROOT_SaveDir
    rm -r $JobText_SaveDir/*txt
    rm -r $ROOT_SaveDir/*.root
    
    for ((num = 0; num <= $NUM_UP; num++))
    do
        file_list=data_${ECM}_DstDpi_20G-${num}.txt
        rootfile=DstDpi_data_$ECM-${num}.root
        jobOptions=jobOptions_DstDpi_data_$ECM-${num}.txt
        echo "#include \"\$ROOTIOROOT/share/jobOptions_ReadRec.txt\"                                   "  > ${JobText_SaveDir}/${jobOptions}
        echo "#include \"\$MAGNETICFIELDROOT/share/MagneticField.txt\"                                 " >> ${JobText_SaveDir}/${jobOptions}
        echo "#include \"\$DTAGALGROOT/share/jobOptions_dTag.txt\"                                     " >> ${JobText_SaveDir}/${jobOptions}
        echo "#include \"\$DDECAYALGROOT/share/jobOptions_DDecay.txt\"                                 " >> ${JobText_SaveDir}/${jobOptions}
        if [ "$BOSS" = "705" ]; then
            echo "#include \"\$MEASUREDECMSSVCROOT/share/anaOptions.txt\"                              " >> ${JobText_SaveDir}/${jobOptions}
        fi
        echo "#include \"/besfs/groups/cal/dedx/\$USER/bes/DstDpi/run/DstDpi/samples/$ECM/$file_list\" " >> ${JobText_SaveDir}/${jobOptions}
        echo "                                                                                         " >> ${JobText_SaveDir}/${jobOptions}
        echo "// Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )            " >> ${JobText_SaveDir}/${jobOptions}
        echo "MessageSvc.OutputLevel = 6;                                                              " >> ${JobText_SaveDir}/${jobOptions}
        echo "                                                                                         " >> ${JobText_SaveDir}/${jobOptions}
        echo "// Number of events to be processed (default is 10)                                      " >> ${JobText_SaveDir}/${jobOptions}
        echo "ApplicationMgr.EvtMax = -1;                                                              " >> ${JobText_SaveDir}/${jobOptions}
        echo "                                                                                         " >> ${JobText_SaveDir}/${jobOptions}
        echo "ApplicationMgr.HistogramPersistency = \"ROOT\";                                          " >> ${JobText_SaveDir}/${jobOptions}
        echo "NTupleSvc.Output = {\"FILE1 DATAFILE='$ROOT_SaveDir/$rootfile' OPT='NEW' TYP='ROOT'\"};  " >> ${JobText_SaveDir}/${jobOptions}
    done
done
