#!/usr/bin/env bash

# Main driver to submit jobs 
# Author SHI Xin <shixin@ihep.ac.cn>
# Modified by JING Maoqiang <jingmq@ihep.ac.cn>
# Created [2019-12-11 Dec 14:56]

usage() {
    printf "NAME\n\tsubmit.sh - Main driver to submit jobs\n"
    printf "\nSYNOPSIS\n"
    printf "\n\t%-5s\n" "./submit.sh [OPTION]" 
    printf "\nOPTIONS\n" 
    printf "\n\t%-9s  %-40s"  "0.1"       "[run on data sample for DstDpi @703]"
    printf "\n\t%-9s  %-40s"  "0.2"       "[run on data sample for DstDpi @705]"
    printf "\n\t%-9s  %-40s"  "0.3"       "[run on inclusive MC sample for DstDpi @703]"
    printf "\n\t%-9s  %-40s"  "0.4"       "[run on  MC samples for DstDpi @705]"
    printf "\n\n" 
}

usage_0_1() {
    printf "\n\t%-9s  %-40s"  ""          ""
    printf "\n\t%-9s  %-40s"  "0.1.1"     "Split data sample with each group 20G"
    printf "\n\t%-9s  %-40s"  "0.1.2"     "Generate Condor jobs on data ---- 1"
    printf "\n\t%-9s  %-40s"  "0.1.3"     "Test for data"
    printf "\n\t%-9s  %-40s"  "0.1.4"     "Submit Condor jobs on data ---- 2"
    printf "\n\t%-9s  %-40s"  "0.1.5"     "Synthesize data root files"
    printf "\n\t%-9s  %-40s"  "0.1.6"     "Get necessary info"
    printf "\n\t%-9s  %-40s"  "0.1.7"     "Apply cuts"
    printf "\n\t%-9s  %-40s"  ""           ""
    printf "\n"
}

usage_0_2() {
    printf "\n\t%-9s  %-40s"  ""          ""
    printf "\n\t%-9s  %-40s"  "0.2.1"     "Split data sample with each group 20G"
    printf "\n\t%-9s  %-40s"  "0.2.2"     "Generate Condor jobs on data ---- 1"
    printf "\n\t%-9s  %-40s"  "0.2.3"     "Test for data"
    printf "\n\t%-9s  %-40s"  "0.2.4"     "Submit Condor jobs on data ---- 2"
    printf "\n\t%-9s  %-40s"  "0.2.5"     "Synthesize data root files"
    printf "\n\t%-9s  %-40s"  "0.2.6"     "Get necessary info"
    printf "\n\t%-9s  %-40s"  "0.2.7"     "Apply cuts"
    printf "\n\t%-9s  %-40s"  ""           ""
    printf "\n"
}

usage_0_3() {
    printf "\n\t%-9s  %-40s"  ""          ""
    printf "\n\t%-9s  %-40s"  "0.3.1"     "Split inclusive MC samples with each group 20G"
    printf "\n\t%-9s  %-40s"  "0.3.2"     "Generate Condor jobs on inclusive MC samples ---- 1"
    printf "\n\t%-9s  %-40s"  "0.3.3"     "Test for inclusive MC samples"
    printf "\n\t%-9s  %-40s"  "0.3.4"     "Submit Condor jobs on inclusive MC samples ---- 2"
    printf "\n\t%-9s  %-40s"  "0.3.5"     "Synthesize inclusive MC samples root files"
    printf "\n\t%-9s  %-40s"  "0.3.6"     "Get necessary info"
    printf "\n\t%-9s  %-40s"  "0.3.7"     "Apply cuts"
    printf "\n\t%-9s  %-40s"  ""           ""
    printf "\n"
}

usage_0_4() {
    printf "\n\t%-9s  %-40s"  ""          ""
    printf "\n\t%-9s  %-40s"  "0.4.1"     "Generate MC samples ---- Simulation && Reconstruction"
    printf "\n\t%-9s  %-40s"  "0.4.2"     "Generate MC samples ---- Event Selection"
    printf "\n\t%-9s  %-40s"  "0.4.3"     "Synthesize inclusive MC samples root files"
    printf "\n\t%-9s  %-40s"  "0.4.4"     "Get necessary info"
    printf "\n\t%-9s  %-40s"  "0.4.5"     "Apply cuts"
    printf "\n\t%-9s  %-40s"  ""           ""
    printf "\n"
}

