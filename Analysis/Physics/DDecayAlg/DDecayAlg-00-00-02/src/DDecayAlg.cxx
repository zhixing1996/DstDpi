// -*- C++ -*- //
//
// Description: ee -> DD(2420) -> DDPIPI
//
// Original Author:  Maoqiang JING <jingmq@ihep.ac.cn>
//         Created:  [2019-08-08 Thu 15:09] 
//         Inspired by Suyu XIAO's code 
// 
//

#include "DDecayAlg.h"

const double PI = 3.1415927;
const double M_Pi0 = 0.1349766;
const double M_D0 = 1.86483;
const double M_D0st = 2.00685;
const double M_Dplus = 1.86965;
const double M_Dplusst = 2.01026;
const double mass[5] = {
    0.000511, 0.105658, 0.139570, 0.493677, 0.938272 // e, mu, pi, K, p
};
int Ncut0, Ncut1, Ncut2, Ncut3, Ncut4, Ncut5, Ncut6, Ncut7, Ncut8, Ncut9, Ncut10, Ncut11, Ncut12, Ncut13, Ncut14, Ncut15, Ncut16, Ncut17, Ncut18, Ncut19, Ncut20;

// 
// module declare
//

VertexFit * vtxfit = VertexFit::instance();
KinematicFit * kmfit = KinematicFit::instance();

DECLARE_ALGORITHM_FACTORY( DDecay )
DECLARE_FACTORY_ENTRIES( DDecayAlg ) {
    DECLARE_ALGORITHM( DDecay );
}

LOAD_FACTORY_ENTRIES( DDecayAlg )

DDecay::DDecay(const std::string& name, ISvcLocator* pSvcLocator) :
	Algorithm(name, pSvcLocator) {
        m_DModes.push_back(0);
        m_DModes.push_back(1);
        m_DModes.push_back(3);
        m_DModes.push_back(200);
        declareProperty("DMode", m_DModes);
        declareProperty("IsMonteCarlo", m_isMonteCarlo = true);
        declareProperty("UsePID", m_pid = true);
        declareProperty("Debug", m_debug = false);
        declareProperty("Ecms", m_Ecms = 4.600);
}

StatusCode DDecay::initialize() {
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << ">>>>>>> in initialize()" << endmsg;

    StatusCode status;

    NTuplePtr nt1(ntupleSvc(), "FILE1/STD");
    if (nt1) m_tuple1 = nt1;
    else {
        m_tuple1 = ntupleSvc()->book("FILE1/STD", CLID_ColumnWiseTuple, "Single tag D decay");
        if (m_tuple1) {
            status = m_tuple1->addItem("runNo", m_runNo);
            status = m_tuple1->addItem("evtNo", m_evtNo);
            status = m_tuple1->addItem("flag1", m_flag1);
            status = m_tuple1->addItem("n_trkD", m_n_trkD, 0, 5); // number of members should locates in 0~5
            status = m_tuple1->addIndexedItem("rawp4_Dtrk", m_n_trkD, 6, m_rawp4_Dtrk); // four members array
            status = m_tuple1->addIndexedItem("p4_Dtrk", m_n_trkD, 4, m_p4_Dtrk);
            status = m_tuple1->addItem("n_shwD", m_n_shwD, 0, 2); 
            status = m_tuple1->addIndexedItem("rawp4_Dshw", m_n_shwD, 4, m_rawp4_Dshw);
            status = m_tuple1->addIndexedItem("p4_Dshw", m_n_shwD, 4, m_p4_Dshw);
            status = m_tuple1->addItem("mode", m_mode);
            status = m_tuple1->addItem("charm", m_charm);
            status = m_tuple1->addItem("chi2_vf", m_chi2_vf);
            status = m_tuple1->addItem("chi2_kf", m_chi2_kf);
            status = m_tuple1->addItem("n_count", m_n_count); // multi-counting D in one event
            status = m_tuple1->addItem("n_othertrks", m_n_othertrks, 0, 20);
            status = m_tuple1->addIndexedItem("rawp4_otherMdctrk", m_n_othertrks, 6, m_rawp4_otherMdctrk);
            status = m_tuple1->addIndexedItem("rawp4_otherMdcKaltrk", m_n_othertrks, 6, m_rawp4_otherMdcKaltrk);
            status = m_tuple1->addItem("n_othershws", m_n_othershws, 0, 50);
            status = m_tuple1->addIndexedItem("rawp4_othershw", m_n_othershws, 4, m_rawp4_othershw);
            status = m_tuple1->addItem("indexmc", m_idxmc, 0, 100);
            status = m_tuple1->addIndexedItem("pdgid", m_idxmc, m_pdgid);
            status = m_tuple1->addIndexedItem("motheridx", m_idxmc, m_motheridx);
            status = m_tuple1->addItem("DpId", m_Id_Dp);
            status = m_tuple1->addItem("DmId", m_Id_Dm);
            status = m_tuple1->addItem("p4_pi_MCTruth", 4, m_p4_pi_MCTruth);
            status = m_tuple1->addItem("p4_D_MCTruth", 4, m_p4_D_MCTruth);
            status = m_tuple1->addItem("p4_Dbar_MCTruth", 4, m_p4_Dbar_MCTruth);
        }
        else {
            log << MSG::ERROR << "Cannot book N-tuple:" << long(m_tuple1) << endmsg;
            return StatusCode::FAILURE;
        }
    }

    NTuplePtr nt2(ntupleSvc(), "FILE1/STDDmiss_sig_D");
    if (nt2) m_tuple2 = nt2;
    else {
        m_tuple2 = ntupleSvc()->book("FILE1/STDDmiss_sig_D", CLID_ColumnWiseTuple, "Single tag D decay and miss D-bar track (signal sample)");
        if (m_tuple2) {
            status = m_tuple2->addItem("runNo", m_runNo_STDDmiss_sig_D);
            status = m_tuple2->addItem("evtNo", m_evtNo_STDDmiss_sig_D);
            status = m_tuple2->addItem("flag1", m_flag1_STDDmiss_sig_D);
            status = m_tuple2->addItem("n_trkD", m_n_trkD_STDDmiss_sig_D, 0, 5); // number of members should locates in 0~5
            status = m_tuple2->addIndexedItem("rawp4_Dtrk", m_n_trkD_STDDmiss_sig_D, 6, m_rawp4_Dtrk_STDDmiss_sig_D); // four members array
            status = m_tuple2->addIndexedItem("p4_Dtrk", m_n_trkD_STDDmiss_sig_D, 4, m_p4_Dtrk_STDDmiss_sig_D);
            status = m_tuple2->addItem("n_shwD", m_n_shwD_STDDmiss_sig_D, 0, 2); 
            status = m_tuple2->addIndexedItem("rawp4_Dshw", m_n_shwD_STDDmiss_sig_D, 4, m_rawp4_Dshw_STDDmiss_sig_D);
            status = m_tuple2->addIndexedItem("p4_Dshw", m_n_shwD_STDDmiss_sig_D, 4, m_p4_Dshw_STDDmiss_sig_D);
            status = m_tuple2->addItem("rawp4_tagpi", 4, m_rawp4_tagpi_STDDmiss_sig_D);
            status = m_tuple2->addItem("p4_pi", 4, m_p4_pi_STDDmiss_sig_D);
            status = m_tuple2->addItem("p4_Dmiss", 4, m_p4_Dmiss_STDDmiss_sig_D);
            status = m_tuple2->addItem("mode", m_mode_STDDmiss_sig_D);
            status = m_tuple2->addItem("charm", m_charm_STDDmiss_sig_D);
            status = m_tuple2->addItem("chi2_vf", m_chi2_vf_STDDmiss_sig_D);
            status = m_tuple2->addItem("chi2_kf", m_chi2_kf_STDDmiss_sig_D);
            status = m_tuple2->addItem("rm_Dpi", m_rm_Dpi_STDDmiss_sig_D);
            status = m_tuple2->addItem("n_othertrks", m_n_othertrks_STDDmiss_sig_D, 0, 20);
            status = m_tuple2->addIndexedItem("rawp4_otherMdctrk", m_n_othertrks_STDDmiss_sig_D, 6, m_rawp4_otherMdctrk_STDDmiss_sig_D);
            status = m_tuple2->addIndexedItem("rawp4_otherMdcKaltrk", m_n_othertrks_STDDmiss_sig_D, 6, m_rawp4_otherMdcKaltrk_STDDmiss_sig_D);
            status = m_tuple2->addItem("n_othershws", m_n_othershws_STDDmiss_sig_D, 0, 50);
            status = m_tuple2->addIndexedItem("rawp4_othershw", m_n_othershws_STDDmiss_sig_D, 4, m_rawp4_othershw_STDDmiss_sig_D);
            status = m_tuple2->addItem("indexmc", m_idxmc_STDDmiss_sig_D, 0, 100);
            status = m_tuple2->addIndexedItem("pdgid", m_idxmc_STDDmiss_sig_D, m_pdgid_STDDmiss_sig_D);
            status = m_tuple2->addIndexedItem("motheridx", m_idxmc_STDDmiss_sig_D, m_motheridx_STDDmiss_sig_D);
            status = m_tuple2->addItem("DpId", m_Id_Dp_STDDmiss_sig_D);
            status = m_tuple2->addItem("DmId", m_Id_Dm_STDDmiss_sig_D);
            status = m_tuple2->addItem("n_trk", m_n_trk_STDDmiss_sig_D, 0, 10); 
            status = m_tuple2->addIndexedItem("pid_trk", m_n_trk_STDDmiss_sig_D, 14, m_pid_trk_STDDmiss_sig_D);
            status = m_tuple2->addItem("p4_pi_MCTruth", 4, m_p4_pi_MCTruth_STDDmiss_sig_D);
            status = m_tuple2->addItem("p4_D_MCTruth", 4, m_p4_D_MCTruth_STDDmiss_sig_D);
            status = m_tuple2->addItem("p4_Dbar_MCTruth", 4, m_p4_Dbar_MCTruth_STDDmiss_sig_D);
        }
        else {
            log << MSG::ERROR << "Cannot book N-tuple:" << long(m_tuple2) << endmsg;
            return StatusCode::FAILURE;
        }
    }

    NTuplePtr nt3(ntupleSvc(), "FILE1/STDDmiss_sig_Dst");
    if (nt3) m_tuple3 = nt3;
    else {
        m_tuple3 = ntupleSvc()->book("FILE1/STDDmiss_sig_Dst", CLID_ColumnWiseTuple, "Single tag D decay and miss D*-bar track (signal sample)");
        if (m_tuple3) {
            status = m_tuple3->addItem("runNo", m_runNo_STDDmiss_sig_Dst);
            status = m_tuple3->addItem("evtNo", m_evtNo_STDDmiss_sig_Dst);
            status = m_tuple3->addItem("flag1", m_flag1_STDDmiss_sig_Dst);
            status = m_tuple3->addItem("n_trkD", m_n_trkD_STDDmiss_sig_Dst, 0, 5); // number of members should locates in 0~5
            status = m_tuple3->addIndexedItem("rawp4_Dtrk", m_n_trkD_STDDmiss_sig_Dst, 6, m_rawp4_Dtrk_STDDmiss_sig_Dst); // four members array
            status = m_tuple3->addIndexedItem("p4_Dtrk", m_n_trkD_STDDmiss_sig_Dst, 4, m_p4_Dtrk_STDDmiss_sig_Dst);
            status = m_tuple3->addItem("n_shwD", m_n_shwD_STDDmiss_sig_Dst, 0, 2); 
            status = m_tuple3->addIndexedItem("rawp4_Dshw", m_n_shwD_STDDmiss_sig_Dst, 4, m_rawp4_Dshw_STDDmiss_sig_Dst);
            status = m_tuple3->addIndexedItem("p4_Dshw", m_n_shwD_STDDmiss_sig_Dst, 4, m_p4_Dshw_STDDmiss_sig_Dst);
            status = m_tuple3->addItem("rawp4_tagpi", 4, m_rawp4_tagpi_STDDmiss_sig_Dst);
            status = m_tuple3->addItem("p4_pi", 4, m_p4_pi_STDDmiss_sig_Dst);
            status = m_tuple3->addItem("p4_Dmiss", 4, m_p4_Dmiss_STDDmiss_sig_Dst);
            status = m_tuple3->addItem("mode", m_mode_STDDmiss_sig_Dst);
            status = m_tuple3->addItem("charm", m_charm_STDDmiss_sig_Dst);
            status = m_tuple3->addItem("chi2_vf", m_chi2_vf_STDDmiss_sig_Dst);
            status = m_tuple3->addItem("chi2_kf", m_chi2_kf_STDDmiss_sig_Dst);
            status = m_tuple3->addItem("rm_Dpi", m_rm_Dpi_STDDmiss_sig_Dst);
            status = m_tuple3->addItem("n_othertrks", m_n_othertrks_STDDmiss_sig_Dst, 0, 20);
            status = m_tuple3->addIndexedItem("rawp4_otherMdctrk", m_n_othertrks_STDDmiss_sig_Dst, 6, m_rawp4_otherMdctrk_STDDmiss_sig_Dst);
            status = m_tuple3->addIndexedItem("rawp4_otherMdcKaltrk", m_n_othertrks_STDDmiss_sig_Dst, 6, m_rawp4_otherMdcKaltrk_STDDmiss_sig_Dst);
            status = m_tuple3->addItem("n_othershws", m_n_othershws_STDDmiss_sig_Dst, 0, 50);
            status = m_tuple3->addIndexedItem("rawp4_othershw", m_n_othershws_STDDmiss_sig_Dst, 4, m_rawp4_othershw_STDDmiss_sig_Dst);
            status = m_tuple3->addItem("indexmc", m_idxmc_STDDmiss_sig_Dst, 0, 100);
            status = m_tuple3->addIndexedItem("pdgid", m_idxmc_STDDmiss_sig_Dst, m_pdgid_STDDmiss_sig_Dst);
            status = m_tuple3->addIndexedItem("motheridx", m_idxmc_STDDmiss_sig_Dst, m_motheridx_STDDmiss_sig_Dst);
            status = m_tuple3->addItem("DpId", m_Id_Dp_STDDmiss_sig_Dst);
            status = m_tuple3->addItem("DmId", m_Id_Dm_STDDmiss_sig_Dst);
            status = m_tuple3->addItem("n_trk", m_n_trk_STDDmiss_sig_Dst, 0, 10); 
            status = m_tuple3->addIndexedItem("pid_trk", m_n_trk_STDDmiss_sig_Dst, 14, m_pid_trk_STDDmiss_sig_Dst);
        }
        else {
            log << MSG::ERROR << "Cannot book N-tuple:" << long(m_tuple3) << endmsg;
            return StatusCode::FAILURE;
        }
    }

    NTuplePtr nt4(ntupleSvc(), "FILE1/STDDmiss_bkg_D");
    if (nt4) m_tuple4 = nt4;
    else {
        m_tuple4 = ntupleSvc()->book("FILE1/STDDmiss_bkg_D", CLID_ColumnWiseTuple, "Single tag D decay and miss D-bar track (background sample)");
        if (m_tuple4) {
            status = m_tuple4->addItem("runNo", m_runNo_STDDmiss_bkg_D);
            status = m_tuple4->addItem("evtNo", m_evtNo_STDDmiss_bkg_D);
            status = m_tuple4->addItem("flag1", m_flag1_STDDmiss_bkg_D);
            status = m_tuple4->addItem("n_trkD", m_n_trkD_STDDmiss_bkg_D, 0, 5); // number of members should locates in 0~5
            status = m_tuple4->addIndexedItem("rawp4_Dtrk", m_n_trkD_STDDmiss_bkg_D, 6, m_rawp4_Dtrk_STDDmiss_bkg_D); // four members array
            status = m_tuple4->addIndexedItem("p4_Dtrk", m_n_trkD_STDDmiss_bkg_D, 4, m_p4_Dtrk_STDDmiss_bkg_D);
            status = m_tuple4->addItem("n_shwD", m_n_shwD_STDDmiss_bkg_D, 0, 2); 
            status = m_tuple4->addIndexedItem("rawp4_Dshw", m_n_shwD_STDDmiss_bkg_D, 4, m_rawp4_Dshw_STDDmiss_bkg_D);
            status = m_tuple4->addIndexedItem("p4_Dshw", m_n_shwD_STDDmiss_bkg_D, 4, m_p4_Dshw_STDDmiss_bkg_D);
            status = m_tuple4->addItem("rawp4_tagpi", 4, m_rawp4_tagpi_STDDmiss_bkg_D);
            status = m_tuple4->addItem("p4_pi", 4, m_p4_pi_STDDmiss_bkg_D);
            status = m_tuple4->addItem("p4_Dmiss", 4, m_p4_Dmiss_STDDmiss_bkg_D);
            status = m_tuple4->addItem("mode", m_mode_STDDmiss_bkg_D);
            status = m_tuple4->addItem("charm", m_charm_STDDmiss_bkg_D);
            status = m_tuple4->addItem("chi2_vf", m_chi2_vf_STDDmiss_bkg_D);
            status = m_tuple4->addItem("chi2_kf", m_chi2_kf_STDDmiss_bkg_D);
            status = m_tuple4->addItem("rm_Dpi", m_rm_Dpi_STDDmiss_bkg_D);
            status = m_tuple4->addItem("n_othertrks", m_n_othertrks_STDDmiss_bkg_D, 0, 20);
            status = m_tuple4->addIndexedItem("rawp4_otherMdctrk", m_n_othertrks_STDDmiss_bkg_D, 6, m_rawp4_otherMdctrk_STDDmiss_bkg_D);
            status = m_tuple4->addIndexedItem("rawp4_otherMdcKaltrk", m_n_othertrks_STDDmiss_bkg_D, 6, m_rawp4_otherMdcKaltrk_STDDmiss_bkg_D);
            status = m_tuple4->addItem("n_othershws", m_n_othershws_STDDmiss_bkg_D, 0, 50);
            status = m_tuple4->addIndexedItem("rawp4_othershw", m_n_othershws_STDDmiss_bkg_D, 4, m_rawp4_othershw_STDDmiss_bkg_D);
            status = m_tuple4->addItem("indexmc", m_idxmc_STDDmiss_bkg_D, 0, 100);
            status = m_tuple4->addIndexedItem("pdgid", m_idxmc_STDDmiss_bkg_D, m_pdgid_STDDmiss_bkg_D);
            status = m_tuple4->addIndexedItem("motheridx", m_idxmc_STDDmiss_bkg_D, m_motheridx_STDDmiss_bkg_D);
            status = m_tuple4->addItem("DpId", m_Id_Dp_STDDmiss_bkg_D);
            status = m_tuple4->addItem("DmId", m_Id_Dm_STDDmiss_bkg_D);
        }
        else {
            log << MSG::ERROR << "Cannot book N-tuple:" << long(m_tuple4) << endmsg;
            return StatusCode::FAILURE;
        }
    }

    NTuplePtr nt5(ntupleSvc(), "FILE1/STDDmiss_bkg_Dst");
    if (nt5) m_tuple5 = nt5;
    else {
        m_tuple5 = ntupleSvc()->book("FILE1/STDDmiss_bkg_Dst", CLID_ColumnWiseTuple, "Single tag D decay and miss D*-bar track (background sample)");
        if (m_tuple5) {
            status = m_tuple5->addItem("runNo", m_runNo_STDDmiss_bkg_Dst);
            status = m_tuple5->addItem("evtNo", m_evtNo_STDDmiss_bkg_Dst);
            status = m_tuple5->addItem("flag1", m_flag1_STDDmiss_bkg_Dst);
            status = m_tuple5->addItem("n_trkD", m_n_trkD_STDDmiss_bkg_Dst, 0, 5); // number of members should locates in 0~5
            status = m_tuple5->addIndexedItem("rawp4_Dtrk", m_n_trkD_STDDmiss_bkg_Dst, 6, m_rawp4_Dtrk_STDDmiss_bkg_Dst); // four members array
            status = m_tuple5->addIndexedItem("p4_Dtrk", m_n_trkD_STDDmiss_bkg_Dst, 4, m_p4_Dtrk_STDDmiss_bkg_Dst);
            status = m_tuple5->addItem("n_shwD", m_n_shwD_STDDmiss_bkg_Dst, 0, 2); 
            status = m_tuple5->addIndexedItem("rawp4_Dshw", m_n_shwD_STDDmiss_bkg_Dst, 4, m_rawp4_Dshw_STDDmiss_bkg_Dst);
            status = m_tuple5->addIndexedItem("p4_Dshw", m_n_shwD_STDDmiss_bkg_Dst, 4, m_p4_Dshw_STDDmiss_bkg_Dst);
            status = m_tuple5->addItem("rawp4_tagpi", 4, m_rawp4_tagpi_STDDmiss_bkg_Dst);
            status = m_tuple5->addItem("p4_pi", 4, m_p4_pi_STDDmiss_bkg_Dst);
            status = m_tuple5->addItem("p4_Dmiss", 4, m_p4_Dmiss_STDDmiss_bkg_Dst);
            status = m_tuple5->addItem("mode", m_mode_STDDmiss_bkg_Dst);
            status = m_tuple5->addItem("charm", m_charm_STDDmiss_bkg_Dst);
            status = m_tuple5->addItem("chi2_vf", m_chi2_vf_STDDmiss_bkg_Dst);
            status = m_tuple5->addItem("chi2_kf", m_chi2_kf_STDDmiss_bkg_Dst);
            status = m_tuple5->addItem("rm_Dpi", m_rm_Dpi_STDDmiss_bkg_Dst);
            status = m_tuple5->addItem("n_othertrks", m_n_othertrks_STDDmiss_bkg_Dst, 0, 20);
            status = m_tuple5->addIndexedItem("rawp4_otherMdctrk", m_n_othertrks_STDDmiss_bkg_Dst, 6, m_rawp4_otherMdctrk_STDDmiss_bkg_Dst);
            status = m_tuple5->addIndexedItem("rawp4_otherMdcKaltrk", m_n_othertrks_STDDmiss_bkg_Dst, 6, m_rawp4_otherMdcKaltrk_STDDmiss_bkg_Dst);
            status = m_tuple5->addItem("n_othershws", m_n_othershws_STDDmiss_bkg_Dst, 0, 50);
            status = m_tuple5->addIndexedItem("rawp4_othershw", m_n_othershws_STDDmiss_bkg_Dst, 4, m_rawp4_othershw_STDDmiss_bkg_Dst);
            status = m_tuple5->addItem("indexmc", m_idxmc_STDDmiss_bkg_Dst, 0, 100);
            status = m_tuple5->addIndexedItem("pdgid", m_idxmc_STDDmiss_bkg_Dst, m_pdgid_STDDmiss_bkg_Dst);
            status = m_tuple5->addIndexedItem("motheridx", m_idxmc_STDDmiss_bkg_Dst, m_motheridx_STDDmiss_bkg_Dst);
            status = m_tuple5->addItem("DpId", m_Id_Dp_STDDmiss_bkg_Dst);
            status = m_tuple5->addItem("DmId", m_Id_Dm_STDDmiss_bkg_Dst);
        }
        else {
            log << MSG::ERROR << "Cannot book N-tuple:" << long(m_tuple5) << endmsg;
            return StatusCode::FAILURE;
        }
    }

    log << MSG::INFO << "successfully return from initialize()" << endmsg;
    return StatusCode::SUCCESS;
}

