#ifndef Physics_Analysis_DDecay_H
#define Physics_Analysis_DDecay_H 

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/LoadFactoryEntries.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PropertyMgr.h"
#include "DatabaseSvc/IDatabaseSvc.h"

// don't know their functions
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "EventModel/Event.h"
/////////////////////////////

#include "EventModel/EventModel.h"
#include "EventModel/EventHeader.h"

#include "EvtRecEvent/EvtRecEvent.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "EvtRecEvent/EvtRecDTag.h"
#include "EvtRecEvent/EvtRecPi0.h"
#include "EvtRecEvent/EvtRecVeeVertex.h"
#include "MdcRecEvent/RecMdcKalTrack.h"
#include "DTagTool/DTagTool.h"

#include "VertexFit/KinematicFit.h"
#include "VertexFit/VertexFit.h"
#include "VertexFit/IVertexDbSvc.h"
#include "VertexFit/SecondVertexFit.h"
#include "VertexFit/Helix.h"
#include "VertexFit/WTrackParameter.h"
#include "ParticleID/ParticleID.h"

#include "McTruth/McParticle.h"

#include "TMath.h"
#include "TLorentzVector.h"
#include <vector>
#include <TTree.h>

#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/ThreeVector.h"
using CLHEP::Hep3Vector;
using CLHEP::HepLorentzVector;
typedef std::vector<HepLorentzVector> Vp4;
typedef std::vector<int> Vint;
typedef std::vector<WTrackParameter> VWTrkPara;

class DDecay : public Algorithm {

    public:
        DDecay(const std::string& name, ISvcLocator* pSvcLocator);
        StatusCode initialize();
        StatusCode execute();
        StatusCode finalize();  

    private:
        std::vector<int> m_DModes;
        bool m_isMonteCarlo;
        bool m_pid;
        bool m_debug;
        double m_Ecms;

        // judgement variables
        bool stat_DTagTool;
        bool stat_tagSD;
        bool stat_saveCandD;
        bool stat_saveOthertrks;
        bool stat_saveOthershws;
        
        // common info
        int runNo;
        int evtNo;
        long flag1;

        // All McTruth info
        Vp4 pAll;
        Vint pdg;
        Vint mother;
        double p4_D_MCTruth[4];
        double p4_Dbar_MCTruth[4];
        double p4_pi_MCTruth[4];
        int pdgid[100];
        int motheridx[100];
        int idxmc;

        // DpDm McTruth info
        int DpId;
        int DmId;

        // DTagTool
        EvtRecDTagCol::iterator dtag_iter_end;
        EvtRecDTagCol::iterator dtag_iter_begin;
        EvtRecDTagCol::iterator dtag_iter;
        int n_trkD;
        int n_shwD;
        int mode;
        int MODE;
        int charm;
        double chi2_vf;
        VertexParameter birth;
        double chi2_kf;
        double chi2_kf_STDDmiss_sig_D;
        double chi2_kf_STDDmiss_sig_Dst;
        double chi2_kf_STDDmiss_bkg_D;
        double chi2_kf_STDDmiss_bkg_Dst;
        double rawp4_othershw[50][4];
        double mDcand;
        double mDcand_miss;
        int n_count;
        VWTrkPara vwtrkpara_othershws;

        // Ntuple1 info
        NTuple::Tuple* m_tuple1;
        NTuple::Item<int> m_runNo;
        NTuple::Item<int> m_evtNo;
        NTuple::Item<int> m_flag1;
        NTuple::Item<int> m_n_trkD;
        NTuple::Matrix<double> m_rawp4_Dtrk;
        NTuple::Matrix<double> m_p4_Dtrk;
        NTuple::Item<int> m_n_shwD;
        NTuple::Matrix<double> m_rawp4_Dshw;
        NTuple::Matrix<double> m_p4_Dshw;
        NTuple::Item<int> m_mode;
        NTuple::Item<int> m_charm;
        NTuple::Item<double> m_chi2_vf;
        NTuple::Item<double> m_chi2_kf;
        NTuple::Item<int> m_n_count;
        NTuple::Item<int> m_n_othertrks;
        NTuple::Matrix<double> m_rawp4_otherMdctrk;
        NTuple::Matrix<double> m_rawp4_otherMdcKaltrk;
        NTuple::Item<int> m_n_othershws;
        NTuple::Matrix<double> m_rawp4_othershw;
        NTuple::Item<int> m_idxmc;
        NTuple::Array<int> m_pdgid;
        NTuple::Array<int> m_motheridx;
        NTuple::Item<int> m_Id_Dp;
        NTuple::Item<int> m_Id_Dm;
        NTuple::Array<double> m_p4_D_MCTruth;
        NTuple::Array<double> m_p4_Dbar_MCTruth;
        NTuple::Array<double> m_p4_pi_MCTruth;