if [[ $# -eq 0 ]]; then
    usage
    echo "Please enter your option: "
    read option
else
    option=$1
fi

sub_0_1() {

case $option in
    
    # --------------------------------------------------------------------------
    #  run on data sample for DstDpi @703 
    # --------------------------------------------------------------------------

    0.1.1) echo "Split data sample with each group 20G ..."
           cd ./run/DstDpi/gen_script
           ./make_file_list_Data_DstDpi.sh 703
           chmod u+x Gen_FileList_Data_703
           bash Gen_FileList_Data_703
           rm -r Gen_FileList_Data_703
           cd /besfs/groups/cal/dedx/$USER/bes/DstDpi
	       ;;

    0.1.2) echo "Generate Condor jobs on data ---- 1..." 
	       cd ./run/DstDpi/gen_script
	       ./make_jobOption_file_Data_DstDpi.sh 703
	       cd /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/jobs_text/data/4600
	       cp -r jobOptions_DstDpi_data_4600-1.txt jobOptions_DstDpi_data_4600-0.txt
           sed -i "s/ApplicationMgr\.EvtMax = -1/ApplicationMgr\.EvtMax = 20000/g" jobOptions_DstDpi_data_4600-0.txt
           sed -i "s/DstDpi_data_4600-1\.root/DstDpi_data_4600-0\.root/g" jobOptions_DstDpi_data_4600-0.txt
	       ;;

    0.1.3) echo "Test for data" 
           echo "have you changed test number?(yes / no)
           ./run/DstDpi/jobs_text/data/4600/jobOptions_DstDpi_data_4600-0.txt"
           read opt
           if [ $opt == "yes" ]
               then
               echo "now in yes"  
               cd ./run/DstDpi/jobs_text/data/4600
               boss.exe jobOptions_DstDpi_data_4600-0.txt
           else
               echo "Default value is 'no', please change test number."
           fi
           ;;

    0.1.4) echo "Submit Condor jobs on data ---- 2..." 
	       cd ./run/DstDpi/gen_script
           ./sub_jobOption_file_Data_DstDpi.sh 703
	       ;;

    0.1.5) echo "Synthesize data root files..."
	       cd ./run/DstDpi/gen_script
           ./syn_Data_DstDpi.sh 703
	       ;;

    0.1.6) echo "Get necessary info..."
	       cd ./run/DstDpi/gen_script
           ./get_info_Data_DstDpi.sh 703
           mv Get_Info_Data_703 ../logfile
           cd ../logfile
           chmod u+x Get_Info_Data_703
           mkdir -p jobs.out jobs.err
           hep_sub -g physics Get_Info_Data_703 -o jobs.out -e jobs.err
	       ;;

    0.1.7) echo "Apply cuts ..."
           cd ./run/DstDpi/gen_script
           ./apply_cuts_Data_DstDpi.sh 703
           mv Apply_Cuts_Data_703 ../logfile
           cd ../logfile
           chmod u+x Apply_Cuts_Data_703
           bash Apply_Cuts_Data_703
           rm -r Apply_Cuts_Data_703
           cd /besfs/groups/cal/dedx/$USER/bes/DstDpi
	       ;;

esac

}

