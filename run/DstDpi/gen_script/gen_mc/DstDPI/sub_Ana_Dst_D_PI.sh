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
    mkdir -p $WORKAREA/run/DstDpi/rootfile/mc/DstDPI/$ECM
    mkdir -p $WORKAREA/run/DstDpi/jobs_text/mc/DstDPI/$ECM
    cd $WORKAREA/run/DstDpi/jobs_text/mc/DstDPI/$ECM
    rm -rf mc_Dst_D_PI_PHSP_$ECM*txt
    cp -rf $WORKAREA/python/make_mc.py ./
    cp -rf $WORKAREA/python/tools.py ./
    ./make_mc.py /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/dst/mc/DstDPI/$ECM mc Dst_D_PI PHSP DstDPI $ECM 2
    cp -rf /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/gen_script/gen_mc/subAna.sh ./
    rm -rf *boss*
    rm -rf $WORKAREA/run/DstDpi/rootfile/mc/DstDPI/$ECM/*root
    ./subAna.sh $ECM Dst_D_PI_PHSP
done