        // Ntuple2 info
        NTuple::Tuple* m_tuple2;
        NTuple::Item<int> m_runNo_STDDmiss_sig_D;
        NTuple::Item<int> m_evtNo_STDDmiss_sig_D;
        NTuple::Item<int> m_flag1_STDDmiss_sig_D;
        NTuple::Item<int> m_n_trkD_STDDmiss_sig_D;
        NTuple::Matrix<double> m_rawp4_Dtrk_STDDmiss_sig_D;
        NTuple::Matrix<double> m_p4_Dtrk_STDDmiss_sig_D;
        NTuple::Item<int> m_n_shwD_STDDmiss_sig_D;
        NTuple::Matrix<double> m_rawp4_Dshw_STDDmiss_sig_D;
        NTuple::Matrix<double> m_p4_Dshw_STDDmiss_sig_D;
        NTuple::Array<double> m_rawp4_tagpi_STDDmiss_sig_D;
        NTuple::Array<double> m_p4_pi_STDDmiss_sig_D;
        NTuple::Array<double> m_p4_Dmiss_STDDmiss_sig_D;
        NTuple::Item<int> m_mode_STDDmiss_sig_D;
        NTuple::Item<int> m_charm_STDDmiss_sig_D;
        NTuple::Item<double> m_chi2_vf_STDDmiss_sig_D;
        NTuple::Item<double> m_chi2_kf_STDDmiss_sig_D;
        NTuple::Item<double> m_rm_Dpi_STDDmiss_sig_D;
        NTuple::Item<int> m_n_count_STDDmiss_sig_D;
        NTuple::Item<int> m_n_othertrks_STDDmiss_sig_D;
        NTuple::Matrix<double> m_rawp4_otherMdctrk_STDDmiss_sig_D;
        NTuple::Matrix<double> m_rawp4_otherMdcKaltrk_STDDmiss_sig_D;
        NTuple::Item<int> m_n_othershws_STDDmiss_sig_D;
        NTuple::Matrix<double> m_rawp4_othershw_STDDmiss_sig_D;
        NTuple::Item<int> m_idxmc_STDDmiss_sig_D;
        NTuple::Array<int> m_pdgid_STDDmiss_sig_D;
        NTuple::Array<int> m_motheridx_STDDmiss_sig_D;
        NTuple::Item<int> m_Id_Dp_STDDmiss_sig_D;
        NTuple::Item<int> m_Id_Dm_STDDmiss_sig_D;
        NTuple::Item<int> m_n_trk_STDDmiss_sig_D; 
        NTuple::Matrix<double> m_pid_trk_STDDmiss_sig_D;
        NTuple::Array<double> m_p4_D_MCTruth_STDDmiss_sig_D;
        NTuple::Array<double> m_p4_Dbar_MCTruth_STDDmiss_sig_D;
        NTuple::Array<double> m_p4_pi_MCTruth_STDDmiss_sig_D;