sub_0_2() {

case $option in
    
    # --------------------------------------------------------------------------
    #  run on data sample for DstDpi @705 
    # --------------------------------------------------------------------------

    0.2.1) echo "Split data sample with each group 20G ..."
           cd ./run/DstDpi/gen_script
           ./make_file_list_Data_DstDpi.sh 705
           chmod u+x Gen_FileList_Data_705
           bash Gen_FileList_Data_705
           rm -r Gen_FileList_Data_705
           cd /besfs/groups/cal/dedx/$USER/bes/DstDpi
	       ;;

    0.2.2) echo "Generate Condor jobs on data ---- 1..." 
	       cd ./run/DstDpi/gen_script
	       ./make_jobOption_file_Data_DstDpi.sh 705
	       cd /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/jobs_text/data/4620
	       cp -r jobOptions_DstDpi_data_4620-1.txt jobOptions_DstDpi_data_4620-0.txt
           sed -i "s/ApplicationMgr\.EvtMax = -1/ApplicationMgr\.EvtMax = 20000/g" jobOptions_DstDpi_data_4620-0.txt
           sed -i "s/DstDpi_data_4620-1\.root/DstDpi_data_4620-0\.root/g" jobOptions_DstDpi_data_4620-0.txt
	       ;;

    0.2.3) echo "Test for data" 
           echo "have you changed test number?(yes / no)
           ./run/DstDpi/jobs_text/data/4620/jobOptions_DstDpi_data_4620-0.txt"
           read opt
           if [ $opt == "yes" ]
               then
               echo "now in yes"  
               cd ./run/DstDpi/jobs_text/data/4620
               boss.exe jobOptions_DstDpi_data_4620-0.txt
           else
               echo "Default value is 'no', please change test number."
           fi
           ;;

    0.2.4) echo "Submit Condor jobs on data ---- 2..." 
	       cd ./run/DstDpi/gen_script
           ./sub_jobOption_file_Data_DstDpi.sh 705
	       ;;

    0.2.5) echo "Synthesize data root files..."
	       cd ./run/DstDpi/gen_script
           ./syn_Data_DstDpi.sh 705
	       ;;

    0.2.6) echo "Get necessary info..."
	       cd ./run/DstDpi/gen_script
           ./get_info_Data_DstDpi.sh 705
           mv Get_Info_Data_705 ../logfile
           cd ../logfile
           chmod u+x Get_Info_Data_705
           mkdir -p jobs.out jobs.err
           hep_sub -g physics Get_Info_Data_705 -o jobs.out -e jobs.err
	       ;;

    0.2.7) echo "Apply cuts ..."
           cd ./run/DstDpi/gen_script
           ./apply_cuts_Data_DstDpi.sh 705
           mv Apply_Cuts_Data_705 ../logfile
           cd ../logfile
           chmod u+x Apply_Cuts_Data_705
           bash Apply_Cuts_Data_705
           rm -r Apply_Cuts_Data_705
           cd /besfs/groups/cal/dedx/$USER/bes/DstDpi
	       ;;

esac

}

