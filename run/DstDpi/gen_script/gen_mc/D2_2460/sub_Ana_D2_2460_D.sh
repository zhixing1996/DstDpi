#!/bin/sh
BOSS=$1
if [ "$BOSS" = "703" ]; then
    ECMS=("4600")
fi
if [ "$BOSS" = "705" ]; then
    ECMS=("4620" "4640" "4660" "4680" "4700")
fi

WORKAREA=/besfs/groups/cal/dedx/$USER/bes/DstDpi
for ECM in ${ECMS[@]}; do
    mkdir -p $WORKAREA/run/DstDpi/rootfile/mc/D2_2460/$ECM
    mkdir -p $WORKAREA/run/DstDpi/jobs_text/mc/D2_2460/$ECM
    cd $WORKAREA/run/DstDpi/jobs_text/mc/D2_2460/$ECM
    rm -rf mc_D2_2460_D_mDIY_$ECM*txt
    cp -rf $WORKAREA/python/make_mc.py ./
    cp -rf $WORKAREA/python/tools.py ./
    ./make_mc.py /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/dst/mc/D2_2460/$ECM mc D2_2460_D mDIY D2_2460 $ECM 2
    cp -rf /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/gen_script/gen_mc/subAna.sh ./
    rm -rf *boss*
    rm -rf $WORKAREA/run/DstDpi/rootfile/mc/D2_2460/$ECM/*root
    ./subAna.sh $ECM D2_2460_D_mDIY
done
