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
        int pdgid[100];
        int motheridx[100];
        int idxmc;

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
        double chi2_kf_STDDmiss;
        double chi2_kf_STDDmiss_low;
        double chi2_kf_STDDmiss_up;
        double rawp4_othershw[50][4];
        double mDcand;
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

        // Ntuple2 info
        NTuple::Tuple* m_tuple2;
        NTuple::Item<int> m_runNo_STDDmiss;
        NTuple::Item<int> m_evtNo_STDDmiss;
        NTuple::Item<int> m_flag1_STDDmiss;
        NTuple::Item<int> m_n_trkD_STDDmiss;
        NTuple::Matrix<double> m_rawp4_Dtrk_STDDmiss;
        NTuple::Matrix<double> m_p4_Dtrkold_STDDmiss;
        NTuple::Matrix<double> m_p4_Dtrk_STDDmiss;
        NTuple::Matrix<double> m_p4_Dtrk_STDDmiss_low;
        NTuple::Matrix<double> m_p4_Dtrk_STDDmiss_up;
        NTuple::Item<int> m_n_shwD_STDDmiss;
        NTuple::Matrix<double> m_rawp4_Dshw_STDDmiss;
        NTuple::Matrix<double> m_p4_Dshwold_STDDmiss;
        NTuple::Matrix<double> m_p4_Dshw_STDDmiss;
        NTuple::Matrix<double> m_p4_Dshw_STDDmiss_low;
        NTuple::Matrix<double> m_p4_Dshw_STDDmiss_up;
        NTuple::Array<double> m_p4_pi_STDDmiss;
        NTuple::Array<double> m_p4_pi_STDDmiss_low;
        NTuple::Array<double> m_p4_pi_STDDmiss_up;
        NTuple::Array<double> m_p4_Dmiss_STDDmiss;
        NTuple::Array<double> m_p4_Dmiss_STDDmiss_low;
        NTuple::Array<double> m_p4_Dmiss_STDDmiss_up;
        NTuple::Item<int> m_mode_STDDmiss;
        NTuple::Item<int> m_charm_STDDmiss;
        NTuple::Item<double> m_chi2_vf_STDDmiss;
        NTuple::Item<double> m_chi2_kf_STDDmiss;
        NTuple::Item<double> m_chi2_kf_STDDmiss_low;
        NTuple::Item<double> m_chi2_kf_STDDmiss_up;
        NTuple::Item<int> m_idxmc_STDDmiss;
        NTuple::Array<int> m_pdgid_STDDmiss;
        NTuple::Array<int> m_motheridx_STDDmiss;
        NTuple::Item<int> m_n_othertrks_STDDmiss;
        NTuple::Matrix<double> m_rawp4_otherMdctrk_STDDmiss;
        NTuple::Matrix<double> m_rawp4_otherMdcKaltrk_STDDmiss;
        NTuple::Array<double> m_rawp4_tagPi_STDDmiss;
        NTuple::Item<int> m_n_othershws_STDDmiss;
        NTuple::Matrix<double> m_rawp4_othershw_STDDmiss;
        NTuple::Item<double> m_rm_Dpi_STDDmiss;

        // functions
        void clearVariables();
        void recordVariables();
        void recordVariables_STDDmiss();
        void saveAllMcTruthInfo();
        bool useDTagTool();
        bool tagSingleD();
        bool saveCandD(VWTrkPara &vwtrkpara_charge, VWTrkPara &vwtrkpara_photon);
        bool judge_continue(int mode, int charm, double charge_par, double type_par);
        double fitVertex(VWTrkPara &vwtrkpara, VertexParameter &birth_photon);
        double fitKM(VWTrkPara &vwtrkpara_charge, VWTrkPara &vwtrkpara_photon, VertexParameter &birth);
        double fitKM_STDDmiss(VWTrkPara &vwtrkpara_charge, VWTrkPara &vwtrkpara_photon, VWTrkPara &vwtrkpara_pi, int n_pi, VertexParameter &birth);
        double fitKM_STDDmiss_low(VWTrkPara &vwtrkpara_charge, VWTrkPara &vwtrkpara_photon, VWTrkPara &vwtrkpara_pi, int n_pi, VertexParameter &birth, double sidebandlow_mean);
        double fitKM_STDDmiss_up(VWTrkPara &vwtrkpara_charge, VWTrkPara &vwtrkpara_photon, VWTrkPara &vwtrkpara_pi, int n_pi, VertexParameter &birth, double sidebandup_mean);
        bool saveOthertrks(VWTrkPara &vwtrkpara_charge, VWTrkPara &vwtrkpara_photon, VertexParameter &birth);
        bool saveOthershws();
};
#endif