sub_0_3() {

case $option in
    
    # --------------------------------------------------------------------------
    #  run on inclusive MC samples for DstDpi @703
    # --------------------------------------------------------------------------

    0.3.1) echo "Split inclusive MC samples with each group 20G ..."
           cd ./run/DstDpi/gen_script
           ./make_file_list_incMC_DstDpi.sh 703 DD
           chmod u+x Gen_FileList_incMC_703_DD
           bash Gen_FileList_incMC_703_DD
           rm -r Gen_FileList_incMC_703_DD
           ./make_file_list_incMC_DstDpi.sh 703 qq
           chmod u+x Gen_FileList_incMC_703_qq
           bash Gen_FileList_incMC_703_qq
           rm -r Gen_FileList_incMC_703_qq
           ./make_file_list_incMC_DstDpi.sh 703 LcLc
           chmod u+x Gen_FileList_incMC_703_LcLc
           bash Gen_FileList_incMC_703_LcLc
           rm -r Gen_FileList_incMC_703_LcLc
           ./make_file_list_incMC_DstDpi.sh 705 hadron
           chmod u+x Gen_FileList_incMC_705_hadron
           bash Gen_FileList_incMC_705_hadron
           rm -r Gen_FileList_incMC_705_hadron
           ./make_file_list_incMC_DstDpi.sh 705 LcLc
           chmod u+x Gen_FileList_incMC_705_LcLc
           bash Gen_FileList_incMC_705_LcLc
           rm -r Gen_FileList_incMC_705_LcLc
           cd /besfs/groups/cal/dedx/$USER/bes/DstDpi
	       ;;

    0.3.2) echo "Generate Condor jobs on inclusive MC samples ---- 1..." 
	       cd ./run/DstDpi/gen_script
	       ./make_jobOption_file_incMC_DstDpi.sh 703 DD
	       ./make_jobOption_file_incMC_DstDpi.sh 703 qq
	       ./make_jobOption_file_incMC_DstDpi.sh 703 LcLc
	       ./make_jobOption_file_incMC_DstDpi.sh 705 hadron
	       ./make_jobOption_file_incMC_DstDpi.sh 705 LcLc
	       cd /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/jobs_text/mc/DD/4600
	       cp -r jobOptions_DstDpi_incMC_DD_4600-1.txt jobOptions_DstDpi_incMC_DD_4600-0.txt
           sed -i "s/ApplicationMgr\.EvtMax = -1/ApplicationMgr\.EvtMax = 500/g" jobOptions_DstDpi_incMC_DD_4600-0.txt
           sed -i "s/DstDpi_incMC_DD_4600-1\.root/DstDpi_incMC_DD_4600-0\.root/g" jobOptions_DstDpi_incMC_DD_4600-0.txt
	       ;;

    0.3.3) echo "Test for inclusive MC samples" 
           echo "have you changed test number?(yes / no)
           ./run/DstDpi/jobs_text/mc/DD/4600/jobOptions_DstDpi_incMC_DD_4600-0.txt"
           read opt
           if [ $opt == "yes" ]
               then
               echo "now in yes"  
               cd ./run/DstDpi/jobs_text/mc/DD/4600
               boss.exe jobOptions_DstDpi_incMC_DD_4600-0.txt
           else
               echo "Default value is 'no', please change test number."
           fi
           ;;

    0.3.4) echo "Submit Condor jobs on inclusive MC ---- 2..." 
	       cd ./run/DstDpi/gen_script
           ./sub_jobOption_file_incMC_DstDpi.sh 703 DD
           ./sub_jobOption_file_incMC_DstDpi.sh 703 qq
           ./sub_jobOption_file_incMC_DstDpi.sh 703 LcLc
           ./sub_jobOption_file_incMC_DstDpi.sh 705 hadron
           ./sub_jobOption_file_incMC_DstDpi.sh 705 LcLc
	       ;;

    0.3.5) echo "Synthesize inclusive MC samples root files..."
	       cd ./run/DstDpi/gen_script
           ./syn_incMC_DstDpi.sh 703 DD
           ./syn_incMC_DstDpi.sh 703 qq
           ./syn_incMC_DstDpi.sh 703 LcLc
           ./syn_incMC_DstDpi.sh 705 hadron
           ./syn_incMC_DstDpi.sh 705 LcLc
	       ;;

    0.3.6) echo "Get necessary info..."
	       cd /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/gen_script
           ./get_info_incMC_DstDpi.sh 703 DD
           mv Get_Info_incMC_DD_703 ../logfile
           cd /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/logfile
           chmod u+x Get_Info_incMC_DD_703
           mkdir -p jobs.out jobs.err
           hep_sub -g physics Get_Info_incMC_DD_703 -o jobs.out -e jobs.err
	       cd /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/gen_script
           ./get_info_incMC_DstDpi.sh 703 qq
           mv Get_Info_incMC_qq_703 ../logfile
           cd /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/logfile
           chmod u+x Get_Info_incMC_qq_703
           mkdir -p jobs.out jobs.err
           hep_sub -g physics Get_Info_incMC_qq_703 -o jobs.out -e jobs.err
	       cd /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/gen_script
           ./get_info_incMC_DstDpi.sh 703 LcLc
           mv Get_Info_incMC_LcLc_703 ../logfile
           cd /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/logfile
           chmod u+x Get_Info_incMC_LcLc_703
           mkdir -p jobs.out jobs.err
           hep_sub -g physics Get_Info_incMC_LcLc_703 -o jobs.out -e jobs.err
	       cd /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/gen_script
           ./get_info_incMC_DstDpi.sh 705 hadron
           mv Get_Info_incMC_hadron_705 ../logfile
           cd /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/logfile
           chmod u+x Get_Info_incMC_hadron_705
           mkdir -p jobs.out jobs.err
           hep_sub -g physics Get_Info_incMC_hadron_705 -o jobs.out -e jobs.err
	       cd /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/gen_script
           ./get_info_incMC_DstDpi.sh 705 LcLc
           mv Get_Info_incMC_LcLc_705 ../logfile
           cd /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/logfile
           chmod u+x Get_Info_incMC_LcLc_705
           mkdir -p jobs.out jobs.err
           hep_sub -g physics Get_Info_incMC_LcLc_705 -o jobs.out -e jobs.err
	       ;;

    0.3.7) echo "Apply cuts ..."
           cd ./run/DstDpi/gen_script
           ./apply_cuts_incMC_DstDpi.sh 703 DD
           chmod u+x Apply_Cuts_DD_703
           bash Apply_Cuts_DD_703
           rm -r Apply_Cuts_DD_703
           ./apply_cuts_incMC_DstDpi.sh 705 hadron
           chmod u+x Apply_Cuts_hadron_705
           bash Apply_Cuts_hadron_705
           rm -r Apply_Cuts_hadron_705
           ./apply_cuts_incMC_DstDpi.sh 705 LcLc
           chmod u+x Apply_Cuts_LcLc_705
           bash Apply_Cuts_LcLc_705
           rm -r Apply_Cuts_LcLc_705
           cd /besfs/groups/cal/dedx/$USER/bes/DstDpi
	       ;;