        // Ntuple3 info
        NTuple::Tuple* m_tuple3;
        NTuple::Item<int> m_runNo_STDDmiss_sig_Dst;
        NTuple::Item<int> m_evtNo_STDDmiss_sig_Dst;
        NTuple::Item<int> m_flag1_STDDmiss_sig_Dst;
        NTuple::Item<int> m_n_trkD_STDDmiss_sig_Dst;
        NTuple::Matrix<double> m_rawp4_Dtrk_STDDmiss_sig_Dst;
        NTuple::Matrix<double> m_p4_Dtrk_STDDmiss_sig_Dst;
        NTuple::Item<int> m_n_shwD_STDDmiss_sig_Dst;
        NTuple::Matrix<double> m_rawp4_Dshw_STDDmiss_sig_Dst;
        NTuple::Matrix<double> m_p4_Dshw_STDDmiss_sig_Dst;
        NTuple::Array<double> m_rawp4_tagpi_STDDmiss_sig_Dst;
        NTuple::Array<double> m_p4_pi_STDDmiss_sig_Dst;
        NTuple::Array<double> m_p4_Dmiss_STDDmiss_sig_Dst;
        NTuple::Item<int> m_mode_STDDmiss_sig_Dst;
        NTuple::Item<int> m_charm_STDDmiss_sig_Dst;
        NTuple::Item<double> m_chi2_vf_STDDmiss_sig_Dst;
        NTuple::Item<double> m_chi2_kf_STDDmiss_sig_Dst;
        NTuple::Item<double> m_rm_Dpi_STDDmiss_sig_Dst;
        NTuple::Item<int> m_n_count_STDDmiss_sig_Dst;
        NTuple::Item<int> m_n_othertrks_STDDmiss_sig_Dst;
        NTuple::Matrix<double> m_rawp4_otherMdctrk_STDDmiss_sig_Dst;
        NTuple::Matrix<double> m_rawp4_otherMdcKaltrk_STDDmiss_sig_Dst;
        NTuple::Item<int> m_n_othershws_STDDmiss_sig_Dst;
        NTuple::Matrix<double> m_rawp4_othershw_STDDmiss_sig_Dst;
        NTuple::Item<int> m_idxmc_STDDmiss_sig_Dst;
        NTuple::Array<int> m_pdgid_STDDmiss_sig_Dst;
        NTuple::Array<int> m_motheridx_STDDmiss_sig_Dst;
        NTuple::Item<int> m_Id_Dp_STDDmiss_sig_Dst;
        NTuple::Item<int> m_Id_Dm_STDDmiss_sig_Dst;
        NTuple::Item<int> m_n_trk_STDDmiss_sig_Dst; 
        NTuple::Matrix<double> m_pid_trk_STDDmiss_sig_Dst;

        // Ntuple4 info
        NTuple::Tuple* m_tuple4;
        NTuple::Item<int> m_runNo_STDDmiss_bkg_D;
        NTuple::Item<int> m_evtNo_STDDmiss_bkg_D;
        NTuple::Item<int> m_flag1_STDDmiss_bkg_D;
        NTuple::Item<int> m_n_trkD_STDDmiss_bkg_D;
        NTuple::Matrix<double> m_rawp4_Dtrk_STDDmiss_bkg_D;
        NTuple::Matrix<double> m_p4_Dtrk_STDDmiss_bkg_D;
        NTuple::Item<int> m_n_shwD_STDDmiss_bkg_D;
        NTuple::Matrix<double> m_rawp4_Dshw_STDDmiss_bkg_D;
        NTuple::Matrix<double> m_p4_Dshw_STDDmiss_bkg_D;
        NTuple::Array<double> m_rawp4_tagpi_STDDmiss_bkg_D;
        NTuple::Array<double> m_p4_pi_STDDmiss_bkg_D;
        NTuple::Array<double> m_p4_Dmiss_STDDmiss_bkg_D;
        NTuple::Item<int> m_mode_STDDmiss_bkg_D;
        NTuple::Item<int> m_charm_STDDmiss_bkg_D;
        NTuple::Item<double> m_chi2_vf_STDDmiss_bkg_D;
        NTuple::Item<double> m_chi2_kf_STDDmiss_bkg_D;
        NTuple::Item<double> m_rm_Dpi_STDDmiss_bkg_D;
        NTuple::Item<int> m_n_count_STDDmiss_bkg_D;
        NTuple::Item<int> m_n_othertrks_STDDmiss_bkg_D;
        NTuple::Matrix<double> m_rawp4_otherMdctrk_STDDmiss_bkg_D;
        NTuple::Matrix<double> m_rawp4_otherMdcKaltrk_STDDmiss_bkg_D;
        NTuple::Item<int> m_n_othershws_STDDmiss_bkg_D;
        NTuple::Matrix<double> m_rawp4_othershw_STDDmiss_bkg_D;
        NTuple::Item<int> m_idxmc_STDDmiss_bkg_D;
        NTuple::Array<int> m_pdgid_STDDmiss_bkg_D;
        NTuple::Array<int> m_motheridx_STDDmiss_bkg_D;
        NTuple::Item<int> m_Id_Dp_STDDmiss_bkg_D;
        NTuple::Item<int> m_Id_Dm_STDDmiss_bkg_D;