StatusCode DDecay::execute() {
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << "in execute()" << endreq;

    // clear variables
    clearVariables();

    // grt common info
    SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
    if (!eventHeader) return StatusCode::FAILURE;
    runNo = eventHeader->runNumber();
    evtNo = eventHeader->eventNumber();

    if (m_debug) {
        std::cout << "**************************************" << std::endl;
        std::cout << "run = "  <<  runNo  <<  ", event = " << evtNo << std::endl;
        std::cout << "**************************************" << std::endl;
    }

    flag1 = eventHeader->flag1();
    int decay_1 = ((eventHeader->flag1()/1000000))%1000;
    int decay_2 = (eventHeader->flag1()/1000)%1000;
    int decay_3 = eventHeader->flag1()%1000;
    if (m_debug) std::cout << " flg " << decay_1 << "     " << decay_2 << "     " << decay_3 << std::endl;

    // save McTruth info
    if (runNo < 0 && m_isMonteCarlo) saveAllMcTruthInfo();
    if (runNo < 0 && m_isMonteCarlo) saveDpDmMcTruthInfo();

    // use DTagTool
    stat_DTagTool = useDTagTool();

    return StatusCode::SUCCESS;
}

StatusCode DDecay::finalize() {
    std::cout << "Ncut0: " << Ncut0 << std::endl;
    std::cout << "Ncut1: " << Ncut1 << std::endl;
    std::cout << "Ncut2: " << Ncut2 << std::endl;
    std::cout << "Ncut3: " << Ncut3 << std::endl;
    std::cout << "Ncut4: " << Ncut4 << std::endl;
    std::cout << "Ncut5: " << Ncut5 << std::endl;
    std::cout << "Ncut6: " << Ncut6 << std::endl;
    MsgStream log(msgSvc(), name());
    log << MSG::INFO << ">>>>>>> in finalize()" << endmsg;

    return StatusCode::SUCCESS;
}

void DDecay::clearVariables() {
    // common info
    runNo = 0;
    evtNo = 0;
    flag1 = 0;
    m_runNo = 0;
    m_evtNo = 0;
    m_flag1 = 0;
    m_runNo_STDDmiss_sig_D = 0;
    m_evtNo_STDDmiss_sig_D = 0;
    m_flag1_STDDmiss_sig_D = 0;
    m_runNo_STDDmiss_sig_Dst = 0;
    m_evtNo_STDDmiss_sig_Dst = 0;
    m_flag1_STDDmiss_sig_Dst = 0;
    m_runNo_STDDmiss_bkg_D = 0;
    m_evtNo_STDDmiss_bkg_D = 0;
    m_flag1_STDDmiss_bkg_D = 0;
    m_runNo_STDDmiss_bkg_Dst = 0;
    m_evtNo_STDDmiss_bkg_Dst = 0;
    m_flag1_STDDmiss_bkg_Dst = 0;

    // all McTruth info
    m_idxmc = 0;
    m_idxmc_STDDmiss_sig_D = 0;
    m_idxmc_STDDmiss_sig_Dst = 0;
    m_idxmc_STDDmiss_bkg_D = 0;
    m_idxmc_STDDmiss_bkg_Dst = 0;

    // DpDm McTruth info
    DpId = -999;
    DmId = -999;

    // single D tag
    mDcand = 0;
    mDcand_miss = 0;
    n_count = 0;
    m_n_count = 0;
    MODE = -999;
    n_trkD = 0;
    n_shwD = 0;
    mode = -999;
    charm = -999;
    chi2_vf = 999;
    chi2_kf = 999;
    m_n_trkD = 0;
    m_n_shwD = 0;
    m_mode = -999;
    m_charm = -999;
    m_chi2_vf = 999;
    m_chi2_kf = 999;
    m_n_trkD_STDDmiss_sig_D = 0;
    m_n_shwD_STDDmiss_sig_D = 0;
    m_mode_STDDmiss_sig_D = -999;
    m_charm_STDDmiss_sig_D = -999;
    m_chi2_vf_STDDmiss_sig_D = 999;
    m_chi2_kf_STDDmiss_sig_D = 999;
    m_n_trkD_STDDmiss_sig_Dst = 0;
    m_n_shwD_STDDmiss_sig_Dst = 0;
    m_mode_STDDmiss_sig_Dst = -999;
    m_charm_STDDmiss_sig_Dst = -999;
    m_chi2_vf_STDDmiss_sig_Dst = 999;
    m_chi2_kf_STDDmiss_sig_Dst = 999;
    m_n_trkD_STDDmiss_bkg_D = 0;
    m_n_shwD_STDDmiss_bkg_D = 0;
    m_mode_STDDmiss_bkg_D = -999;
    m_charm_STDDmiss_bkg_D = -999;
    m_chi2_vf_STDDmiss_bkg_D = 999;
    m_chi2_kf_STDDmiss_bkg_D = 999;
    m_n_trkD_STDDmiss_bkg_Dst = 0;
    m_n_shwD_STDDmiss_bkg_Dst = 0;
    m_mode_STDDmiss_bkg_Dst = -999;
    m_charm_STDDmiss_bkg_Dst = -999;
    m_chi2_vf_STDDmiss_bkg_Dst = 999;
    m_chi2_kf_STDDmiss_bkg_Dst = 999;

    // judgement variables
    stat_DTagTool = false;
    stat_tagSD = false;
    stat_saveCandD = false;
    stat_saveOthertrks = false;
    stat_saveOthershws = false;
}

void DDecay::saveAllMcTruthInfo() {
    SmartDataPtr<Event::McParticleCol> mcParticleCol(eventSvc(), "/Event/MC/McParticleCol");
    if (!mcParticleCol) {
        std::cout << "Could not retreive McParticleCol" << std::endl;
    }
    else {
        Event::McParticleCol::iterator iter_mc = mcParticleCol->begin(); // loop all the particles in the decay chain(MCTruth)
        int pid = (*iter_mc)->particleProperty();
        unsigned int idx;
        unsigned int midx;
        idxmc = 0;
        if (pid == 90022 || pid == 80022) {
             for (iter_mc++; iter_mc != mcParticleCol->end(); iter_mc++) {
                 if (!(*iter_mc)->decayFromGenerator()) continue;
                 pid = (*iter_mc)->particleProperty();
                 idx = (*iter_mc)->trackIndex();
                 midx = ((*iter_mc)->mother()).trackIndex();
                 pdgid[idxmc] = pid;
                 if (idx == midx || midx == 0) motheridx[idxmc] = idx - 1;
                 else motheridx[idxmc] = midx - 1;
                 idxmc++;
             }
        }
        else {
            for (; iter_mc != mcParticleCol->end(); iter_mc++) {
                if (!(*iter_mc)->decayFromGenerator()) continue;
                pdgid[idxmc] = (*iter_mc)->particleProperty();
                motheridx[idxmc]= ((*iter_mc)->mother()).trackIndex();
                idxmc++;
            }
        }
        if (m_debug) std::cout << " PDG.SIZE():  " << idxmc << std::endl;
        iter_mc = mcParticleCol->begin(); // loop all the particles in the decay chain(MCTruth)
        for (; iter_mc != mcParticleCol->end(); iter_mc++) {
            pAll.push_back((*iter_mc)->initialFourMomentum()); // initialFourMomentum: Four Momentum in the iteraction point
            pdg.push_back((*iter_mc)->particleProperty());
            mother.push_back((*iter_mc)->mother().particleProperty());
        }
        for (int i = 0; i < pdg.size(); i++) {
            if (fabs(mother[i]) == 9030443 && fabs(pdg[i]) == 211) {
                p4_pi_MCTruth[0] = pAll[i].px();
                p4_pi_MCTruth[1] = pAll[i].py();
                p4_pi_MCTruth[2] = pAll[i].pz();
                p4_pi_MCTruth[3] = pAll[i].e();
                for (int j = 0; j < pdg.size(); j++) {
                    if (fabs(mother[j]) == 9030443 && fabs(pdg[j]) == 411) {
                        p4_D_MCTruth[0] = pAll[j].px();
                        p4_D_MCTruth[1] = pAll[j].py();
                        p4_D_MCTruth[2] = pAll[j].pz();
                        p4_D_MCTruth[3] = pAll[j].e();
                    }
                    if (fabs(mother[j]) == 9030443 && fabs(pdg[j]) == 421) {
                        p4_Dbar_MCTruth[0] = pAll[j].px();
                        p4_Dbar_MCTruth[1] = pAll[j].py();
                        p4_Dbar_MCTruth[2] = pAll[j].pz();
                        p4_Dbar_MCTruth[3] = pAll[j].e();
                    }
                }
            }
        }
    }
}