esac

}

sub_0_4() {

case $option in
    
    # --------------------------------------------------------------------------
    #  run on MC samples for DstDpi @705 
    #      D2_2460 --> e+e- --> DD2(2460); DD2(2460) --> Dpi
    #      DDPI    --> e+e- --> D-D0-barpi+
    #      DDstPI  --> e+e- --> D+D0*-barpi-
    #      DstDPI  --> e+e- --> D*-D0pi+
    # --------------------------------------------------------------------------

    0.4.1) echo "Generate MC samples ---- Simulation && Reconstruction ..."
           echo "which MC sample do you want to simulate?"
           read opt
           if [ $opt == "D2_2460" ]; then
               cd /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/gen_script/gen_mc/D2_2460
               ./sub_jobOption_D2_2460_D.sh 703
               echo "Please set BOSS-705!!!!!!"
               ./sub_jobOption_D2_2460_D.sh 705
           elif [ $opt == "DDPI" ]; then
               cd /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/gen_script/gen_mc/DDPI
               ./sub_jobOption_D_D_PI.sh 703
               echo "Please set BOSS-705!!!!!!"
               ./sub_jobOption_D_D_PI.sh 705
           elif [ $opt == "DDstPI" ]; then
               cd /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/gen_script/gen_mc/DDstPI
               ./sub_jobOption_D_Dst_PI.sh 703
               echo "Please set BOSS-705!!!!!!"
               ./sub_jobOption_D_Dst_PI.sh 705
           elif [ $opt == "DstDPI" ]; then
               cd /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/gen_script/gen_mc/DstDPI
               ./sub_jobOption_Dst_D_PI.sh 703
               echo "Please set BOSS-705!!!!!!"
               ./sub_jobOption_Dst_D_PI.sh 705
           else
               echo "Please add the MC simulation joboption files!"
           fi
	       ;;

    0.4.2) echo "Generate MC samples ---- Event Selection ..."
           echo "which MC sample do you want to select?"
           read opt
           if [ $opt == "D2_2460" ]; then
               cd /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/gen_script/gen_mc/D2_2460
               ./sub_Ana_D2_2460_D.sh 703
               echo "Please set BOSS-705!!!!!!"
               ./sub_Ana_D2_2460_D.sh 705
           elif [ $opt == "DDPI" ]; then
               cd /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/gen_script/gen_mc/DDPI
               ./sub_Ana_D_D_PI.sh 703
               echo "Please set BOSS-705!!!!!!"
               ./sub_Ana_D_D_PI.sh 705
           elif [ $opt == "DDstPI" ]; then
               cd /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/gen_script/gen_mc/DDstPI
               ./sub_Ana_D_Dst_PI.sh 703
               echo "Please set BOSS-705!!!!!!"
               ./sub_Ana_D_Dst_PI.sh 705
           elif [ $opt == "DstDPI" ]; then
               cd /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/gen_script/gen_mc/DstDPI
               ./sub_Ana_Dst_D_PI.sh 703
               echo "Please set BOSS-705!!!!!!"
               ./sub_Ana_Dst_D_PI.sh 705
           else
               echo "Please add the MC simulation joboption files!"
           fi
	       ;;

    0.4.3) echo "Synthesize inclusive MC samples root files ..."
           cd ./run/DstDpi/gen_script
           echo "which MC sample do you want to synthesize?"
           read opt
           if [ $opt == "D2_2460" ]; then
               ./syn_MC_DstDpi.sh 703 D2_2460
               ./syn_MC_DstDpi.sh 705 D2_2460
           elif [ $opt == "DDPI" ]; then
               ./syn_MC_DstDpi.sh 703 DDPI
               ./syn_MC_DstDpi.sh 705 DDPI
           elif [ $opt == "DDstPI" ]; then
               ./syn_MC_DstDpi.sh 703 DDstPI
               ./syn_MC_DstDpi.sh 705 DDstPI
           elif [ $opt == "DstDPI" ]; then
               ./syn_MC_DstDpi.sh 703 DstDPI
               ./syn_MC_DstDpi.sh 705 DstDPI
           else
               echo "Please add the MC simulation joboption files!"
           fi
	       ;;

    0.4.4) echo "Get necessary info..."
           echo "which MC sample's info do you want to get?"
           read opt
           if [ $opt == "D2_2460" ]; then
	           cd /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/gen_script
               ./get_info_MC_DstDpi.sh 703 D2_2460
               mv Get_Info_MC_D2_2460_703 ../logfile
               cd /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/logfile
               chmod u+x Get_Info_MC_D2_2460_703
               mkdir -p jobs.out jobs.err
               hep_sub -g physics Get_Info_MC_D2_2460_703 -o jobs.out -e jobs.err
	           cd /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/gen_script
               ./get_info_MC_DstDpi.sh 705 D2_2460
               mv Get_Info_MC_D2_2460_705 ../logfile
               cd /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/logfile
               chmod u+x Get_Info_MC_D2_2460_705
               mkdir -p jobs.out jobs.err
               hep_sub -g physics Get_Info_MC_D2_2460_705 -o jobs.out -e jobs.err
           elif [ $opt == "DDPI" ]; then
	           cd /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/gen_script
               ./get_info_MC_DstDpi.sh 703 DDPI
               mv Get_Info_MC_DDPI_703 ../logfile
               cd /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/logfile
               chmod u+x Get_Info_MC_DDPI_703
               mkdir -p jobs.out jobs.err
               hep_sub -g physics Get_Info_MC_DDPI_703 -o jobs.out -e jobs.err
	           cd /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/gen_script
               ./get_info_MC_DstDpi.sh 705 DDPI
               mv Get_Info_MC_DDPI_705 ../logfile
               cd /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/logfile
               chmod u+x Get_Info_MC_DDPI_705
               mkdir -p jobs.out jobs.err
               hep_sub -g physics Get_Info_MC_DDPI_705 -o jobs.out -e jobs.err
           elif [ $opt == "DDstPI" ]; then
	           cd /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/gen_script
               ./get_info_MC_DstDpi.sh 703 DDstPI
               mv Get_Info_MC_DDstPI_703 ../logfile
               cd /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/logfile
               chmod u+x Get_Info_MC_DDstPI_703
               mkdir -p jobs.out jobs.err
               hep_sub -g physics Get_Info_MC_DDstPI_703 -o jobs.out -e jobs.err
	           cd /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/gen_script
               ./get_info_MC_DstDpi.sh 705 DDstPI
               mv Get_Info_MC_DDstPI_705 ../logfile
               cd /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/logfile
               chmod u+x Get_Info_MC_DDstPI_705
               mkdir -p jobs.out jobs.err
               hep_sub -g physics Get_Info_MC_DDstPI_705 -o jobs.out -e jobs.err
           elif [ $opt == "DstDPI" ]; then
	           cd /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/gen_script
               ./get_info_MC_DstDpi.sh 703 DstDPI
               mv Get_Info_MC_DstDPI_703 ../logfile
               cd /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/logfile
               chmod u+x Get_Info_MC_DstDPI_703
               mkdir -p jobs.out jobs.err
               hep_sub -g physics Get_Info_MC_DstDPI_703 -o jobs.out -e jobs.err
	           cd /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/gen_script
               ./get_info_MC_DstDpi.sh 705 DstDPI
               mv Get_Info_MC_DstDPI_705 ../logfile
               cd /besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/logfile
               chmod u+x Get_Info_MC_DstDPI_705
               mkdir -p jobs.out jobs.err
               hep_sub -g physics Get_Info_MC_DstDPI_705 -o jobs.out -e jobs.err
           else
               echo "Please add the MC simulation joboption files!"
           fi
           ;;

    0.4.5) echo "Apply cuts ..."
           cd ./run/DstDpi/gen_script
           echo "which MC sample do you want to apply cuts?"
           read opt
           if [ $opt == "D2_2460" ]; then
               ./apply_cuts_MC_DstDpi.sh 703 D2_2460
               chmod u+x Apply_Cuts_D2_2460_703
               bash Apply_Cuts_D2_2460_703
               rm -r Apply_Cuts_D2_2460_703
               ./apply_cuts_MC_DstDpi.sh 705 D2_2460
               chmod u+x Apply_Cuts_D2_2460_705
               bash Apply_Cuts_D2_2460_705
               rm -r Apply_Cuts_D2_2460_705
           elif [ $opt == "DDPI" ]; then
               ./apply_cuts_MC_DstDpi.sh 703 DDPI
               chmod u+x Apply_Cuts_DDPI_703
               bash Apply_Cuts_DDPI_703
               rm -r Apply_Cuts_DDPI_703
               ./apply_cuts_MC_DstDpi.sh 705 DDPI
               chmod u+x Apply_Cuts_DDPI_705
               bash Apply_Cuts_DDPI_705
               rm -r Apply_Cuts_DDPI_705
           elif [ $opt == "DDstPI" ]; then
               ./apply_cuts_MC_DstDpi.sh 703 DDstPI
               chmod u+x Apply_Cuts_DDstPI_703
               bash Apply_Cuts_DDstPI_703
               rm -r Apply_Cuts_DDstPI_703
               ./apply_cuts_MC_DstDpi.sh 705 DDstPI
               chmod u+x Apply_Cuts_DDstPI_705
               bash Apply_Cuts_DDstPI_705
               rm -r Apply_Cuts_DDstPI_705
           elif [ $opt == "DstDPI" ]; then
               ./apply_cuts_MC_DstDpi.sh 703 DstDPI
               chmod u+x Apply_Cuts_DstDPI_703
               bash Apply_Cuts_DstDPI_703
               rm -r Apply_Cuts_DstDPI_703
               ./apply_cuts_MC_DstDpi.sh 705 DstDPI
               chmod u+x Apply_Cuts_DstDPI_705
               bash Apply_Cuts_DstDPI_705
               rm -r Apply_Cuts_DstDPI_705
           else
               echo "Please add the MC simulation joboption files!"
           fi
	       ;;