        // Ntuple5 info
        NTuple::Tuple* m_tuple5;
        NTuple::Item<int> m_runNo_STDDmiss_bkg_Dst;
        NTuple::Item<int> m_evtNo_STDDmiss_bkg_Dst;
        NTuple::Item<int> m_flag1_STDDmiss_bkg_Dst;
        NTuple::Item<int> m_n_trkD_STDDmiss_bkg_Dst;
        NTuple::Matrix<double> m_rawp4_Dtrk_STDDmiss_bkg_Dst;
        NTuple::Matrix<double> m_p4_Dtrk_STDDmiss_bkg_Dst;
        NTuple::Item<int> m_n_shwD_STDDmiss_bkg_Dst;
        NTuple::Matrix<double> m_rawp4_Dshw_STDDmiss_bkg_Dst;
        NTuple::Matrix<double> m_p4_Dshw_STDDmiss_bkg_Dst;
        NTuple::Array<double> m_rawp4_tagpi_STDDmiss_bkg_Dst;
        NTuple::Array<double> m_p4_pi_STDDmiss_bkg_Dst;
        NTuple::Array<double> m_p4_Dmiss_STDDmiss_bkg_Dst;
        NTuple::Item<int> m_mode_STDDmiss_bkg_Dst;
        NTuple::Item<int> m_charm_STDDmiss_bkg_Dst;
        NTuple::Item<double> m_chi2_vf_STDDmiss_bkg_Dst;
        NTuple::Item<double> m_chi2_kf_STDDmiss_bkg_Dst;
        NTuple::Item<double> m_rm_Dpi_STDDmiss_bkg_Dst;
        NTuple::Item<int> m_n_count_STDDmiss_bkg_Dst;
        NTuple::Item<int> m_n_othertrks_STDDmiss_bkg_Dst;
        NTuple::Matrix<double> m_rawp4_otherMdctrk_STDDmiss_bkg_Dst;
        NTuple::Matrix<double> m_rawp4_otherMdcKaltrk_STDDmiss_bkg_Dst;
        NTuple::Item<int> m_n_othershws_STDDmiss_bkg_Dst;
        NTuple::Matrix<double> m_rawp4_othershw_STDDmiss_bkg_Dst;
        NTuple::Item<int> m_idxmc_STDDmiss_bkg_Dst;
        NTuple::Array<int> m_pdgid_STDDmiss_bkg_Dst;
        NTuple::Array<int> m_motheridx_STDDmiss_bkg_Dst;
        NTuple::Item<int> m_Id_Dp_STDDmiss_bkg_Dst;
        NTuple::Item<int> m_Id_Dm_STDDmiss_bkg_Dst;

        // functions
        void clearVariables();
        void recordVariables();
        void recordVariables_STDDmiss_sig_D();
        void recordVariables_STDDmiss_sig_Dst();
        void recordVariables_STDDmiss_bkg_D();
        void recordVariables_STDDmiss_bkg_Dst();
        void saveAllMcTruthInfo();
        void saveDpDmMcTruthInfo();
        bool useDTagTool();
        bool tagSingleD();
        bool saveCandD(VWTrkPara &vwtrkpara_charge, VWTrkPara &vwtrkpara_photon);
        double fitVertex(VWTrkPara &vwtrkpara, VertexParameter &birth_photon);
        double fitKM(VWTrkPara &vwtrkpara_charge, VWTrkPara &vwtrkpara_photon, VertexParameter &birth);
        bool saveOthertrks(VWTrkPara &vwtrkpara_charge, VWTrkPara &vwtrkpara_photon, VertexParameter &birth);
        bool saveOthershws();
        double ECMS(int runNo);
        bool judge_continue(int mode, int charm, double charge_par, double type_par, std::string sample_type, std::string Dmiss_type);
        double fitKM_STDDmiss_sig_D(VWTrkPara &vwtrkpara_charge, VWTrkPara &vwtrkpara_photon, VWTrkPara &vwtrkpara_pi, int n_pi, VertexParameter &birth);
        double fitKM_STDDmiss_sig_Dst(VWTrkPara &vwtrkpara_charge, VWTrkPara &vwtrkpara_photon, VWTrkPara &vwtrkpara_pi, int n_pi, VertexParameter &birth);
        double fitKM_STDDmiss_bkg_D(VWTrkPara &vwtrkpara_charge, VWTrkPara &vwtrkpara_photon, VWTrkPara &vwtrkpara_pi, int n_pi, VertexParameter &birth);
        double fitKM_STDDmiss_bkg_Dst(VWTrkPara &vwtrkpara_charge, VWTrkPara &vwtrkpara_photon, VWTrkPara &vwtrkpara_pi, int n_pi, VertexParameter &birth);
};
#endif