void DDecay::saveDpDmMcTruthInfo() {
    SmartDataPtr<Event::McParticleCol> mcParticleCol(eventSvc(), "/Event/MC/McParticleCol");
    Event::McParticleCol::iterator iter_DpDmmc = mcParticleCol->begin();
    for (; iter_DpDmmc != mcParticleCol->end(); iter_DpDmmc++) {
        if ((*iter_DpDmmc)->particleProperty() == 421) { // D0
            const SmartRefVector<Event::McParticle>& gc = (*iter_DpDmmc)->daughterList();
            int num_k=0, num_pip=0, num_pi0=0, num_others=0;
            if (gc.size() > 0) {
                if (m_debug) std::cout << " D0  daughter particles" << gc[0]->particleProperty() << "   " << gc[1]->particleProperty() << std::endl;
                for (unsigned int j = 0; j < gc.size(); j++) {
                    if (gc[j]->particleProperty() == -22) continue;
                    else if (fabs(gc[j]->particleProperty()) == 321) num_k++;
                    else if (fabs(gc[j]->particleProperty()) == 211) num_pip++;
                    else if (gc[j]->particleProperty() == 111) num_pi0++;
                    else num_others++;
                }
            }
            if (num_k == 1 && num_pip == 1 && num_pi0 == 1 && num_others==0) { // D->K-pi+pi0
                DpId = 1;
            }
            else if (num_k == 1 && num_pip == 1 && num_pi0 == 0 && num_others == 0) { // D->Kpi
                DpId = 0;
            }
            else DpId = 3;
        }
        if ((*iter_DpDmmc)->particleProperty() == 411) { //D+
            const SmartRefVector<Event::McParticle>& gc = (*iter_DpDmmc)->daughterList();
            int num_k = 0, num_pip = 0, num_others = 0;
            if (gc.size() > 0) {
                if (m_debug) std::cout << " Dp  daughter particles" << gc[0]->particleProperty() << "   " << gc[1]->particleProperty() << std::endl;
                for(unsigned int j = 0; j < gc.size(); j++) {
                    if (gc[j]->particleProperty() == -22) continue; // D+->Kpipi
                    else if (gc[j]->particleProperty() == -321) num_k++;
                    else if (gc[j]->particleProperty() == 211) num_pip++;
                    else num_others++;
                }
            }
            if (num_k == 1 && num_pip == 2 && num_others == 0) {
                DpId = 200;
            }
        }
        if (m_debug) std::cout << runNo << " : " << evtNo << " : decay mode " << DpId << "   " << DmId << std::endl;
    }
}

bool DDecay::useDTagTool() {
    DTagTool dtagTool; // before running this program, DTagTool have running, and the suited D candidates have been tagged
    if (m_pid) dtagTool.setPID(true); // all the combinations of tracks have been tested, the suited D candidantes among them have been tagged "true", others have been tagged "false"
    if (dtagTool.isDTagListEmpty()) {
        if (m_debug) std::cout << "no D candidates found" << std::endl;
        return false;
    }
    else {
        // to retrieve RecEvent
        SmartDataPtr<EvtRecEvent> evtRecEvent(eventSvc(), "/Event/EvtRec/EvtRecEvent");
        if (!evtRecEvent) {
            std::cout << MSG::FATAL << "Could not find EvtRecEvent" << std::endl;
            return false;
        }
        // to retrieve RecTrackCol
        SmartDataPtr<EvtRecTrackCol> evtRecTrackCol(eventSvc(), "/Event/EvtRec/EvtRecTrackCol");
        if (!evtRecTrackCol) {
            std::cout << MSG::FATAL << "Could not find EvtRecTrackCol" << std::endl;
            return false;
        }
    }
    
    stat_tagSD = tagSingleD();
    if (stat_tagSD) return true;
    else return false;
}

bool DDecay::tagSingleD() {
    VWTrkPara vwtrkpara_charge, vwtrkpara_photon;
    for (int i = 0; i < m_DModes.size(); i++) {
        mode = m_DModes[i];
        if (mode != 200 && mode != 0 && mode != 1 && mode != 3) continue; 
        // mode = 200 includes: D+->K-pi+pi+ or D-->K+pi-pi-
        // mode = 201 includes: D+->K-pi+pi+pi0
        // mode = 202 includes: D+->Kspi+
        // mode = 203 includes: D+->Kspi+pi0
        // mode = 204 includes: D+->Kspi+pi-pi+
        // mode = 205 includes: D+->K-K+pi+
        // mode = 208 includes: D+->KsK+
        // mode = 213 includes: D+->KsKsK+
        // mode = 216 includes: D+->KsK+pi+pi-
        // mode = 0 includes: D0->K-pi+ or Dbar->K+pi-
        // mode = 1 includes: D0->K-pi+pi0 or Dbar->K+pi-pi0
        // mode = 3 includes: D0->K-pi+pi+pi- or Dbar->K+pi-pi-pi+

        // define D candidate mass
        if ((int)mode < 200) mDcand = M_D0;
        else if ((int)mode < 400) mDcand = M_Dplus;
        else {
            std::cout << "single tag mode is not found! " << std::endl;
            return false;
        }
        stat_saveCandD = saveCandD(vwtrkpara_charge, vwtrkpara_photon);
    }
    if (stat_saveCandD) return true;
    else return false;
}

bool DDecay::saveCandD(VWTrkPara &vwtrkpara_charge, VWTrkPara &vwtrkpara_photon) {
    // Accessing DTagList
    SmartDataPtr<EvtRecDTagCol> evtRecDTagCol(eventSvc(), EventModel::EvtRec::EvtRecDTagCol);
    if (!evtRecDTagCol) {
        std::cout << MSG::FATAL <<"Could not find EvtRecDTagCol" << std::endl;
        return false;
    }

    // keep all the candidate of D
    dtag_iter_begin = evtRecDTagCol->begin();
    dtag_iter_end = evtRecDTagCol->end();
    dtag_iter = dtag_iter_begin;

    // loop over the dtag list
    for (; dtag_iter != dtag_iter_end; dtag_iter++) {
        // whether to use pid
        Ncut0++;
        if (m_pid) {
            if ((*dtag_iter)->type() !=1 || (*dtag_iter)->decayMode() != mode) {
                continue; // type = 1: PID has been performed, type = 0, PID hasn't been performed
            }
        }
        else {
            if ((*dtag_iter)->decayMode() != mode) {
                continue;
            }
        }
        Ncut1++;

        if (m_debug) std::cout << " --> dtag found : " << mode << std::endl;
        if (m_debug) std::cout << " D charm number: " << (*dtag_iter)->charm() << std::endl; // (*dtag_iter)->charm() = 1: c, -1: cbar

        // very broad mass window requirement
        if (fabs((*dtag_iter)->mass() - mDcand) > 0.07) {
            continue;
        }
        Ncut2++;

        SmartRefVector<EvtRecTrack> Dtrks = (*dtag_iter)->tracks();
        SmartRefVector<EvtRecTrack> Dshws = (*dtag_iter)->showers();
        n_trkD = Dtrks.size();
        n_shwD = Dshws.size();
        MODE = (*dtag_iter)->decayMode();
        charm = (*dtag_iter)->charm(); // (*dtag_iter)->charm() = 1: c, -1: cbar

        if (m_debug) std::cout<<" D: ntrk  nshw " << n_trkD << "  " << n_shwD << std::endl;

        vwtrkpara_charge.clear();
        HepLorentzVector pK;
        HepLorentzVector ppi;
        DTagTool dtagTool;
        for (int j = 0; j < n_trkD; j++) {
            RecMdcKalTrack* KalTrk = Dtrks[j]->mdcKalTrack();
            // to fill Kaon candidates
            // if (j == 0) { // default arrangement: (K,pi), number of K depend on the mode you choose
            if (dtagTool.isKaon(Dtrks[j])) {
                KalTrk->setPidType(RecMdcKalTrack::kaon);
                if (m_debug) std::cout << " filling kaon track " << std::endl;
                vwtrkpara_charge.push_back(WTrackParameter(mass[3], KalTrk->getZHelixK(), KalTrk->getZErrorK()));
                for (int k = 0; k < 4; k++) m_rawp4_Dtrk[j][k] = KalTrk->p4(mass[3])[k]; // MDC gives three momentum, combined with mass, we can get energy which means four momentum
                m_rawp4_Dtrk[j][4] = KalTrk->charge();
                m_rawp4_Dtrk[j][5] = 3;
                pK.setPx(KalTrk->p4(mass[3])[0]);
                pK.setPy(KalTrk->p4(mass[3])[1]);
                pK.setPz(KalTrk->p4(mass[3])[2]);
                pK.setE(KalTrk->p4(mass[3])[3]);
            }
            // to fill Pion candidates
            else {
                KalTrk->setPidType(RecMdcKalTrack::pion);
                if (m_debug) std::cout << " filling pion track " << std::endl;
                vwtrkpara_charge.push_back(WTrackParameter(mass[2], KalTrk->getZHelix(), KalTrk->getZError()));
                for (int k = 0; k < 4; k++) m_rawp4_Dtrk[j][k] = KalTrk->p4(mass[2])[k];
                m_rawp4_Dtrk[j][4] = KalTrk->charge();
                m_rawp4_Dtrk[j][5] = 2;
                ppi.setPx(KalTrk->p4(mass[2])[0]);
                ppi.setPy(KalTrk->p4(mass[2])[1]);
                ppi.setPz(KalTrk->p4(mass[2])[2]);
                ppi.setE(KalTrk->p4(mass[2])[3]);
            }
        }

        // to check the vector in each dtag item
        if (m_debug) {
            double index_vector = 0;
            std::cout << "total cahrged: " << vwtrkpara_charge.size() << std::endl;
            while(index_vector < vwtrkpara_charge.size()) {
                std::cout << " Add charged tracks: " << index_vector 
                << " with charge: " << vwtrkpara_charge[index_vector].charge()
                << " with momentum: " << vwtrkpara_charge[index_vector].p() 
                << " mass: " << vwtrkpara_charge[index_vector].mass() << std::endl;
                index_vector++;
            }
        }

        if (vwtrkpara_charge.size() != n_trkD) {
            continue;
        }
        Ncut3++;

        // do vertex fit
        chi2_vf = fitVertex(vwtrkpara_charge, birth);
        if (chi2_vf > 100) {
            continue;
        }
        Ncut4++;

        if (m_debug) std::cout << " vertex fitting chisq: " << chi2_vf << std::endl;
        if (m_debug) std::cout << " vertex fitting vertex: " << birth.vx() << std::endl;

        HepLorentzVector pgam;
        vwtrkpara_photon.clear(); // for mode ee->DDpipi, there is no pi0, so the vector will be empty
        for(int j = 0; j < Dshws.size(); j++) {
            RecEmcShower *gTrk = Dshws[j]->emcShower();
            Hep3Vector Gm_Vec(gTrk->x(), gTrk->y(), gTrk->z());
            Hep3Vector Gm_Mom = Gm_Vec - birth.vx(); // vx: Vertex, we regard that the track of the gamma before it enters EMC is a line, so to get the info of this line, we can just subtract the vertex info from the EMC hit point
            Gm_Mom.setMag(gTrk->energy());
            HepLorentzVector Gm_p4(Gm_Mom, gTrk->energy());
            vwtrkpara_photon.push_back(WTrackParameter(gTrk->position(), Gm_p4, gTrk->dphi(), gTrk->dtheta(), gTrk->dE())); // dE, error of the gamma energy
            for (int k = 0; k < 4; k++) m_rawp4_Dshw[j][k] = Gm_p4[k];
            pgam.setPx(Gm_p4[0]);
            pgam.setPy(Gm_p4[1]);
            pgam.setPz(Gm_p4[2]);
            pgam.setE(Gm_p4[3]);
        }
        
        if (m_debug) {
            double index_vector=0;
            std::cout << "total neutral: " << vwtrkpara_photon.size() << std::endl;
            while(index_vector < vwtrkpara_photon.size()) {
                std::cout << " Add neutral tracks: " << index_vector 
                << " with momentum: " << vwtrkpara_photon[index_vector].p() << std::endl;
                index_vector++;
            }
        }

        // KM fit on D candidate
        chi2_kf = fitKM(vwtrkpara_charge, vwtrkpara_photon, birth);

        // to store the other showers information
        stat_saveOthershws = saveOthershws();

        // to store the other track information
        stat_saveOthertrks = saveOthertrks(vwtrkpara_charge, vwtrkpara_photon, birth);

        // record variables
        if (stat_saveOthertrks && stat_saveOthershws) {
            n_count++;
            recordVariables();
        }
    }
    if (stat_saveOthertrks && stat_saveOthershws) return true;
    else return false;
}

