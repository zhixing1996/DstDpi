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
    printf "\n\t%-9s  %-40s"  "0.3"       "[analyze ee-->D(*)Dpi]"
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
    printf "\n\t%-9s  %-40s"  "0.3.1"     "Plot RM(Dpi) distributions"
    printf "\n\t%-9s  %-40s"  "0.3.2"     "Plot M(Dpi) distributions"
    printf "\n\t%-9s  %-40s"  "0.3.3"     "Plot RM2(D) versus M2(Dpi)"
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
           chmod u+x Gen_FileList_703
           bash Gen_FileList_703
           rm -r Gen_FileList_703
           cd /besfs/groups/cal/dedx/$USER/bes/DstDpi
	       ;;

    0.1.2) echo "Generate Condor jobs on data ---- 1..." 
	       cd run/DstDpi/gen_script
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
           mv Get_Info_703 ../logfile
           cd ../logfile
           chmod u+x Get_Info_703
           mkdir -p jobs.out jobs.err
           hep_sub -g physics Get_Info_703 -o jobs.out -e jobs.err
	       ;;

    0.1.7) echo "Apply cuts ..."
           cd ./run/DstDpi/gen_script
           ./apply_cuts_DstDpi.sh 703
           chmod u+x Apply_Cuts_703
           bash Apply_Cuts_703
           rm -r Apply_Cuts_703
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
           chmod u+x Gen_FileList_705
           bash Gen_FileList_705
           rm -r Gen_FileList_705
           cd /besfs/groups/cal/dedx/$USER/bes/DstDpi
	       ;;

    0.2.2) echo "Generate Condor jobs on data ---- 1..." 
	       cd run/DstDpi/gen_script
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
           mv Get_Info_705 ../logfile
           cd ../logfile
           chmod u+x Get_Info_705
           mkdir -p jobs.out jobs.err
           hep_sub -g physics Get_Info_705 -o jobs.out -e jobs.err
	       ;;

    0.2.7) echo "Apply cuts ..."
           cd ./run/DstDpi/gen_script
           ./apply_cuts_DstDpi.sh 705
           chmod u+x Apply_Cuts_705
           bash Apply_Cuts_705
           rm -r Apply_Cuts_705
           cd /besfs/groups/cal/dedx/$USER/bes/DstDpi
	       ;;

esac

}

sub_0_3() {

case $option in
    
    # --------------------------------------------------------------------------
    #  analysis
    # --------------------------------------------------------------------------

    0.3.1) echo "Plot RM(Dpi) distributions ..."
           cd ./run/DstDpi/gen_script
           ./plot_rm_Dpi.sh
           chmod u+x Plot_rm_Dpi
           bash Plot_rm_Dpi
           rm -r Plot_rm_Dpi
           cd /besfs/groups/cal/dedx/$USER/bes/DstDpi
	       ;;

    0.3.2) echo "Plot M(Dpi) distributions ..."
           cd ./run/DstDpi/gen_script
           ./plot_m_Dpi.sh
           chmod u+x Plot_m_Dpi
           bash Plot_m_Dpi
           rm -r Plot_m_Dpi
           cd /besfs/groups/cal/dedx/$USER/bes/DstDpi
	       ;;

    0.3.3) echo "Plot RM2(D) versus M2(Dpi) ..."
           cd ./run/DstDpi/gen_script
           ./scatter_data.sh
           chmod u+x Scatter_Data
           bash Scatter_Data
           rm -r Scatter_Data
           cd /besfs/groups/cal/dedx/$USER/bes/DstDpi
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
    #  Analysis
    # --------------------------------------------------------------------------

    0.3) echo "Analyzing ee-->D(*)Dpi ..."
         usage_0_3 
         echo "Please enter your option: " 
         read option  
         sub_0_3 option 
	     ;;

    0.3.*) echo "Analyzing ee-->D(*)Dpi ..."
           sub_0_3 option  
           ;;  

esac