esac

}


case $option in
   
    # --------------------------------------------------------------------------
    #  Data @703 
    # --------------------------------------------------------------------------

    0.1) echo "Running on data sample @703..."
         usage_0_1 
         echo "Please enter your option: " 
         read option  
         sub_0_1 option 
	     ;;

    0.1.*) echo "Running on data sample @703..."
           sub_0_1 option  
           ;;  
        
    # --------------------------------------------------------------------------
    #  Data @705 
    # --------------------------------------------------------------------------

    0.2) echo "Running on data sample @705..."
         usage_0_2 
         echo "Please enter your option: " 
         read option  
         sub_0_2 option 
	     ;;

    0.2.*) echo "Running on data sample @705..."
           sub_0_2 option  
           ;;  

    # --------------------------------------------------------------------------
    #  Inclusive MC samples @703
    # --------------------------------------------------------------------------

    0.3) echo "Running on inclusive MC samples @703 ..."
         usage_0_3 
         echo "Please enter your option: " 
         read option  
         sub_0_3 option 
	     ;;

    0.3.*) echo "Running on inclusive MC samples @703 ..."
           sub_0_3 option  
           ;;  

    # --------------------------------------------------------------------------
    #  MC samples @705
    # --------------------------------------------------------------------------

    0.4) echo "Running on MC samples for DstDpi @705 ..."
         usage_0_4 
         echo "Please enter your option: " 
         read option  
         sub_0_4 option 
	     ;;

    0.4.*) echo "Running on MC samples for DstDpi @705 ..."
           sub_0_4 option  
           ;;  

esac