bool DDecay::saveOthertrks(VWTrkPara &vwtrkpara_charge, VWTrkPara &vwtrkpara_photon, VertexParameter &birth) {
    SmartRefVector<EvtRecTrack> othertracks = (*dtag_iter)->otherTracks();
    SmartDataPtr<EvtRecEvent> evtRecEvent(eventSvc(), "/Event/EvtRec/EvtRecEvent");
    if (m_debug) std::cout << " total charged tracks : " << evtRecEvent->totalCharged() << std::endl;
    if (m_debug) std::cout << " other track number : " << othertracks.size() << " for mode " << mode << std::endl;
    DTagTool dtagTool;
    m_n_othertrks = 0;
    HepLorentzVector ppi_cand;
    // to find the good pions and kaons
    for (int i = 0; i < othertracks.size(); i++) {
        if (!(dtagTool.isGoodTrack(othertracks[i]))) continue;
        Ncut5++;
        if (dtagTool.isPion(othertracks[i])) {
            RecMdcTrack *mdcTrk = othertracks[i]->mdcTrack();
            RecMdcKalTrack *mdcKalTrk = othertracks[i]->mdcKalTrack();
            mdcKalTrk->setPidType(RecMdcKalTrack::pion);
            ppi_cand.setPx(mdcKalTrk->p4(mass[2])[0]);
            ppi_cand.setPy(mdcKalTrk->p4(mass[2])[1]);
            ppi_cand.setPz(mdcKalTrk->p4(mass[2])[2]);
            ppi_cand.setE(mdcKalTrk->p4(mass[2])[3]);
            for (int j = 0; j < 4; j++) {
                m_rawp4_otherMdctrk[m_n_othertrks][j] = mdcTrk->p4(mass[2])[j];
                m_rawp4_otherMdcKaltrk[m_n_othertrks][j] = mdcKalTrk->p4(mass[2])[j];
            }
            m_rawp4_otherMdctrk[m_n_othertrks][4] = mdcTrk->chi2();
            m_rawp4_otherMdctrk[m_n_othertrks][5] = mdcTrk->stat(); // stat: status
            m_rawp4_otherMdcKaltrk[m_n_othertrks][4] = mdcKalTrk->charge();
            m_rawp4_otherMdcKaltrk[m_n_othertrks][5] = 2;
        }
        if (dtagTool.isKaon(othertracks[i])) {
            RecMdcTrack *mdcTrk = othertracks[i]->mdcTrack();
            RecMdcKalTrack *mdcKalTrk = othertracks[i]->mdcKalTrack();
            mdcKalTrk->setPidType(RecMdcKalTrack::kaon);
            for (int j = 0; j < 4; j++) {
                m_rawp4_otherMdctrk[m_n_othertrks][j] = mdcTrk->p4(mass[3])[j];
                m_rawp4_otherMdcKaltrk[m_n_othertrks][j] = mdcKalTrk->p4(mass[3])[j];
            }
            m_rawp4_otherMdctrk[m_n_othertrks][4] = mdcTrk->chi2();
            m_rawp4_otherMdctrk[m_n_othertrks][5] = mdcTrk->stat(); // stat: status
            m_rawp4_otherMdcKaltrk[m_n_othertrks][4] = mdcKalTrk->charge();
            m_rawp4_otherMdcKaltrk[m_n_othertrks][5] = 3;
        }
        m_n_othertrks++;
        if (m_n_othertrks >= 20) return false;
    }

    if (m_debug) std::cout << " recorded " << m_n_othertrks << " other charged good tracks " << std::endl;

    ParticleID *pid = ParticleID::instance();
    pid->init();
    pid->setMethod(pid->methodProbability());
    pid->setChiMinCut(4);

    // signal sample : e+e- --> DpiD-bar
    VWTrkPara vwtrkpara_pi;
    vwtrkpara_pi.clear();
    m_n_trk_STDDmiss_sig_D = 0;
    int n_pi = 0;
    for (int i = 0; i < othertracks.size(); i++) {
        if (!(dtagTool.isGoodTrack(othertracks[i]))) continue;
        if (!judge_continue(mode, charm, m_rawp4_otherMdcKaltrk[i][4], m_rawp4_otherMdcKaltrk[i][5], "sig", "D")) continue;
        RecMdcKalTrack *mdcKalTrk = othertracks[i]->mdcKalTrack();
        m_pid_trk_STDDmiss_sig_D[m_n_trk_STDDmiss_sig_D][0] = mdcKalTrk->p();
        m_pid_trk_STDDmiss_sig_D[m_n_trk_STDDmiss_sig_D][1] = mdcKalTrk->charge();
        m_pid_trk_STDDmiss_sig_D[m_n_trk_STDDmiss_sig_D][2] = mdcKalTrk->pz() / mdcKalTrk->p();
        m_pid_trk_STDDmiss_sig_D[m_n_trk_STDDmiss_sig_D][3] = mdcKalTrk->phi();
        pid->setRecTrack(othertracks[i]);
        pid->usePidSys(pid->useDedx()); // use PID sub-system
        pid->identify(pid->all() | pid->onlyElectron() | pid->onlyMuon() | pid->onlyPion() | pid->onlyKaon() | pid->onlyProton());  
        pid->calculate();
        for (int j = 0; j < 5; j++) {
            if((othertracks[i])->isMdcDedxValid()) {
                RecMdcDedx* dedxtrk = (othertracks[i])->mdcDedx();
                m_pid_trk_STDDmiss_sig_D[m_n_trk_STDDmiss_sig_D][4 + j] = dedxtrk->chi(j);
                if(!(pid->IsPidInfoValid())) continue;
                m_pid_trk_STDDmiss_sig_D[m_n_trk_STDDmiss_sig_D][9 + j] = pid->prob(j);
            }
        }
        m_n_trk_STDDmiss_sig_D++;
        vwtrkpara_pi.push_back(WTrackParameter(mass[2], mdcKalTrk->getZHelix(), mdcKalTrk->getZError()));
        n_pi++;
        HepLorentzVector pD;
        pD.setPx(0.);
        pD.setPy(0.);
        pD.setPz(0.);
        pD.setE(0.);
        for (int j = 0; j < n_trkD; j++) {
            HepLorentzVector ptrack;
            ptrack.setPx(m_p4_Dtrk[j][0]);
            ptrack.setPy(m_p4_Dtrk[j][1]);
            ptrack.setPz(m_p4_Dtrk[j][2]);
            ptrack.setE(m_p4_Dtrk[j][3]);
            pD += ptrack;
        }
        for(int j = 0; j < n_shwD; j++) {
            HepLorentzVector pshower;
            pshower.setPx(m_p4_Dshw[j][0]);
            pshower.setPy(m_p4_Dshw[j][1]);
            pshower.setPz(m_p4_Dshw[j][2]);
            pshower.setE(m_p4_Dshw[j][3]);
            pD += pshower;
        }
        HepLorentzVector pPi;
        pPi.setPx(mdcKalTrk->p4(mass[2])[0]);
        pPi.setPy(mdcKalTrk->p4(mass[2])[1]);
        pPi.setPz(mdcKalTrk->p4(mass[2])[2]);
        pPi.setE(mdcKalTrk->p4(mass[2])[3]);
        HepLorentzVector ecms(0.011*m_Ecms, 0, 0, m_Ecms);
        double rm_Dpi = (ecms - pD - pPi).m();
        chi2_kf_STDDmiss_sig_D = 999;
        m_chi2_kf_STDDmiss_sig_D = 999;
        if (m_debug) std::cout << "recoiling mass of Dpi: " << rm_Dpi << ", ecms: " << ecms << std::endl;
        if (rm_Dpi > 1.6 && rm_Dpi < 2.0) {
            m_rm_Dpi_STDDmiss_sig_D = rm_Dpi;
            chi2_kf_STDDmiss_sig_D = fitKM_STDDmiss_sig_D(vwtrkpara_charge, vwtrkpara_photon, vwtrkpara_pi, n_pi - 1, birth);
        }
        if (m_debug) std::cout << "Start recording region info if passed the requirement" << std::endl;
        if (fabs(chi2_kf_STDDmiss_sig_D) < 9999.) {
            SmartRefVector<EvtRecTrack> Dtrks = (*dtag_iter)->tracks();
            for (int j = 0; j < n_trkD; j++) {
                RecMdcKalTrack* KalTrk = Dtrks[j]->mdcKalTrack();
                if (dtagTool.isKaon(Dtrks[j])) {
                    KalTrk->setPidType(RecMdcKalTrack::kaon);
                    for (int k = 0; k < 4; k++) m_rawp4_Dtrk_STDDmiss_sig_D[j][k] = KalTrk->p4(mass[3])[k];
                    m_rawp4_Dtrk_STDDmiss_sig_D[j][4] = KalTrk->charge();
                    m_rawp4_Dtrk_STDDmiss_sig_D[j][5] = 3;
                    m_pid_trk_STDDmiss_sig_D[m_n_trk_STDDmiss_sig_D][0] = KalTrk->p();
                    m_pid_trk_STDDmiss_sig_D[m_n_trk_STDDmiss_sig_D][1] = KalTrk->charge();
                    m_pid_trk_STDDmiss_sig_D[m_n_trk_STDDmiss_sig_D][2] = KalTrk->pz() / KalTrk->p();
                    m_pid_trk_STDDmiss_sig_D[m_n_trk_STDDmiss_sig_D][3] = KalTrk->phi();
                    pid->setRecTrack(Dtrks[j]);
                    pid->usePidSys(pid->useDedx()); // use PID sub-system
                    pid->identify(pid->all() | pid->onlyElectron() | pid->onlyMuon() | pid->onlyPion() | pid->onlyKaon() | pid->onlyProton());  
                    pid->calculate();
                    for (int k = 0; k < 5; k++) {
                        if((Dtrks[j])->isMdcDedxValid()) {
                            RecMdcDedx* dedxtrk = (Dtrks[j])->mdcDedx();
                            m_pid_trk_STDDmiss_sig_D[m_n_trk_STDDmiss_sig_D][4 + k] = dedxtrk->chi(k);
                            if(!(pid->IsPidInfoValid())) continue;
                            m_pid_trk_STDDmiss_sig_D[m_n_trk_STDDmiss_sig_D][9 + k] = pid->prob(k);
                        }
                    }
                    m_n_trk_STDDmiss_sig_D++;
                } else {
                    KalTrk->setPidType(RecMdcKalTrack::pion);
                    for (int k = 0; k < 4; k++) m_rawp4_Dtrk_STDDmiss_sig_D[j][k] = KalTrk->p4(mass[2])[k];
                    m_rawp4_Dtrk_STDDmiss_sig_D[j][4] = KalTrk->charge();
                    m_rawp4_Dtrk_STDDmiss_sig_D[j][5] = 2;
                    m_pid_trk_STDDmiss_sig_D[m_n_trk_STDDmiss_sig_D][0] = KalTrk->p();
                    m_pid_trk_STDDmiss_sig_D[m_n_trk_STDDmiss_sig_D][1] = KalTrk->charge();
                    m_pid_trk_STDDmiss_sig_D[m_n_trk_STDDmiss_sig_D][2] = KalTrk->pz() / KalTrk->p();
                    m_pid_trk_STDDmiss_sig_D[m_n_trk_STDDmiss_sig_D][3] = KalTrk->phi();
                    pid->setRecTrack(Dtrks[j]);
                    pid->usePidSys(pid->useDedx()); // use PID sub-system
                    pid->identify(pid->all() | pid->onlyElectron() | pid->onlyMuon() | pid->onlyPion() | pid->onlyKaon() | pid->onlyProton());  
                    pid->calculate();
                    for (int k = 0; k < 5; k++) {
                        if((Dtrks[j])->isMdcDedxValid()) {
                            RecMdcDedx* dedxtrk = (Dtrks[j])->mdcDedx();
                            m_pid_trk_STDDmiss_sig_D[m_n_trk_STDDmiss_sig_D][4 + k] = dedxtrk->chi(k);
                            if(!(pid->IsPidInfoValid())) continue;
                            m_pid_trk_STDDmiss_sig_D[m_n_trk_STDDmiss_sig_D][9 + k] = pid->prob(k);
                        }
                    }
                    m_n_trk_STDDmiss_sig_D++;
                }
            }
            SmartRefVector<EvtRecTrack> Dshws = (*dtag_iter)->showers();
            for(int j = 0; j < Dshws.size(); j++) {
                RecEmcShower *gTrk = Dshws[j]->emcShower();
                Hep3Vector Gm_Vec(gTrk->x(), gTrk->y(), gTrk->z());
                Hep3Vector Gm_Mom = Gm_Vec - birth.vx();
                Gm_Mom.setMag(gTrk->energy());
                HepLorentzVector Gm_p4(Gm_Mom, gTrk->energy());
                for (int k = 0; k < 4; k++) m_rawp4_Dshw_STDDmiss_sig_D[j][k] = Gm_p4[k];
            }
            // to find the good pions and kaons
            m_n_othertrks_STDDmiss_sig_D = 0;
            for (int j = 0; j < othertracks.size(); j++) {
                if (j != i) {
                    if (!(dtagTool.isGoodTrack(othertracks[j]))) continue;
                    if (dtagTool.isPion(othertracks[j])) {
                        RecMdcTrack *mdcTrk = othertracks[j]->mdcTrack();
                        RecMdcKalTrack *mdcKalTrk = othertracks[j]->mdcKalTrack();
                        mdcKalTrk->setPidType(RecMdcKalTrack::pion);
                        for (int k = 0; k < 4; k++) {
                            m_rawp4_otherMdctrk_STDDmiss_sig_D[m_n_othertrks_STDDmiss_sig_D][k] = mdcTrk->p4(mass[2])[k];
                            m_rawp4_otherMdcKaltrk_STDDmiss_sig_D[m_n_othertrks_STDDmiss_sig_D][k] = mdcKalTrk->p4(mass[2])[k];
                        }
                        m_rawp4_otherMdctrk_STDDmiss_sig_D[m_n_othertrks_STDDmiss_sig_D][4] = mdcTrk->chi2();
                        m_rawp4_otherMdctrk_STDDmiss_sig_D[m_n_othertrks_STDDmiss_sig_D][5] = mdcTrk->stat(); // stat: status
                        m_rawp4_otherMdcKaltrk_STDDmiss_sig_D[m_n_othertrks_STDDmiss_sig_D][4] = mdcKalTrk->charge();
                        m_rawp4_otherMdcKaltrk_STDDmiss_sig_D[m_n_othertrks_STDDmiss_sig_D][5] = 2;
                    }
                    if (dtagTool.isKaon(othertracks[j])) {
                        RecMdcTrack *mdcTrk = othertracks[j]->mdcTrack();
                        RecMdcKalTrack *mdcKalTrk = othertracks[j]->mdcKalTrack();
                        mdcKalTrk->setPidType(RecMdcKalTrack::kaon);
                        for (int k = 0; k < 4; k++) {
                            m_rawp4_otherMdctrk_STDDmiss_sig_D[m_n_othertrks_STDDmiss_sig_D][k] = mdcTrk->p4(mass[3])[k];
                            m_rawp4_otherMdcKaltrk_STDDmiss_sig_D[m_n_othertrks_STDDmiss_sig_D][k] = mdcKalTrk->p4(mass[3])[k];
                        }
                        m_rawp4_otherMdctrk_STDDmiss_sig_D[m_n_othertrks_STDDmiss_sig_D][4] = mdcTrk->chi2();
                        m_rawp4_otherMdctrk_STDDmiss_sig_D[m_n_othertrks_STDDmiss_sig_D][5] = mdcTrk->stat(); // stat: status
                        m_rawp4_otherMdcKaltrk_STDDmiss_sig_D[m_n_othertrks_STDDmiss_sig_D][4] = mdcKalTrk->charge();
                        m_rawp4_otherMdcKaltrk_STDDmiss_sig_D[m_n_othertrks_STDDmiss_sig_D][5] = 3;
                    }
                    m_n_othertrks_STDDmiss_sig_D++;
                    if (m_n_othertrks_STDDmiss_sig_D >= 20) continue;
                }
            }
            RecMdcKalTrack *Pi = othertracks[i]->mdcKalTrack();
            for (int j = 0; j < 4; j++) {
                m_rawp4_tagpi_STDDmiss_sig_D[j] = Pi->p4(mass[2])[j];
            }
            SmartRefVector<EvtRecTrack> othershowers = (*dtag_iter)->otherShowers();
            // to find the good photons in the othershowers list
            m_n_othershws_STDDmiss_sig_D = 0;
            for (int j = 0; j < othershowers.size(); j++) {
                if (!(dtagTool.isGoodShower(othershowers[j]))) continue;
                RecEmcShower *gTrk = othershowers[j]->emcShower();
                Hep3Vector Gm_Vec(gTrk->x(), gTrk->y(), gTrk->z());
                Hep3Vector Gm_Mom = Gm_Vec - birth.vx();
                Gm_Mom.setMag(gTrk->energy());
                HepLorentzVector Gm_p4(Gm_Mom, gTrk->energy());
                for (int k = 0; k < 4; k++) m_rawp4_othershw_STDDmiss_sig_D[m_n_othershws_STDDmiss_sig_D][k] = Gm_p4[k];
                m_n_othershws_STDDmiss_sig_D++;
                if (m_n_othershws_STDDmiss_sig_D >= 50) continue;
            }
            recordVariables_STDDmiss_sig_D();
        }
    }

    // signal sample : e+e- --> DpiD*-bar
    vwtrkpara_pi.clear();
    n_pi = 0;
    m_n_trk_STDDmiss_sig_Dst = 0;
    for (int i = 0; i < othertracks.size(); i++) {
        if (!(dtagTool.isGoodTrack(othertracks[i]))) continue;
        if (!judge_continue(mode, charm, m_rawp4_otherMdcKaltrk[i][4], m_rawp4_otherMdcKaltrk[i][5], "sig", "Dst")) continue;
        RecMdcKalTrack *mdcKalTrk = othertracks[i]->mdcKalTrack();
        m_pid_trk_STDDmiss_sig_Dst[m_n_trk_STDDmiss_sig_Dst][0] = mdcKalTrk->p();
        m_pid_trk_STDDmiss_sig_Dst[m_n_trk_STDDmiss_sig_Dst][1] = mdcKalTrk->charge();
        m_pid_trk_STDDmiss_sig_Dst[m_n_trk_STDDmiss_sig_Dst][2] = mdcKalTrk->pz() / mdcKalTrk->p();
        m_pid_trk_STDDmiss_sig_Dst[m_n_trk_STDDmiss_sig_Dst][3] = mdcKalTrk->phi();
        pid->setRecTrack(othertracks[i]);
        pid->usePidSys(pid->useDedx()); // use PID sub-system
        pid->identify(pid->all() | pid->onlyElectron() | pid->onlyMuon() | pid->onlyPion() | pid->onlyKaon() | pid->onlyProton());  
        pid->calculate();
        for (int j = 0; j < 5; j++) {
            if((othertracks[i])->isMdcDedxValid()) {
                RecMdcDedx* dedxtrk = (othertracks[i])->mdcDedx();
                m_pid_trk_STDDmiss_sig_Dst[m_n_trk_STDDmiss_sig_Dst][4 + j] = dedxtrk->chi(j);
                if(!(pid->IsPidInfoValid())) continue;
                m_pid_trk_STDDmiss_sig_Dst[m_n_trk_STDDmiss_sig_Dst][9 + j] = pid->prob(j);
            }
        }
        m_n_trk_STDDmiss_sig_Dst++;
        vwtrkpara_pi.push_back(WTrackParameter(mass[2], mdcKalTrk->getZHelix(), mdcKalTrk->getZError()));
        n_pi++;
        HepLorentzVector pD;
        pD.setPx(0.);
        pD.setPy(0.);
        pD.setPz(0.);
        pD.setE(0.);
        for (int j = 0; j < n_trkD; j++) {
            HepLorentzVector ptrack;
            ptrack.setPx(m_p4_Dtrk[j][0]);
            ptrack.setPy(m_p4_Dtrk[j][1]);
            ptrack.setPz(m_p4_Dtrk[j][2]);
            ptrack.setE(m_p4_Dtrk[j][3]);
            pD += ptrack;
        }
        for(int j = 0; j < n_shwD; j++) {
            HepLorentzVector pshower;
            pshower.setPx(m_p4_Dshw[j][0]);
            pshower.setPy(m_p4_Dshw[j][1]);
            pshower.setPz(m_p4_Dshw[j][2]);
            pshower.setE(m_p4_Dshw[j][3]);
            pD += pshower;
        }
        HepLorentzVector pPi;
        pPi.setPx(mdcKalTrk->p4(mass[2])[0]);
        pPi.setPy(mdcKalTrk->p4(mass[2])[1]);
        pPi.setPz(mdcKalTrk->p4(mass[2])[2]);
        pPi.setE(mdcKalTrk->p4(mass[2])[3]);
        HepLorentzVector ecms(0.011*m_Ecms, 0, 0, m_Ecms);
        double rm_Dpi = (ecms - pD - pPi).m();
        chi2_kf_STDDmiss_sig_Dst = 999;
        m_chi2_kf_STDDmiss_sig_Dst = 999;
        if (m_debug) std::cout << "recoiling mass of Dpi: " << rm_Dpi << ", ecms: " << ecms << std::endl;
        if (rm_Dpi > 1.8 && rm_Dpi < 2.2) {
            m_rm_Dpi_STDDmiss_sig_Dst = rm_Dpi;
            chi2_kf_STDDmiss_sig_Dst = fitKM_STDDmiss_sig_Dst(vwtrkpara_charge, vwtrkpara_photon, vwtrkpara_pi, n_pi - 1, birth);
        }
        if (m_debug) std::cout << "Start recording region info if passed the requirement" << std::endl;
        if (fabs(chi2_kf_STDDmiss_sig_Dst) < 9999.) {
            SmartRefVector<EvtRecTrack> Dtrks = (*dtag_iter)->tracks();
            for (int j = 0; j < n_trkD; j++) {
                RecMdcKalTrack* KalTrk = Dtrks[j]->mdcKalTrack();
                if (dtagTool.isKaon(Dtrks[j])) {
                    KalTrk->setPidType(RecMdcKalTrack::kaon);
                    for (int k = 0; k < 4; k++) m_rawp4_Dtrk_STDDmiss_sig_Dst[j][k] = KalTrk->p4(mass[3])[k];
                    m_rawp4_Dtrk_STDDmiss_sig_Dst[j][4] = KalTrk->charge();
                    m_rawp4_Dtrk_STDDmiss_sig_Dst[j][5] = 3;
                    m_pid_trk_STDDmiss_sig_Dst[m_n_trk_STDDmiss_sig_Dst][0] = KalTrk->p();
                    m_pid_trk_STDDmiss_sig_Dst[m_n_trk_STDDmiss_sig_Dst][1] = KalTrk->charge();
                    m_pid_trk_STDDmiss_sig_Dst[m_n_trk_STDDmiss_sig_Dst][2] = KalTrk->pz() / KalTrk->p();
                    m_pid_trk_STDDmiss_sig_Dst[m_n_trk_STDDmiss_sig_Dst][3] = KalTrk->phi();
                    pid->setRecTrack(Dtrks[j]);
                    pid->usePidSys(pid->useDedx()); // use PID sub-system
                    pid->identify(pid->all() | pid->onlyElectron() | pid->onlyMuon() | pid->onlyPion() | pid->onlyKaon() | pid->onlyProton());  
                    pid->calculate();
                    for (int k = 0; k < 5; k++) {
                        if((Dtrks[j])->isMdcDedxValid()) {
                            RecMdcDedx* dedxtrk = (Dtrks[j])->mdcDedx();
                            m_pid_trk_STDDmiss_sig_Dst[m_n_trk_STDDmiss_sig_Dst][4 + k] = dedxtrk->chi(k);
                            if(!(pid->IsPidInfoValid())) continue;
                            m_pid_trk_STDDmiss_sig_Dst[m_n_trk_STDDmiss_sig_Dst][9 + k] = pid->prob(k);
                        }
                    }
                    m_n_trk_STDDmiss_sig_Dst++;
                } else {
                    KalTrk->setPidType(RecMdcKalTrack::pion);
                    for (int k = 0; k < 4; k++) m_rawp4_Dtrk_STDDmiss_sig_Dst[j][k] = KalTrk->p4(mass[2])[k];
                    m_rawp4_Dtrk_STDDmiss_sig_Dst[j][4] = KalTrk->charge();
                    m_rawp4_Dtrk_STDDmiss_sig_Dst[j][5] = 2;
                    m_pid_trk_STDDmiss_sig_Dst[m_n_trk_STDDmiss_sig_Dst][0] = KalTrk->p();
                    m_pid_trk_STDDmiss_sig_Dst[m_n_trk_STDDmiss_sig_Dst][1] = KalTrk->charge();
                    m_pid_trk_STDDmiss_sig_Dst[m_n_trk_STDDmiss_sig_Dst][2] = KalTrk->pz() / KalTrk->p();
                    m_pid_trk_STDDmiss_sig_Dst[m_n_trk_STDDmiss_sig_Dst][3] = KalTrk->phi();
                    pid->setRecTrack(Dtrks[j]);
                    pid->usePidSys(pid->useDedx()); // use PID sub-system
                    pid->identify(pid->all() | pid->onlyElectron() | pid->onlyMuon() | pid->onlyPion() | pid->onlyKaon() | pid->onlyProton());  
                    pid->calculate();
                    for (int k = 0; k < 5; k++) {
                        if((Dtrks[j])->isMdcDedxValid()) {
                            RecMdcDedx* dedxtrk = (Dtrks[j])->mdcDedx();
                            m_pid_trk_STDDmiss_sig_Dst[m_n_trk_STDDmiss_sig_Dst][4 + k] = dedxtrk->chi(k);
                            if(!(pid->IsPidInfoValid())) continue;
                            m_pid_trk_STDDmiss_sig_Dst[m_n_trk_STDDmiss_sig_Dst][9 + k] = pid->prob(k);
                        }
                    }
                    m_n_trk_STDDmiss_sig_Dst++;
                }
            }
            SmartRefVector<EvtRecTrack> Dshws = (*dtag_iter)->showers();
            for(int j = 0; j < Dshws.size(); j++) {
                RecEmcShower *gTrk = Dshws[j]->emcShower();
                Hep3Vector Gm_Vec(gTrk->x(), gTrk->y(), gTrk->z());
                Hep3Vector Gm_Mom = Gm_Vec - birth.vx();
                Gm_Mom.setMag(gTrk->energy());
                HepLorentzVector Gm_p4(Gm_Mom, gTrk->energy());
                for (int k = 0; k < 4; k++) m_rawp4_Dshw_STDDmiss_sig_Dst[j][k] = Gm_p4[k];
            }
            // to find the good pions and kaons
            m_n_othertrks_STDDmiss_sig_Dst = 0;
            for (int j = 0; j < othertracks.size(); j++) {
                if (j != i) {
                    if (!(dtagTool.isGoodTrack(othertracks[j]))) continue;
                    if (dtagTool.isPion(othertracks[j])) {
                        RecMdcTrack *mdcTrk = othertracks[j]->mdcTrack();
                        RecMdcKalTrack *mdcKalTrk = othertracks[j]->mdcKalTrack();
                        mdcKalTrk->setPidType(RecMdcKalTrack::pion);
                        for (int k = 0; k < 4; k++) {
                            m_rawp4_otherMdctrk_STDDmiss_sig_Dst[m_n_othertrks_STDDmiss_sig_Dst][k] = mdcTrk->p4(mass[2])[k];
                            m_rawp4_otherMdcKaltrk_STDDmiss_sig_Dst[m_n_othertrks_STDDmiss_sig_Dst][k] = mdcKalTrk->p4(mass[2])[k];
                        }
                        m_rawp4_otherMdctrk_STDDmiss_sig_Dst[m_n_othertrks_STDDmiss_sig_Dst][4] = mdcTrk->chi2();
                        m_rawp4_otherMdctrk_STDDmiss_sig_Dst[m_n_othertrks_STDDmiss_sig_Dst][5] = mdcTrk->stat(); // stat: status
                        m_rawp4_otherMdcKaltrk_STDDmiss_sig_Dst[m_n_othertrks_STDDmiss_sig_Dst][4] = mdcKalTrk->charge();
                        m_rawp4_otherMdcKaltrk_STDDmiss_sig_Dst[m_n_othertrks_STDDmiss_sig_Dst][5] = 2;
                    }
                    if (dtagTool.isKaon(othertracks[j])) {
                        RecMdcTrack *mdcTrk = othertracks[j]->mdcTrack();
                        RecMdcKalTrack *mdcKalTrk = othertracks[j]->mdcKalTrack();
                        mdcKalTrk->setPidType(RecMdcKalTrack::kaon);
                        for (int k = 0; k < 4; k++) {
                            m_rawp4_otherMdctrk_STDDmiss_sig_Dst[m_n_othertrks_STDDmiss_sig_Dst][k] = mdcTrk->p4(mass[3])[k];
                            m_rawp4_otherMdcKaltrk_STDDmiss_sig_Dst[m_n_othertrks_STDDmiss_sig_Dst][k] = mdcKalTrk->p4(mass[3])[k];
                        }
                        m_rawp4_otherMdctrk_STDDmiss_sig_Dst[m_n_othertrks_STDDmiss_sig_Dst][4] = mdcTrk->chi2();
                        m_rawp4_otherMdctrk_STDDmiss_sig_Dst[m_n_othertrks_STDDmiss_sig_Dst][5] = mdcTrk->stat(); // stat: status
                        m_rawp4_otherMdcKaltrk_STDDmiss_sig_Dst[m_n_othertrks_STDDmiss_sig_Dst][4] = mdcKalTrk->charge();
                        m_rawp4_otherMdcKaltrk_STDDmiss_sig_Dst[m_n_othertrks_STDDmiss_sig_Dst][5] = 3;
                    }
                    m_n_othertrks_STDDmiss_sig_Dst++;
                    if (m_n_othertrks_STDDmiss_sig_Dst >= 20) continue;
                }
            }
            RecMdcKalTrack *Pi = othertracks[i]->mdcKalTrack();
            for (int j = 0; j < 4; j++) {
                m_rawp4_tagpi_STDDmiss_sig_Dst[j] = Pi->p4(mass[2])[j];
            }
            SmartRefVector<EvtRecTrack> othershowers = (*dtag_iter)->otherShowers();
            // to find the good photons in the othershowers list
            m_n_othershws_STDDmiss_sig_Dst = 0;
            for (int j = 0; j < othershowers.size(); j++) {
                if (!(dtagTool.isGoodShower(othershowers[j]))) continue;
                RecEmcShower *gTrk = othershowers[j]->emcShower();
                Hep3Vector Gm_Vec(gTrk->x(), gTrk->y(), gTrk->z());
                Hep3Vector Gm_Mom = Gm_Vec - birth.vx();
                Gm_Mom.setMag(gTrk->energy());
                HepLorentzVector Gm_p4(Gm_Mom, gTrk->energy());
                for (int k = 0; k < 4; k++) m_rawp4_othershw_STDDmiss_sig_Dst[m_n_othershws_STDDmiss_sig_Dst][k] = Gm_p4[k];
                m_n_othershws_STDDmiss_sig_Dst++;
                if (m_n_othershws_STDDmiss_sig_Dst >= 50) continue;
            }
            recordVariables_STDDmiss_sig_Dst();
        }
    }

    // background sample : e+e- --> DpiD-bar
    vwtrkpara_pi.clear();
    n_pi = 0;
    for (int i = 0; i < othertracks.size(); i++) {
        if (!(dtagTool.isGoodTrack(othertracks[i]))) continue;
        if (!judge_continue(mode, charm, m_rawp4_otherMdcKaltrk[i][4], m_rawp4_otherMdcKaltrk[i][5], "ws_bkg", "D")) continue;
        RecMdcKalTrack *mdcKalTrk = othertracks[i]->mdcKalTrack();
        vwtrkpara_pi.push_back(WTrackParameter(mass[2], mdcKalTrk->getZHelix(), mdcKalTrk->getZError()));
        n_pi++;
        HepLorentzVector pD;
        pD.setPx(0.);
        pD.setPy(0.);
        pD.setPz(0.);
        pD.setE(0.);
        for (int j = 0; j < n_trkD; j++) {
            HepLorentzVector ptrack;
            ptrack.setPx(m_p4_Dtrk[j][0]);
            ptrack.setPy(m_p4_Dtrk[j][1]);
            ptrack.setPz(m_p4_Dtrk[j][2]);
            ptrack.setE(m_p4_Dtrk[j][3]);
            pD += ptrack;
        }
        for(int j = 0; j < n_shwD; j++) {
            HepLorentzVector pshower;
            pshower.setPx(m_p4_Dshw[j][0]);
            pshower.setPy(m_p4_Dshw[j][1]);
            pshower.setPz(m_p4_Dshw[j][2]);
            pshower.setE(m_p4_Dshw[j][3]);
            pD += pshower;
        }
        HepLorentzVector pPi;
        pPi.setPx(mdcKalTrk->p4(mass[2])[0]);
        pPi.setPy(mdcKalTrk->p4(mass[2])[1]);
        pPi.setPz(mdcKalTrk->p4(mass[2])[2]);
        pPi.setE(mdcKalTrk->p4(mass[2])[3]);
        HepLorentzVector ecms(0.011*m_Ecms, 0, 0, m_Ecms);
        double rm_Dpi = (ecms - pD - pPi).m();
        chi2_kf_STDDmiss_bkg_D = 999;
        m_chi2_kf_STDDmiss_bkg_D = 999;
        if (m_debug) std::cout << "recoiling mass of Dpi: " << rm_Dpi << ", ecms: " << ecms << std::endl;
        if (rm_Dpi > 1.6 && rm_Dpi < 2.0) {
            m_rm_Dpi_STDDmiss_bkg_D = rm_Dpi;
            chi2_kf_STDDmiss_bkg_D = fitKM_STDDmiss_bkg_D(vwtrkpara_charge, vwtrkpara_photon, vwtrkpara_pi, n_pi - 1, birth);
        }
        if (m_debug) std::cout << "Start recording region info if passed the requirement" << std::endl;
        if (fabs(chi2_kf_STDDmiss_bkg_D) < 9999.) {
            SmartRefVector<EvtRecTrack> Dtrks = (*dtag_iter)->tracks();
            for (int j = 0; j < n_trkD; j++) {
                RecMdcKalTrack* KalTrk = Dtrks[j]->mdcKalTrack();
                if (dtagTool.isKaon(Dtrks[j])) {
                    KalTrk->setPidType(RecMdcKalTrack::kaon);
                    for (int k = 0; k < 4; k++) m_rawp4_Dtrk_STDDmiss_bkg_D[j][k] = KalTrk->p4(mass[3])[k];
                    m_rawp4_Dtrk_STDDmiss_bkg_D[j][4] = KalTrk->charge();
                    m_rawp4_Dtrk_STDDmiss_bkg_D[j][5] = 3;
                } else {
                    KalTrk->setPidType(RecMdcKalTrack::pion);
                    for (int k = 0; k < 4; k++) m_rawp4_Dtrk_STDDmiss_bkg_D[j][k] = KalTrk->p4(mass[2])[k];
                    m_rawp4_Dtrk_STDDmiss_bkg_D[j][4] = KalTrk->charge();
                    m_rawp4_Dtrk_STDDmiss_bkg_D[j][5] = 2;
                }
            }
            SmartRefVector<EvtRecTrack> Dshws = (*dtag_iter)->showers();
            for(int j = 0; j < Dshws.size(); j++) {
                RecEmcShower *gTrk = Dshws[j]->emcShower();
                Hep3Vector Gm_Vec(gTrk->x(), gTrk->y(), gTrk->z());
                Hep3Vector Gm_Mom = Gm_Vec - birth.vx();
                Gm_Mom.setMag(gTrk->energy());
                HepLorentzVector Gm_p4(Gm_Mom, gTrk->energy());
                for (int k = 0; k < 4; k++) m_rawp4_Dshw_STDDmiss_bkg_D[j][k] = Gm_p4[k];
            }
            // to find the good pions and kaons
            m_n_othertrks_STDDmiss_bkg_D = 0;
            for (int j = 0; j < othertracks.size(); j++) {
                if (j != i) {
                    if (!(dtagTool.isGoodTrack(othertracks[j]))) continue;
                    if (dtagTool.isPion(othertracks[j])) {
                        RecMdcTrack *mdcTrk = othertracks[j]->mdcTrack();
                        RecMdcKalTrack *mdcKalTrk = othertracks[j]->mdcKalTrack();
                        mdcKalTrk->setPidType(RecMdcKalTrack::pion);
                        for (int k = 0; k < 4; k++) {
                            m_rawp4_otherMdctrk_STDDmiss_bkg_D[m_n_othertrks_STDDmiss_bkg_D][k] = mdcTrk->p4(mass[2])[k];
                            m_rawp4_otherMdcKaltrk_STDDmiss_bkg_D[m_n_othertrks_STDDmiss_bkg_D][k] = mdcKalTrk->p4(mass[2])[k];
                        }
                        m_rawp4_otherMdctrk_STDDmiss_bkg_D[m_n_othertrks_STDDmiss_bkg_D][4] = mdcTrk->chi2();
                        m_rawp4_otherMdctrk_STDDmiss_bkg_D[m_n_othertrks_STDDmiss_bkg_D][5] = mdcTrk->stat(); // stat: status
                        m_rawp4_otherMdcKaltrk_STDDmiss_bkg_D[m_n_othertrks_STDDmiss_bkg_D][4] = mdcKalTrk->charge();
                        m_rawp4_otherMdcKaltrk_STDDmiss_bkg_D[m_n_othertrks_STDDmiss_bkg_D][5] = 2;
                    }
                    if (dtagTool.isKaon(othertracks[j])) {
                        RecMdcTrack *mdcTrk = othertracks[j]->mdcTrack();
                        RecMdcKalTrack *mdcKalTrk = othertracks[j]->mdcKalTrack();
                        mdcKalTrk->setPidType(RecMdcKalTrack::kaon);
                        for (int k = 0; k < 4; k++) {
                            m_rawp4_otherMdctrk_STDDmiss_bkg_D[m_n_othertrks_STDDmiss_bkg_D][k] = mdcTrk->p4(mass[3])[k];
                            m_rawp4_otherMdcKaltrk_STDDmiss_bkg_D[m_n_othertrks_STDDmiss_bkg_D][k] = mdcKalTrk->p4(mass[3])[k];
                        }
                        m_rawp4_otherMdctrk_STDDmiss_bkg_D[m_n_othertrks_STDDmiss_bkg_D][4] = mdcTrk->chi2();
                        m_rawp4_otherMdctrk_STDDmiss_bkg_D[m_n_othertrks_STDDmiss_bkg_D][5] = mdcTrk->stat(); // stat: status
                        m_rawp4_otherMdcKaltrk_STDDmiss_bkg_D[m_n_othertrks_STDDmiss_bkg_D][4] = mdcKalTrk->charge();
                        m_rawp4_otherMdcKaltrk_STDDmiss_bkg_D[m_n_othertrks_STDDmiss_bkg_D][5] = 3;
                    }
                    m_n_othertrks_STDDmiss_bkg_D++;
                    if (m_n_othertrks_STDDmiss_bkg_D >= 20) continue;
                }
            }
            RecMdcKalTrack *Pi = othertracks[i]->mdcKalTrack();
            for (int j = 0; j < 4; j++) {
                m_rawp4_tagpi_STDDmiss_bkg_D[j] = Pi->p4(mass[2])[j];
            }
            SmartRefVector<EvtRecTrack> othershowers = (*dtag_iter)->otherShowers();
            // to find the good photons in the othershowers list
            m_n_othershws_STDDmiss_bkg_D = 0;
            for (int j = 0; j < othershowers.size(); j++) {
                if (!(dtagTool.isGoodShower(othershowers[j]))) continue;
                RecEmcShower *gTrk = othershowers[j]->emcShower();
                Hep3Vector Gm_Vec(gTrk->x(), gTrk->y(), gTrk->z());
                Hep3Vector Gm_Mom = Gm_Vec - birth.vx();
                Gm_Mom.setMag(gTrk->energy());
                HepLorentzVector Gm_p4(Gm_Mom, gTrk->energy());
                for (int k = 0; k < 4; k++) m_rawp4_othershw_STDDmiss_bkg_D[m_n_othershws_STDDmiss_bkg_D][k] = Gm_p4[k];
                m_n_othershws_STDDmiss_bkg_D++;
                if (m_n_othershws_STDDmiss_bkg_D >= 50) continue;
            }
            recordVariables_STDDmiss_bkg_D();
        }
    }

    // background sample : e+e- --> DpiD*-bar
    vwtrkpara_pi.clear();
    n_pi = 0;
    for (int i = 0; i < othertracks.size(); i++) {
        if (!(dtagTool.isGoodTrack(othertracks[i]))) continue;
        if (!judge_continue(mode, charm, m_rawp4_otherMdcKaltrk[i][4], m_rawp4_otherMdcKaltrk[i][5], "ws_bkg", "Dst")) continue;
        RecMdcKalTrack *mdcKalTrk = othertracks[i]->mdcKalTrack();
        vwtrkpara_pi.push_back(WTrackParameter(mass[2], mdcKalTrk->getZHelix(), mdcKalTrk->getZError()));
        n_pi++;
        HepLorentzVector pD;
        pD.setPx(0.);
        pD.setPy(0.);
        pD.setPz(0.);
        pD.setE(0.);
        for (int j = 0; j < n_trkD; j++) {
            HepLorentzVector ptrack;
            ptrack.setPx(m_p4_Dtrk[j][0]);
            ptrack.setPy(m_p4_Dtrk[j][1]);
            ptrack.setPz(m_p4_Dtrk[j][2]);
            ptrack.setE(m_p4_Dtrk[j][3]);
            pD += ptrack;
        }
        for(int j = 0; j < n_shwD; j++) {
            HepLorentzVector pshower;
            pshower.setPx(m_p4_Dshw[j][0]);
            pshower.setPy(m_p4_Dshw[j][1]);
            pshower.setPz(m_p4_Dshw[j][2]);
            pshower.setE(m_p4_Dshw[j][3]);
            pD += pshower;
        }
        HepLorentzVector pPi;
        pPi.setPx(mdcKalTrk->p4(mass[2])[0]);
        pPi.setPy(mdcKalTrk->p4(mass[2])[1]);
        pPi.setPz(mdcKalTrk->p4(mass[2])[2]);
        pPi.setE(mdcKalTrk->p4(mass[2])[3]);
        HepLorentzVector ecms(0.011*m_Ecms, 0, 0, m_Ecms);
        double rm_Dpi = (ecms - pD - pPi).m();
        chi2_kf_STDDmiss_bkg_Dst = 999;
        m_chi2_kf_STDDmiss_bkg_Dst = 999;
        if (m_debug) std::cout << "recoiling mass of Dpi: " << rm_Dpi << ", ecms: " << ecms << std::endl;
        if (rm_Dpi > 1.8 && rm_Dpi < 2.2) {
            m_rm_Dpi_STDDmiss_bkg_Dst = rm_Dpi;
            chi2_kf_STDDmiss_bkg_Dst = fitKM_STDDmiss_bkg_Dst(vwtrkpara_charge, vwtrkpara_photon, vwtrkpara_pi, n_pi - 1, birth);
        }
        if (m_debug) std::cout << "Start recording region info if passed the requirement" << std::endl;
        if (fabs(chi2_kf_STDDmiss_bkg_Dst) < 9999.) {
            SmartRefVector<EvtRecTrack> Dtrks = (*dtag_iter)->tracks();
            for (int j = 0; j < n_trkD; j++) {
                RecMdcKalTrack* KalTrk = Dtrks[j]->mdcKalTrack();
                if (dtagTool.isKaon(Dtrks[j])) {
                    KalTrk->setPidType(RecMdcKalTrack::kaon);
                    for (int k = 0; k < 4; k++) m_rawp4_Dtrk_STDDmiss_bkg_Dst[j][k] = KalTrk->p4(mass[3])[k];
                    m_rawp4_Dtrk_STDDmiss_bkg_Dst[j][4] = KalTrk->charge();
                    m_rawp4_Dtrk_STDDmiss_bkg_Dst[j][5] = 3;
                } else {
                    KalTrk->setPidType(RecMdcKalTrack::pion);
                    for (int k = 0; k < 4; k++) m_rawp4_Dtrk_STDDmiss_bkg_Dst[j][k] = KalTrk->p4(mass[2])[k];
                    m_rawp4_Dtrk_STDDmiss_bkg_Dst[j][4] = KalTrk->charge();
                    m_rawp4_Dtrk_STDDmiss_bkg_Dst[j][5] = 2;
                }
            }
            SmartRefVector<EvtRecTrack> Dshws = (*dtag_iter)->showers();
            for(int j = 0; j < Dshws.size(); j++) {
                RecEmcShower *gTrk = Dshws[j]->emcShower();
                Hep3Vector Gm_Vec(gTrk->x(), gTrk->y(), gTrk->z());
                Hep3Vector Gm_Mom = Gm_Vec - birth.vx();
                Gm_Mom.setMag(gTrk->energy());
                HepLorentzVector Gm_p4(Gm_Mom, gTrk->energy());
                for (int k = 0; k < 4; k++) m_rawp4_Dshw_STDDmiss_bkg_Dst[j][k] = Gm_p4[k];
            }
            // to find the good pions and kaons
            m_n_othertrks_STDDmiss_bkg_Dst = 0;
            for (int j = 0; j < othertracks.size(); j++) {
                if (j != i) {
                    if (!(dtagTool.isGoodTrack(othertracks[j]))) continue;
                    if (dtagTool.isPion(othertracks[j])) {
                        RecMdcTrack *mdcTrk = othertracks[j]->mdcTrack();
                        RecMdcKalTrack *mdcKalTrk = othertracks[j]->mdcKalTrack();
                        mdcKalTrk->setPidType(RecMdcKalTrack::pion);
                        for (int k = 0; k < 4; k++) {
                            m_rawp4_otherMdctrk_STDDmiss_bkg_Dst[m_n_othertrks_STDDmiss_bkg_Dst][k] = mdcTrk->p4(mass[2])[k];
                            m_rawp4_otherMdcKaltrk_STDDmiss_bkg_Dst[m_n_othertrks_STDDmiss_bkg_Dst][k] = mdcKalTrk->p4(mass[2])[k];
                        }
                        m_rawp4_otherMdctrk_STDDmiss_bkg_Dst[m_n_othertrks_STDDmiss_bkg_Dst][4] = mdcTrk->chi2();
                        m_rawp4_otherMdctrk_STDDmiss_bkg_Dst[m_n_othertrks_STDDmiss_bkg_Dst][5] = mdcTrk->stat(); // stat: status
                        m_rawp4_otherMdcKaltrk_STDDmiss_bkg_Dst[m_n_othertrks_STDDmiss_bkg_Dst][4] = mdcKalTrk->charge();
                        m_rawp4_otherMdcKaltrk_STDDmiss_bkg_Dst[m_n_othertrks_STDDmiss_bkg_Dst][5] = 2;
                    }
                    if (dtagTool.isKaon(othertracks[j])) {
                        RecMdcTrack *mdcTrk = othertracks[j]->mdcTrack();
                        RecMdcKalTrack *mdcKalTrk = othertracks[j]->mdcKalTrack();
                        mdcKalTrk->setPidType(RecMdcKalTrack::kaon);
                        for (int k = 0; k < 4; k++) {
                            m_rawp4_otherMdctrk_STDDmiss_bkg_Dst[m_n_othertrks_STDDmiss_bkg_Dst][k] = mdcTrk->p4(mass[3])[k];
                            m_rawp4_otherMdcKaltrk_STDDmiss_bkg_Dst[m_n_othertrks_STDDmiss_bkg_Dst][k] = mdcKalTrk->p4(mass[3])[k];
                        }
                        m_rawp4_otherMdctrk_STDDmiss_bkg_Dst[m_n_othertrks_STDDmiss_bkg_Dst][4] = mdcTrk->chi2();
                        m_rawp4_otherMdctrk_STDDmiss_bkg_Dst[m_n_othertrks_STDDmiss_bkg_Dst][5] = mdcTrk->stat(); // stat: status
                        m_rawp4_otherMdcKaltrk_STDDmiss_bkg_Dst[m_n_othertrks_STDDmiss_bkg_Dst][4] = mdcKalTrk->charge();
                        m_rawp4_otherMdcKaltrk_STDDmiss_bkg_Dst[m_n_othertrks_STDDmiss_bkg_Dst][5] = 3;
                    }
                    m_n_othertrks_STDDmiss_bkg_Dst++;
                    if (m_n_othertrks_STDDmiss_bkg_Dst >= 20) continue;
                }
            }
            RecMdcKalTrack *Pi = othertracks[i]->mdcKalTrack();
            for (int j = 0; j < 4; j++) {
                m_rawp4_tagpi_STDDmiss_bkg_Dst[j] = Pi->p4(mass[2])[j];
            }
            SmartRefVector<EvtRecTrack> othershowers = (*dtag_iter)->otherShowers();
            // to find the good photons in the othershowers list
            m_n_othershws_STDDmiss_bkg_Dst = 0;
            for (int j = 0; j < othershowers.size(); j++) {
                if (!(dtagTool.isGoodShower(othershowers[j]))) continue;
                RecEmcShower *gTrk = othershowers[j]->emcShower();
                Hep3Vector Gm_Vec(gTrk->x(), gTrk->y(), gTrk->z());
                Hep3Vector Gm_Mom = Gm_Vec - birth.vx();
                Gm_Mom.setMag(gTrk->energy());
                HepLorentzVector Gm_p4(Gm_Mom, gTrk->energy());
                for (int k = 0; k < 4; k++) m_rawp4_othershw_STDDmiss_bkg_Dst[m_n_othershws_STDDmiss_bkg_Dst][k] = Gm_p4[k];
                m_n_othershws_STDDmiss_bkg_Dst++;
                if (m_n_othershws_STDDmiss_bkg_Dst >= 50) continue;
            }
            recordVariables_STDDmiss_bkg_Dst();
        }
    }

    if (m_n_othertrks >= 20) return false;
    else return true;
}

