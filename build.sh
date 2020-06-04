#!/usr/bin/env bash

# Main driver to build programs
# Author SHI Xin <shixin@ihep.ac.cn>
# Modified by JING Maoqiang <jingmq@ihep.ac.cn>
# Created [2019-12-11 Dec 14:50]


usage() {
    printf "NAME\n\tbuild.sh - Main driver to build programs\n"
    printf "\nSYNOPSIS\n"
    printf "\n\t%-5s\n" "./build.sh [OPTION]" 
    printf "\nOPTIONS\n" 
    printf "\n\t%-9s  %-40s"  "0.0"      "[set up useful tools]"
    printf "\n\t%-9s  %-40s"  "0.1"      "[build DDecayAlg analyzer]"
    printf "\n\n" 
}

usage_0_0() {
    printf "\n\t%-9s  %-40s"  ""         ""   
    printf "\n\t%-9s  %-40s"  "0.0.1"    "Build DIY psi(4260) --> D2D generator"
    printf "\n\t%-9s  %-40s"  "0.0.2"    "Build "
    printf "\n"
}

usage_0_1() {
    printf "\n\t%-9s  %-40s"  ""         ""   
    printf "\n\t%-9s  %-40s"  "0.1.1"    "Build DDECAYALGROOT module(01) ---- haven't added missing track"
    printf "\n\t%-9s  %-40s"  "0.1.2"    "Build DDECAYALGROOT module(02) ---- add missing track (wrong signed background)"
    printf "\n\t%-9s  %-40s"  "0.1.3"    "Build DDECAYALGROOT module(03) ---- add missing track (sideband)"
    printf "\n\t%-9s  %-40s"  ""         ""
    printf "\n"
}

if [[ $# -eq 0 ]]; then
    usage
    echo "Please enter your option: "
    read option
else
    option=$1    
fi

sub_0_0() {

case $option in
    
    # --------------------------------------------------------------------------
    #  Useful tools 
    # --------------------------------------------------------------------------

    0.0.1) echo "Building DIY psi(4260) --> D2D generator ..."
           mkdir -p ./Generator/BesEvtGen
           cp /afs/ihep.ac.cn/bes3/offline/Boss/7.0.3.p01/Generator/BesEvtGen/BesEvtGen-00-03-98 ./Generator/BesEvtGen/ -rf
           cp ./DIY/D2D/UsermDIY.cc ./Generator/BesEvtGen/BesEvtGen-00-03-98/user
           cd ./Generator/BesEvtGen/BesEvtGen-00-03-98/cmt
           cmt broadcast cmt config
           cmt broadcast make
	       ;;

    0.0.2) echo "Building  ..."
	       ;;

esac

}

sub_0_1() {

case $option in
    
    # --------------------------------------------------------------------------
    #  TESTPIALGROOT module
    # --------------------------------------------------------------------------

    0.1.1) echo "Building DDECAYALGROOT module(01) ..."
           rm -rf ./Analysis/Physics/DDecayAlg/DDecayAlg-00-00-01/x86_*
           cd ./Analysis/Physics/DDecayAlg/DDecayAlg-00-00-01/cmt
           cmt config
           gmake
           source setup.sh
	       ;;

    0.1.2) echo "Building DDECAYALGROOT module(02) ..."
           rm -rf ./Analysis/Physics/DDecayAlg/DDecayAlg-00-00-02/x86_*
           cd ./Analysis/Physics/DDecayAlg/DDecayAlg-00-00-02/cmt
           cmt config
           gmake
           source setup.sh
	       ;;

    0.1.3) echo "Building DDECAYALGROOT module(03) ..."
           rm -rf ./Analysis/Physics/DDecayAlg/DDecayAlg-00-00-03/x86_*
           cd ./Analysis/Physics/DDecayAlg/DDecayAlg-00-00-03/cmt
           cmt config
           gmake
           source setup.sh
	       ;;

esac

}

case $option in
    
    # --------------------------------------------------------------------------
    #  Useful tools 
    # --------------------------------------------------------------------------

    0.0) echo "Setting up useful tools..."
         usage_0_0
         echo "Please enter your option: " 
         read option  
         sub_0_0 option 
	     ;;

    0.0.*) echo "Building useful tools..."
           sub_0_0 option  
           ;;  

    # --------------------------------------------------------------------------
    #  TESTPIALGROOT module 
    # --------------------------------------------------------------------------

    0.1) echo "Building DDECAYALGROOT module..."
         usage_0_1 
         echo "Please enter your option: " 
         read option  
         sub_0_1 option 
	     ;;

    0.1.*) echo "Building DDECAYALGROOT module..."
           sub_0_1 option  
           ;;  
        
esac
