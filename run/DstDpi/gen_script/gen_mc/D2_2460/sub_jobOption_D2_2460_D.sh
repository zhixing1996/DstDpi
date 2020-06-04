#/bin/bash
BOSS=$1
if [ "$BOSS" = "703" ]; then
    ECMS=("4600")
    CMS=("4.600")
    RUNNO_LOW=("35227")
    RUNNO_UP=("36213")
fi
if [ "$BOSS" = "705" ]; then
    # ECMS=("4620" "4640" "4660" "4680" "4700")
    # CMS=("4.626" "4.640" "4.660" "4.680" "4.700")
    # RUNNO_LOW=("63075" "63516" "63718" "63867" "64028")
    # RUNNO_UP=("63515" "63715" "63852" "64015" "64289")
    ECMS=("4620" "4640" "4660" "4680" "4700")
    CMS=("4.626" "4.640" "4.660" "4.680" "4.700")
    RUNNO_LOW=("35227" "35227" "35227" "35227" "35227")
    RUNNO_UP=("36213" "36213" "36213" "36213" "36213")
fi

COUN=0
FILENAME="Sub_jobOption_"$BOSS
for ECM in ${ECMS[@]}; do
    JobText_SaveDir=/besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/jobs_text/mc/D2_2460/$ECM
    Script_Dir=/besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/gen_script/gen_mc/D2_2460
    mkdir -p $JobText_SaveDir
    rm -rf $JobText_SaveDir/jobOptions*txt
    rm -rf $JobText_SaveDir/subSimRec_*.sh
    rm -rf $JobText_SaveDir/fort.*
    rm -rf $JobText_SaveDir/phokhara*
    cd $JobText_SaveDir
    cp -r $Script_Dir/jobOptions_sim_sig_D2_2460_D_mDIY_tempE_705.sh jobOptions_sim_sig_D2_2460_D_mDIY_${ECM}_705.sh
    cp -r $Script_Dir/jobOptions_rec_sig_D2_2460_D_mDIY_tempE_705.sh jobOptions_rec_sig_D2_2460_D_mDIY_${ECM}_705.sh
    threshold=4.12
    sh jobOptions_sim_sig_D2_2460_D_mDIY_${ECM}_705.sh 0 19 $ECM ${CMS[$COUNT]} 5000 $threshold ${RUNNO_LOW[$COUNT]} ${RUNNO_UP[$COUNT]}
    sh jobOptions_rec_sig_D2_2460_D_mDIY_${ECM}_705.sh 0 19 $ECM
    rm -rf /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/rtraw/D2_2460/$ECM/*.rtraw
    rm -rf /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/dst/D2_2460/$ECM/*.dst
    cp -rf /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/gen_script/gen_mc/subSimRec.sh ./
    sh subSimRec.sh jobOptions_sim_sig_D2_2460_D_mDIY_$ECM jobOptions_rec_sig_D2_2460_D_mDIY_$ECM subSimRec_D2_2460_$ECM 0 19
    COUNT=$((${COUNT}+1))
done