bool DDecay::saveOthershws() {
    SmartRefVector<EvtRecTrack> othershowers = (*dtag_iter)->otherShowers();
    SmartDataPtr<EvtRecEvent> evtRecEvent(eventSvc(), "/Event/EvtRec/EvtRecEvent");
    if (m_debug) std::cout << " total showers : " << evtRecEvent->totalNeutral() <<endl;
    if (m_debug) std::cout << " other shower numbers : " << othershowers.size() << " for mode " << mode << std::endl;
    DTagTool dtagTool;
    m_n_othershws = 0;
    // to find the good photons in the othershowers list
    vwtrkpara_othershws.clear();
    for (int i = 0; i < othershowers.size(); i++) {
        if (!(dtagTool.isGoodShower(othershowers[i]))) continue;
        RecEmcShower *gTrk = othershowers[i]->emcShower();
        Hep3Vector Gm_Vec(gTrk->x(), gTrk->y(), gTrk->z());
        Hep3Vector Gm_Mom = Gm_Vec - birth.vx();
        Gm_Mom.setMag(gTrk->energy());
        HepLorentzVector Gm_p4(Gm_Mom, gTrk->energy());
        for (int j = 0; j < 4; j++) m_rawp4_othershw[m_n_othershws][j] = Gm_p4[j];
        m_n_othershws++;
        if (m_n_othershws >= 50) return false;
        vwtrkpara_othershws.push_back(WTrackParameter(gTrk->position(), Gm_p4, gTrk->dphi(), gTrk->dtheta(), gTrk->dE()));
    }

    if (m_debug) std::cout << " recorded " << m_n_othershws << " other good showers " << std::endl;
    if (m_n_othershws >= 50) return false;
    else return true;
}

double DDecay::fitVertex(VWTrkPara &vwtrkpara, VertexParameter &birth) {
    VertexParameter vxpar;
    Hep3Vector xorigin(0,0,0);
    HepSymMatrix Exorigin(3,0); // error matrix
    double bx = 1E+6, by = 1E+6, bz = 1E+6;
    Exorigin[0][0] = bx*bx; Exorigin[1][1] = by*by; Exorigin[2][2] = bz*bz;
    vxpar.setVx(xorigin); vxpar.setEvx(Exorigin); // vx: vertex
    vtxfit->init();
    for (int i = 0; i < vwtrkpara.size(); i++) vtxfit->AddTrack(i, vwtrkpara[i]);
    Vint trkId(vwtrkpara.size(), 0); // ????????????????
    for (int i = 0; i < vwtrkpara.size(); i++) trkId[i] = i;
    vtxfit->AddVertex(0, vxpar, trkId);
    if (!vtxfit->Fit(0)) return 9999; // 0: which vertex to fit
    double vf_chi2 = vtxfit->chisq(0);
    vtxfit->Swim(0); // adjust momentum according to errorr matrix
    for (int i = 0; i < vwtrkpara.size(); i++) vwtrkpara[i] = vtxfit->wtrk(i);
    if (vf_chi2 == 1) std::cout << "==========  VF chi2: " << vf_chi2 << std::endl;
    birth = vtxfit->vpar(0);
    return vf_chi2;
}

double DDecay::fitKM(VWTrkPara &vwtrkpara_charge, VWTrkPara &vwtrkpara_photon, VertexParameter &birth) {
    kmfit->init();
    // kmfit->setEspread(0.0011);
    kmfit->setBeamPosition(birth.vx());
    kmfit->setVBeamPosition(birth.Evx()); // error matrix of vertex

    if (m_debug) std::cout << " to start add tracks to KF fit " << std::endl;

    int count = 0;
    Vint D1list;
    Vint Pi0list;
    D1list.clear();
    Pi0list.clear();
    while (count < vwtrkpara_charge.size()) {
        kmfit->AddTrack(count, vwtrkpara_charge[count]);
        D1list.push_back(count);
        count++;
    }
    while (count < vwtrkpara_charge.size() + vwtrkpara_photon.size()) {
        kmfit->AddTrack(count, vwtrkpara_photon[count - vwtrkpara_charge.size()]);
        Pi0list.push_back(count);
        D1list.push_back(count);
        count++;
    }
    int n_res = 0;
    for (int i = 0; i < (Pi0list.size()/2); i++) kmfit->AddResonance(n_res++, M_Pi0, n_trkD + i*2, n_trkD + i*2 + 1); // the last two variales: two gamma tracks
    kmfit->AddResonance(n_res, mDcand, D1list);

    if (m_debug) std::cout << " finished KF add Tracks ... " << std::endl;

    double kf_chi2 = 999;
    if (!kmfit->Fit(0)) return 999;
    if (!kmfit->Fit()) return 999;
    else {
        // kf_chi2 = kmfit->chisq()/(1 + Pi0list.size()/2); // chi2/ndf, 1: constration of mD, Pi0list.size()/2: constration of Pi0
        kf_chi2 = kmfit->chisq();
        if (m_debug) std::cout << "  " << mode << "  fit chisq:   " << kf_chi2 << std::endl;
        for (int i = 0; i < n_trkD; i++) {
            for (int j = 0; j < 4; j++) m_p4_Dtrk[i][j] = kmfit->pfit(i)[j];
        }
        if (m_debug) std::cout << " fill D1trkP4 successfully for mode !!! " << mode << std::endl;
        for (int i = 0; i < n_shwD; i++) {
            for (int j = 0; j < 4; j++) m_p4_Dshw[i][j] = kmfit->pfit(i + n_trkD)[j];
        }
        if (m_debug) std::cout << " recorded the four momentum of showers and tracks in tagged D " << std::endl;
    }
    return kf_chi2;
}

double DDecay::fitKM_STDDmiss_sig_D(VWTrkPara &vwtrkpara_charge, VWTrkPara &vwtrkpara_photon, VWTrkPara &vwtrkpara_pi, int n_pi, VertexParameter &birth) {
    kmfit->init();
    // kmfit->setEspread(0.0011);
    kmfit->setBeamPosition(birth.vx());
    kmfit->setVBeamPosition(birth.Evx()); // error matrix of vertex

    if (m_debug) std::cout << " to start add tracks to KF fit " << std::endl;

    int count = 0;
    Vint D1list;
    Vint Pi0list;
    D1list.clear();
    Pi0list.clear();
    while (count < vwtrkpara_charge.size()) {
        kmfit->AddTrack(count, vwtrkpara_charge[count]);
        D1list.push_back(count);
        count++;
    }
    while (count < vwtrkpara_charge.size() + vwtrkpara_photon.size()) {
        kmfit->AddTrack(count, vwtrkpara_photon[count - vwtrkpara_charge.size()]);
        Pi0list.push_back(count);
        D1list.push_back(count);
        count++;
    }
    int n_res = 0;
    for (int i = 0; i < (Pi0list.size()/2); i++) kmfit->AddResonance(n_res++, M_Pi0, n_trkD + i*2, n_trkD + i*2 + 1); // the last two variales: two gamma tracks
    kmfit->AddResonance(n_res++, mDcand, D1list);
    kmfit->AddTrack(count++, vwtrkpara_pi[n_pi]);
    kmfit->AddMissTrack(count++, mDcand_miss);
    HepLorentzVector ecms(0.011*m_Ecms, 0, 0, m_Ecms);
    kmfit->AddFourMomentum(n_res, ecms);

    if (m_debug) std::cout << " finished KF add Tracks ... " << std::endl;

    double kf_chi2 = 999;
    if (!kmfit->Fit(0)) return 999;
    if (!kmfit->Fit()) return 999;
    else {
        // kf_chi2 = kmfit->chisq()/(1 + Pi0list.size()/2); // chi2/ndf, 1: constration of mD, Pi0list.size()/2: constration of Pi0
        kf_chi2 = kmfit->chisq();
        if (m_debug) std::cout << "  " << mode << "  fit chisq:   " << kf_chi2 << std::endl;
        for (int i = 0; i < n_trkD; i++) {
            for (int j = 0; j < 4; j++) m_p4_Dtrk_STDDmiss_sig_D[i][j] = kmfit->pfit(i)[j];
        }
        if (m_debug) std::cout << " fill D1trkP4 successfully for mode !!! " << mode << std::endl;
        for (int i = 0; i < n_shwD; i++) {
            for (int j = 0; j < 4; j++) m_p4_Dshw_STDDmiss_sig_D[i][j] = kmfit->pfit(i + n_trkD)[j];
        }
        for (int i = 0; i < 4; i++) m_p4_pi_STDDmiss_sig_D[i] = kmfit->pfit(n_trkD + n_shwD)[i];
        for (int i = 0; i < 4; i++) m_p4_Dmiss_STDDmiss_sig_D[i] = kmfit->pfit(n_trkD + n_shwD + 1)[i];
        if (m_debug) std::cout << " recorded the four momentum of showers and tracks in tagged D, pi, and Dmiss..." << std::endl;
    }
    return kf_chi2;
}

double DDecay::fitKM_STDDmiss_sig_Dst(VWTrkPara &vwtrkpara_charge, VWTrkPara &vwtrkpara_photon, VWTrkPara &vwtrkpara_pi, int n_pi, VertexParameter &birth) {
    kmfit->init();
    // kmfit->setEspread(0.0011);
    kmfit->setBeamPosition(birth.vx());
    kmfit->setVBeamPosition(birth.Evx()); // error matrix of vertex

    if (m_debug) std::cout << " to start add tracks to KF fit " << std::endl;

    int count = 0;
    Vint D1list;
    Vint Pi0list;
    D1list.clear();
    Pi0list.clear();
    while (count < vwtrkpara_charge.size()) {
        kmfit->AddTrack(count, vwtrkpara_charge[count]);
        D1list.push_back(count);
        count++;
    }
    while (count < vwtrkpara_charge.size() + vwtrkpara_photon.size()) {
        kmfit->AddTrack(count, vwtrkpara_photon[count - vwtrkpara_charge.size()]);
        Pi0list.push_back(count);
        D1list.push_back(count);
        count++;
    }
    int n_res = 0;
    for (int i = 0; i < (Pi0list.size()/2); i++) kmfit->AddResonance(n_res++, M_Pi0, n_trkD + i*2, n_trkD + i*2 + 1); // the last two variales: two gamma tracks
    kmfit->AddResonance(n_res++, mDcand, D1list);
    kmfit->AddTrack(count++, vwtrkpara_pi[n_pi]);
    kmfit->AddMissTrack(count++, mDcand_miss);
    HepLorentzVector ecms(0.011*m_Ecms, 0, 0, m_Ecms);
    kmfit->AddFourMomentum(n_res, ecms);

    if (m_debug) std::cout << " finished KF add Tracks ... " << std::endl;

    double kf_chi2 = 999;
    if (!kmfit->Fit(0)) return 999;
    if (!kmfit->Fit()) return 999;
    else {
        // kf_chi2 = kmfit->chisq()/(1 + Pi0list.size()/2); // chi2/ndf, 1: constration of mD, Pi0list.size()/2: constration of Pi0
        kf_chi2 = kmfit->chisq();
        if (m_debug) std::cout << "  " << mode << "  fit chisq:   " << kf_chi2 << std::endl;
        for (int i = 0; i < n_trkD; i++) {
            for (int j = 0; j < 4; j++) m_p4_Dtrk_STDDmiss_sig_Dst[i][j] = kmfit->pfit(i)[j];
        }
        if (m_debug) std::cout << " fill D1trkP4 successfully for mode !!! " << mode << std::endl;
        for (int i = 0; i < n_shwD; i++) {
            for (int j = 0; j < 4; j++) m_p4_Dshw_STDDmiss_sig_Dst[i][j] = kmfit->pfit(i + n_trkD)[j];
        }
        for (int i = 0; i < 4; i++) m_p4_pi_STDDmiss_sig_Dst[i] = kmfit->pfit(n_trkD + n_shwD)[i];
        for (int i = 0; i < 4; i++) m_p4_Dmiss_STDDmiss_sig_Dst[i] = kmfit->pfit(n_trkD + n_shwD + 1)[i];
        if (m_debug) std::cout << " recorded the four momentum of showers and tracks in tagged D, pi, and Dmiss..." << std::endl;
    }
    return kf_chi2;
}

double DDecay::fitKM_STDDmiss_bkg_D(VWTrkPara &vwtrkpara_charge, VWTrkPara &vwtrkpara_photon, VWTrkPara &vwtrkpara_pi, int n_pi, VertexParameter &birth) {
    kmfit->init();
    // kmfit->setEspread(0.0011);
    kmfit->setBeamPosition(birth.vx());
    kmfit->setVBeamPosition(birth.Evx()); // error matrix of vertex

    if (m_debug) std::cout << " to start add tracks to KF fit " << std::endl;

    int count = 0;
    Vint D1list;
    Vint Pi0list;
    D1list.clear();
    Pi0list.clear();
    while (count < vwtrkpara_charge.size()) {
        kmfit->AddTrack(count, vwtrkpara_charge[count]);
        D1list.push_back(count);
        count++;
    }
    while (count < vwtrkpara_charge.size() + vwtrkpara_photon.size()) {
        kmfit->AddTrack(count, vwtrkpara_photon[count - vwtrkpara_charge.size()]);
        Pi0list.push_back(count);
        D1list.push_back(count);
        count++;
    }
    int n_res = 0;
    for (int i = 0; i < (Pi0list.size()/2); i++) kmfit->AddResonance(n_res++, M_Pi0, n_trkD + i*2, n_trkD + i*2 + 1); // the last two variales: two gamma tracks
    kmfit->AddResonance(n_res++, mDcand, D1list);
    kmfit->AddTrack(count++, vwtrkpara_pi[n_pi]);
    kmfit->AddMissTrack(count++, mDcand_miss);
    HepLorentzVector ecms(0.011*m_Ecms, 0, 0, m_Ecms);
    kmfit->AddFourMomentum(n_res, ecms);

    if (m_debug) std::cout << " finished KF add Tracks ... " << std::endl;

    double kf_chi2 = 999;
    if (!kmfit->Fit(0)) return 999;
    if (!kmfit->Fit()) return 999;
    else {
        // kf_chi2 = kmfit->chisq()/(1 + Pi0list.size()/2); // chi2/ndf, 1: constration of mD, Pi0list.size()/2: constration of Pi0
        kf_chi2 = kmfit->chisq();
        if (m_debug) std::cout << "  " << mode << "  fit chisq:   " << kf_chi2 << std::endl;
        for (int i = 0; i < n_trkD; i++) {
            for (int j = 0; j < 4; j++) m_p4_Dtrk_STDDmiss_bkg_D[i][j] = kmfit->pfit(i)[j];
        }
        if (m_debug) std::cout << " fill D1trkP4 successfully for mode !!! " << mode << std::endl;
        for (int i = 0; i < n_shwD; i++) {
            for (int j = 0; j < 4; j++) m_p4_Dshw_STDDmiss_bkg_D[i][j] = kmfit->pfit(i + n_trkD)[j];
        }
        for (int i = 0; i < 4; i++) m_p4_pi_STDDmiss_bkg_D[i] = kmfit->pfit(n_trkD + n_shwD)[i];
        for (int i = 0; i < 4; i++) m_p4_Dmiss_STDDmiss_bkg_D[i] = kmfit->pfit(n_trkD + n_shwD + 1)[i];
        if (m_debug) std::cout << " recorded the four momentum of showers and tracks in tagged D, pi, and Dmiss..." << std::endl;
    }
    return kf_chi2;
}

double DDecay::fitKM_STDDmiss_bkg_Dst(VWTrkPara &vwtrkpara_charge, VWTrkPara &vwtrkpara_photon, VWTrkPara &vwtrkpara_pi, int n_pi, VertexParameter &birth) {
    kmfit->init();
    // kmfit->setEspread(0.0011);
    kmfit->setBeamPosition(birth.vx());
    kmfit->setVBeamPosition(birth.Evx()); // error matrix of vertex

    if (m_debug) std::cout << " to start add tracks to KF fit " << std::endl;

    int count = 0;
    Vint D1list;
    Vint Pi0list;
    D1list.clear();
    Pi0list.clear();
    while (count < vwtrkpara_charge.size()) {
        kmfit->AddTrack(count, vwtrkpara_charge[count]);
        D1list.push_back(count);
        count++;
    }
    while (count < vwtrkpara_charge.size() + vwtrkpara_photon.size()) {
        kmfit->AddTrack(count, vwtrkpara_photon[count - vwtrkpara_charge.size()]);
        Pi0list.push_back(count);
        D1list.push_back(count);
        count++;
    }
    int n_res = 0;
    for (int i = 0; i < (Pi0list.size()/2); i++) kmfit->AddResonance(n_res++, M_Pi0, n_trkD + i*2, n_trkD + i*2 + 1); // the last two variales: two gamma tracks
    kmfit->AddResonance(n_res++, mDcand, D1list);
    kmfit->AddTrack(count++, vwtrkpara_pi[n_pi]);
    kmfit->AddMissTrack(count++, mDcand_miss);
    HepLorentzVector ecms(0.011*m_Ecms, 0, 0, m_Ecms);
    kmfit->AddFourMomentum(n_res, ecms);

    if (m_debug) std::cout << " finished KF add Tracks ... " << std::endl;

    double kf_chi2 = 999;
    if (!kmfit->Fit(0)) return 999;
    if (!kmfit->Fit()) return 999;
    else {
        // kf_chi2 = kmfit->chisq()/(1 + Pi0list.size()/2); // chi2/ndf, 1: constration of mD, Pi0list.size()/2: constration of Pi0
        kf_chi2 = kmfit->chisq();
        if (m_debug) std::cout << "  " << mode << "  fit chisq:   " << kf_chi2 << std::endl;
        for (int i = 0; i < n_trkD; i++) {
            for (int j = 0; j < 4; j++) m_p4_Dtrk_STDDmiss_bkg_Dst[i][j] = kmfit->pfit(i)[j];
        }
        if (m_debug) std::cout << " fill D1trkP4 successfully for mode !!! " << mode << std::endl;
        for (int i = 0; i < n_shwD; i++) {
            for (int j = 0; j < 4; j++) m_p4_Dshw_STDDmiss_bkg_Dst[i][j] = kmfit->pfit(i + n_trkD)[j];
        }
        for (int i = 0; i < 4; i++) m_p4_pi_STDDmiss_bkg_Dst[i] = kmfit->pfit(n_trkD + n_shwD)[i];
        for (int i = 0; i < 4; i++) m_p4_Dmiss_STDDmiss_bkg_Dst[i] = kmfit->pfit(n_trkD + n_shwD + 1)[i];
        if (m_debug) std::cout << " recorded the four momentum of showers and tracks in tagged D, pi, and Dmiss..." << std::endl;
    }
    return kf_chi2;
}


double DDecay::ECMS(int runNo) {
    if (runNo >= 33659 && runNo <= 33719) return 4.08545;
    else if (runNo >= 63075 && runNo <= 63515) return 4.6260;
    else if (runNo >= 63516 && runNo <= 63715) return 4.6400;
    else if (runNo >= 63718 && runNo <= 63852) return 4.6600;
    else if (runNo >= 63867 && runNo <= 64015) return 4.6800;
    else if (runNo >= 64028 && runNo <= 64289) return 4.7000;
    // else if (runNo >= 35227 && runNo <= 35473) return 4.6260;
    // else if (runNo >= 35474 && runNo <= 35720) return 4.6400;
    // else if (runNo >= 35721 && runNo <= 35967) return 4.6600;
    // else if (runNo >= 35227 && runNo <= 36213) return 4.6800;
    // else if (runNo >= 36114 && runNo <= 36213) return 4.7000;
    else return 999.;
}

bool DDecay::judge_continue(int mode, int charm, double charge_par, double type_par, std::string sample_type, std::string Dmiss_type) {
    bool status = false;
    if (sample_type == "sig") {
        if (mode == 200) {
            if (charm == 1 && type_par == 2 && charge_par == -1) status = true;
            if (charm == -1 && type_par == 2 && charge_par == 1) status = true;
            if (Dmiss_type == "D") mDcand_miss = M_D0;
            if (Dmiss_type == "Dst") mDcand_miss = M_D0st;
        }
        if (mode == 0 or mode == 1 or mode == 3) {
            if (charm == 1 && type_par == 2 && charge_par == 1) status = true;
            if (charm == -1 && type_par == 2 && charge_par == -1) status = true;
            if (Dmiss_type == "D") mDcand_miss = M_Dplus;
            if (Dmiss_type == "Dst") mDcand_miss = M_Dplusst;
        }
    }
    if (sample_type == "ws_bkg") {
        if (mode == 200) {
            if (charm == 1 && type_par == 2 && charge_par == 1) status = true;
            if (charm == -1 && type_par == 2 && charge_par == -1) status = true;
            if (Dmiss_type == "D") mDcand_miss = M_D0;
            if (Dmiss_type == "Dst") mDcand_miss = M_D0st;
        }
        if (mode == 0 or mode == 1 or mode == 3) {
            if (charm == 1 && type_par == 2 && charge_par == -1) status = true;
            if (charm == -1 && type_par == 2 && charge_par == 1) status = true;
            if (Dmiss_type == "D") mDcand_miss = M_Dplus;
            if (Dmiss_type == "Dst") mDcand_miss = M_Dplusst;
        }
    }
    if (m_debug){
        if (status) std::cout << "Passed! " << "mode: " << mode << ", charm: " << charm << ", type(2: pion, 3: kaon): " << type_par << ", charge: " << charge_par << ", sample_type " << sample_type << ", Dmiss_type: " << Dmiss_type << "..." << std::endl;
        if (!status) std::cout << "Failed!" << std::endl;
    }
    return status;
}

void DDecay::recordVariables() {
    m_runNo = runNo;
    m_evtNo = evtNo;
    m_flag1 = flag1;

    // save all McTruth info
    if (m_runNo < 0 && m_isMonteCarlo) {
        m_idxmc = idxmc;
        for (int i = 0; i< m_idxmc; i++) {
            m_motheridx[i] = motheridx[i];
            m_pdgid[i] = pdgid[i];
        }
    }

    for (int i = 0; i < 4; i++) {
        m_p4_pi_MCTruth[i] = p4_pi_MCTruth[i];
        m_p4_D_MCTruth[i] = p4_D_MCTruth[i];
        m_p4_Dbar_MCTruth[i] = p4_Dbar_MCTruth[i];
    }

    // save DpDm McTruth info
    m_Id_Dp = DpId;
    m_Id_Dm = DmId;

    // save DTag inDststfo
    m_n_trkD = n_trkD;
    m_n_shwD = n_shwD;
    m_mode = MODE;
    m_charm = charm;
    m_chi2_vf = chi2_vf;
    m_chi2_kf = chi2_kf;
    m_n_count = n_count;

    m_tuple1->write();
    Ncut6++;

    if (m_debug) std::cout << " entry in ntuple is filled for " << mode << std::endl;
}

void DDecay::recordVariables_STDDmiss_sig_D() {
    m_runNo_STDDmiss_sig_D = runNo;
    m_evtNo_STDDmiss_sig_D = evtNo;
    m_flag1_STDDmiss_sig_D = flag1;

    // save all McTruth info
    if (m_runNo_STDDmiss_sig_D < 0 && m_isMonteCarlo) {
        m_idxmc_STDDmiss_sig_D = idxmc;
        for (int i = 0; i< m_idxmc_STDDmiss_sig_D; i++) {
            m_motheridx_STDDmiss_sig_D[i] = motheridx[i];
            m_pdgid_STDDmiss_sig_D[i] = pdgid[i];
        }
    }

    for (int i = 0; i < 4; i++) {
        m_p4_pi_MCTruth_STDDmiss_sig_D[i] = p4_pi_MCTruth[i];
        m_p4_D_MCTruth_STDDmiss_sig_D[i] = p4_D_MCTruth[i];
        m_p4_Dbar_MCTruth_STDDmiss_sig_D[i] = p4_Dbar_MCTruth[i];
    }

    // save DpDm McTruth info
    m_Id_Dp_STDDmiss_sig_D = DpId;
    m_Id_Dm_STDDmiss_sig_D = DmId;

    // save DTag inDststfo
    m_n_trkD_STDDmiss_sig_D = n_trkD;
    m_n_shwD_STDDmiss_sig_D = n_shwD;
    m_mode_STDDmiss_sig_D = MODE;
    m_charm_STDDmiss_sig_D = charm;
    m_chi2_vf_STDDmiss_sig_D = chi2_vf;
    m_chi2_kf_STDDmiss_sig_D = chi2_kf_STDDmiss_sig_D;

    m_tuple2->write();

    if (m_debug) std::cout << " entry in ntuple is filled for " << mode << std::endl;
}

void DDecay::recordVariables_STDDmiss_sig_Dst() {
    m_runNo_STDDmiss_sig_Dst = runNo;
    m_evtNo_STDDmiss_sig_Dst = evtNo;
    m_flag1_STDDmiss_sig_Dst = flag1;

    // save all McTruth info
    if (m_runNo_STDDmiss_sig_Dst < 0 && m_isMonteCarlo) {
        m_idxmc_STDDmiss_sig_Dst = idxmc;
        for (int i = 0; i< m_idxmc_STDDmiss_sig_Dst; i++) {
            m_motheridx_STDDmiss_sig_Dst[i] = motheridx[i];
            m_pdgid_STDDmiss_sig_Dst[i] = pdgid[i];
        }
    }

    // save DpDm McTruth info
    m_Id_Dp_STDDmiss_sig_Dst = DpId;
    m_Id_Dm_STDDmiss_sig_Dst = DmId;

    // save DTag inDststfo
    m_n_trkD_STDDmiss_sig_Dst = n_trkD;
    m_n_shwD_STDDmiss_sig_Dst = n_shwD;
    m_mode_STDDmiss_sig_Dst = MODE;
    m_charm_STDDmiss_sig_Dst = charm;
    m_chi2_vf_STDDmiss_sig_Dst = chi2_vf;
    m_chi2_kf_STDDmiss_sig_Dst = chi2_kf_STDDmiss_sig_Dst;

    m_tuple3->write();

    if (m_debug) std::cout << " entry in ntuple is filled for " << mode << std::endl;
}

void DDecay::recordVariables_STDDmiss_bkg_D() {
    m_runNo_STDDmiss_bkg_D = runNo;
    m_evtNo_STDDmiss_bkg_D = evtNo;
    m_flag1_STDDmiss_bkg_D = flag1;

    // save all McTruth info
    if (m_runNo_STDDmiss_bkg_D < 0 && m_isMonteCarlo) {
        m_idxmc_STDDmiss_bkg_D = idxmc;
        for (int i = 0; i< m_idxmc_STDDmiss_bkg_D; i++) {
            m_motheridx_STDDmiss_bkg_D[i] = motheridx[i];
            m_pdgid_STDDmiss_bkg_D[i] = pdgid[i];
        }
    }

    // save DpDm McTruth info
    m_Id_Dp_STDDmiss_bkg_D = DpId;
    m_Id_Dm_STDDmiss_bkg_D = DmId;

    // save DTag inDststfo
    m_n_trkD_STDDmiss_bkg_D = n_trkD;
    m_n_shwD_STDDmiss_bkg_D = n_shwD;
    m_mode_STDDmiss_bkg_D = MODE;
    m_charm_STDDmiss_bkg_D = charm;
    m_chi2_vf_STDDmiss_bkg_D = chi2_vf;
    m_chi2_kf_STDDmiss_bkg_D = chi2_kf_STDDmiss_bkg_D;

    m_tuple4->write();

    if (m_debug) std::cout << " entry in ntuple is filled for " << mode << std::endl;
}

void DDecay::recordVariables_STDDmiss_bkg_Dst() {
    m_runNo_STDDmiss_bkg_Dst = runNo;
    m_evtNo_STDDmiss_bkg_Dst = evtNo;
    m_flag1_STDDmiss_bkg_Dst = flag1;

    // save all McTruth info
    if (m_runNo_STDDmiss_bkg_Dst < 0 && m_isMonteCarlo) {
        m_idxmc_STDDmiss_bkg_Dst = idxmc;
        for (int i = 0; i< m_idxmc_STDDmiss_bkg_Dst; i++) {
            m_motheridx_STDDmiss_bkg_Dst[i] = motheridx[i];
            m_pdgid_STDDmiss_bkg_Dst[i] = pdgid[i];
        }
    }

    // save DpDm McTruth info
    m_Id_Dp_STDDmiss_bkg_Dst = DpId;
    m_Id_Dm_STDDmiss_bkg_Dst = DmId;

    // save DTag inDststfo
    m_n_trkD_STDDmiss_bkg_Dst = n_trkD;
    m_n_shwD_STDDmiss_bkg_Dst = n_shwD;
    m_mode_STDDmiss_bkg_Dst = MODE;
    m_charm_STDDmiss_bkg_Dst = charm;
    m_chi2_vf_STDDmiss_bkg_Dst = chi2_vf;
    m_chi2_kf_STDDmiss_bkg_Dst = chi2_kf_STDDmiss_bkg_Dst;

    m_tuple5->write();

    if (m_debug) std::cout << " entry in ntuple is filled for " << mode << std::endl;
}
