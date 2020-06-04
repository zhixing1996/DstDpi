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
const double M_Dplus = 1.86965;
const double M_Dst = 2.01026;
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
            status = m_tuple1->addIndexedItem("rawp4_otherMdcKaltrk", m_n_othertrks, 7, m_rawp4_otherMdcKaltrk);
            status = m_tuple1->addItem("n_othershws", m_n_othershws, 0, 50);
            status = m_tuple1->addIndexedItem("rawp4_othershw", m_n_othershws, 4, m_rawp4_othershw);
            status = m_tuple1->addItem("indexmc", m_idxmc, 0, 100);
            status = m_tuple1->addIndexedItem("pdgid", m_idxmc, m_pdgid);
            status = m_tuple1->addIndexedItem("motheridx", m_idxmc, m_motheridx);
        }
        else {
            log << MSG::ERROR << "Cannot book N-tuple:" << long(m_tuple1) << endmsg;
            return StatusCode::FAILURE;
        }
    }

    NTuplePtr nt2(ntupleSvc(), "FILE1/STDDmiss");
    if (nt2) m_tuple2 = nt2;
    else {
        m_tuple2 = ntupleSvc()->book("FILE1/STDDmiss", CLID_ColumnWiseTuple, "Single tag D decay with kinematic fit missing a D in signal region and sideband region");
        if (m_tuple2) {
            status = m_tuple2->addItem("runNo", m_runNo_STDDmiss);
            status = m_tuple2->addItem("evtNo", m_evtNo_STDDmiss);
            status = m_tuple2->addItem("flag1", m_flag1_STDDmiss);
            status = m_tuple2->addItem("n_trkD", m_n_trkD_STDDmiss, 0, 5); // number of members should locates in 0~5
            status = m_tuple2->addIndexedItem("rawp4_Dtrk", m_n_trkD_STDDmiss, 6, m_rawp4_Dtrk_STDDmiss); // four members array
            status = m_tuple2->addIndexedItem("p4_Dtrkold", m_n_trkD_STDDmiss, 4, m_p4_Dtrkold_STDDmiss);
            status = m_tuple2->addIndexedItem("p4_Dtrk", m_n_trkD_STDDmiss, 4, m_p4_Dtrk_STDDmiss);
            status = m_tuple2->addIndexedItem("p4_Dtrk_low", m_n_trkD_STDDmiss, 4, m_p4_Dtrk_STDDmiss_low);
            status = m_tuple2->addIndexedItem("p4_Dtrk_up", m_n_trkD_STDDmiss, 4, m_p4_Dtrk_STDDmiss_up);
            status = m_tuple2->addItem("n_shwD", m_n_shwD_STDDmiss, 0, 2); 
            status = m_tuple2->addIndexedItem("rawp4_Dshw", m_n_shwD_STDDmiss, 4, m_rawp4_Dshw_STDDmiss);
            status = m_tuple2->addIndexedItem("p4_Dshwold", m_n_shwD_STDDmiss, 4, m_p4_Dshwold_STDDmiss);
            status = m_tuple2->addIndexedItem("p4_Dshw", m_n_shwD_STDDmiss, 4, m_p4_Dshw_STDDmiss);
            status = m_tuple2->addIndexedItem("p4_Dshw_low", m_n_shwD_STDDmiss, 4, m_p4_Dshw_STDDmiss_low);
            status = m_tuple2->addIndexedItem("p4_Dshw_up", m_n_shwD_STDDmiss, 4, m_p4_Dshw_STDDmiss_up);
            status = m_tuple2->addItem("p4_pi", 4, m_p4_pi_STDDmiss);
            status = m_tuple2->addItem("p4_pi_low", 4, m_p4_pi_STDDmiss_low);
            status = m_tuple2->addItem("p4_pi_up", 4, m_p4_pi_STDDmiss_up);
            status = m_tuple2->addItem("p4_Dmiss", 4, m_p4_Dmiss_STDDmiss);
            status = m_tuple2->addItem("p4_Dmiss_low", 4, m_p4_Dmiss_STDDmiss_low);
            status = m_tuple2->addItem("p4_Dmiss_up", 4, m_p4_Dmiss_STDDmiss_up);
            status = m_tuple2->addItem("mode", m_mode_STDDmiss);
            status = m_tuple2->addItem("charm", m_charm_STDDmiss);
            status = m_tuple2->addItem("chi2_vf", m_chi2_vf_STDDmiss);
            status = m_tuple2->addItem("chi2_kf", m_chi2_kf_STDDmiss);
            status = m_tuple2->addItem("chi2_kf_low", m_chi2_kf_STDDmiss_low);
            status = m_tuple2->addItem("chi2_kf_up", m_chi2_kf_STDDmiss_up);
            status = m_tuple2->addItem("indexmc", m_idxmc_STDDmiss, 0, 100);
            status = m_tuple2->addIndexedItem("pdgid", m_idxmc_STDDmiss, m_pdgid_STDDmiss);
            status = m_tuple2->addIndexedItem("motheridx", m_idxmc_STDDmiss, m_motheridx_STDDmiss);
            status = m_tuple2->addItem("n_othertrks", m_n_othertrks_STDDmiss, 0, 20);
            status = m_tuple2->addIndexedItem("rawp4_otherMdctrk", m_n_othertrks_STDDmiss, 6, m_rawp4_otherMdctrk_STDDmiss);
            status = m_tuple2->addIndexedItem("rawp4_otherMdcKaltrk", m_n_othertrks_STDDmiss, 6, m_rawp4_otherMdcKaltrk_STDDmiss);
            status = m_tuple2->addItem("rawp4_tagPi", 4, m_rawp4_tagPi_STDDmiss);
            status = m_tuple2->addItem("n_othershws", m_n_othershws_STDDmiss, 0, 50);
            status = m_tuple2->addIndexedItem("rawp4_othershw", m_n_othershws_STDDmiss, 4, m_rawp4_othershw_STDDmiss);
            status = m_tuple2->addItem("rm_Dpi", m_rm_Dpi_STDDmiss);
        }
        else {
            log << MSG::ERROR << "Cannot book N-tuple:" << long(m_tuple2) << endmsg;
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
    std::cout << "Ncut7: " << Ncut7 << std::endl;
    std::cout << "Ncut8: " << Ncut8 << std::endl;
    std::cout << "Ncut9: " << Ncut9 << std::endl;
    std::cout << "Ncut10: " << Ncut10 << std::endl;
    std::cout << "Ncut11: " << Ncut11 << std::endl;
    std::cout << "Ncut12: " << Ncut12 << std::endl;
    std::cout << "Ncut13: " << Ncut13 << std::endl;
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
    m_runNo_STDDmiss = 0;
    m_evtNo_STDDmiss = 0;
    m_flag1_STDDmiss = 0;

    // all McTruth info
    m_idxmc = 0;

    // single D tag
    m_n_trkD = 0;
    m_n_trkD_STDDmiss = 0;
    n_trkD = 0;
    m_n_shwD = 0;
    m_n_shwD_STDDmiss = 0;
    n_shwD = 0;
    MODE = -999;
    mode = -999;
    m_mode = -999;
    m_mode_STDDmiss = -999;
    charm = -999;
    m_charm = -999;
    m_charm_STDDmiss = -999;
    chi2_vf = 999;
    m_chi2_vf = 999;
    m_chi2_vf_STDDmiss = 999;
    chi2_kf = 999;
    m_chi2_kf = 999;
    mDcand = 0;
    n_count = 0;
    m_n_count = 0;

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
            }
            // to fill Pion candidates
            else {
                KalTrk->setPidType(RecMdcKalTrack::pion);
                if (m_debug) std::cout << " filling pion track " << std::endl;
                vwtrkpara_charge.push_back(WTrackParameter(mass[2], KalTrk->getZHelix(), KalTrk->getZError()));
                for (int k = 0; k < 4; k++) m_rawp4_Dtrk[j][k] = KalTrk->p4(mass[2])[k];
                m_rawp4_Dtrk[j][4] = KalTrk->charge();
                m_rawp4_Dtrk[j][5] = 2;
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

double DDecay::fitKM_STDDmiss(VWTrkPara &vwtrkpara_charge, VWTrkPara &vwtrkpara_photon, VWTrkPara &vwtrkpara_pi, int n_pi, VertexParameter &birth) {
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
    kmfit->AddMissTrack(count++, M_D0);
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
            for (int j = 0; j < 4; j++) m_p4_Dtrk_STDDmiss[i][j] = kmfit->pfit(i)[j];
        }
        if (m_debug) std::cout << " fill DtrkP4 successfully for mode !!! " << mode << std::endl;
        for (int i = 0; i < n_shwD; i++) {
            for (int j = 0; j < 4; j++) m_p4_Dshw_STDDmiss[i][j] = kmfit->pfit(i + n_trkD)[j];
        }
        for (int i = 0; i < 4; i++) m_p4_pi_STDDmiss[i] = kmfit->pfit(n_trkD + n_shwD)[i];
        for (int i = 0; i < 4; i++) m_p4_Dmiss_STDDmiss[i] = kmfit->pfit(n_trkD + n_shwD + 1)[i];
        if (m_debug) std::cout << " recorded the four momentum of showers and tracks in tagged D " << std::endl;
    }
    return kf_chi2;
}

double DDecay::fitKM_STDDmiss_low(VWTrkPara &vwtrkpara_charge, VWTrkPara &vwtrkpara_photon, VWTrkPara &vwtrkpara_pi, int n_pi, VertexParameter &birth, double sidebandlow_mean) {
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
    kmfit->AddMissTrack(count++, sidebandlow_mean);
    HepLorentzVector ecms(0.011*m_Ecms, 0 , 0, m_Ecms);
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
            for (int j = 0; j < 4; j++) m_p4_Dtrk_STDDmiss_low[i][j] = kmfit->pfit(i)[j];
        }
        if (m_debug) std::cout << " fill DtrkP4 successfully for mode: " << mode << std::endl;
        for (int i = 0; i < n_shwD; i++) {
            for (int j = 0; j < 4; j++) m_p4_Dshw_STDDmiss_low[i][j] = kmfit->pfit(i + n_trkD)[j];
        }
        for (int i = 0; i < 4; i++) m_p4_pi_STDDmiss_low[i] = kmfit->pfit(n_trkD + n_shwD)[i];
        for (int i = 0; i < 4; i++) m_p4_Dmiss_STDDmiss_low[i] = kmfit->pfit(n_trkD + n_shwD + 1)[i];
        if (m_debug) std::cout << " recorded the four momentum of showers and tracks in tagged D " << std::endl;
    }
    return kf_chi2;
}

double DDecay::fitKM_STDDmiss_up(VWTrkPara &vwtrkpara_charge, VWTrkPara &vwtrkpara_photon, VWTrkPara &vwtrkpara_pi, int n_pi, VertexParameter &birth, double sidebandup_mean) {
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
    kmfit->AddMissTrack(count++, sidebandup_mean);
    HepLorentzVector ecms(0.011*m_Ecms, 0 , 0, m_Ecms);
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
            for (int j = 0; j < 4; j++) m_p4_Dtrk_STDDmiss_up[i][j] = kmfit->pfit(i)[j];
        }
        if (m_debug) std::cout << " fill DtrkP4 successfully for mode !!! " << mode << std::endl;
        for (int i = 0; i < n_shwD; i++) {
            for (int j = 0; j < 4; j++) m_p4_Dshw_STDDmiss_up[i][j] = kmfit->pfit(i + n_trkD)[j];
        }
        for (int i = 0; i < 4; i++) m_p4_pi_STDDmiss_up[i] = kmfit->pfit(n_trkD + n_shwD)[i];
        for (int i = 0; i < 4; i++) m_p4_Dmiss_STDDmiss_up[i] = kmfit->pfit(n_trkD + n_shwD + 1)[i];
        if (m_debug) std::cout << " recorded the four momentum of showers and tracks in tagged D " << std::endl;
    }
    return kf_chi2;
}

bool DDecay::saveOthertrks(VWTrkPara &vwtrkpara_charge, VWTrkPara &vwtrkpara_photon, VertexParameter &birth) {
    SmartRefVector<EvtRecTrack> othertracks = (*dtag_iter)->otherTracks();
    SmartDataPtr<EvtRecEvent> evtRecEvent(eventSvc(), "/Event/EvtRec/EvtRecEvent");
    if (m_debug) std::cout << " total charged tracks : " << evtRecEvent->totalCharged() << std::endl;
    if (m_debug) std::cout << " other track number : " << othertracks.size() << " for mode " << mode << std::endl;
    DTagTool dtagTool;
    m_n_othertrks = 0;
    // to find the good pions and kaons
    for (int i = 0; i < othertracks.size(); i++) {
        if (!(dtagTool.isGoodTrack(othertracks[i]))) continue;
        Ncut5++;
        if (dtagTool.isPion(othertracks[i])) {
            RecMdcTrack *mdcTrk = othertracks[i]->mdcTrack();
            RecMdcKalTrack *mdcKalTrk = othertracks[i]->mdcKalTrack();
            mdcKalTrk->setPidType(RecMdcKalTrack::pion);
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
                m_rawp4_otherMdctrk[m_n_othertrks][j] = mdcTrk->p4(mass[2])[j];
                m_rawp4_otherMdcKaltrk[m_n_othertrks][j] = mdcKalTrk->p4(mass[3])[j];
            }
            m_rawp4_otherMdctrk[m_n_othertrks][4] = mdcTrk->chi2();
            m_rawp4_otherMdctrk[m_n_othertrks][5] = mdcTrk->stat(); // stat: status
            m_rawp4_otherMdcKaltrk[m_n_othertrks][4] = mdcKalTrk->charge();
            m_rawp4_otherMdcKaltrk[m_n_othertrks][5] = 3;
            m_rawp4_otherMdcKaltrk[m_n_othertrks][6] = -999;
        }
        m_n_othertrks++;
        if (m_n_othertrks >= 20) return false;
    }

    VWTrkPara vwtrkpara_pi;
    vwtrkpara_pi.clear();
    int n_pi = 0;
    for (int i = 0; i < othertracks.size(); i++) {
        if (!(dtagTool.isGoodTrack(othertracks[i]))) continue;
        if (!judge_continue(mode, charm, m_rawp4_otherMdcKaltrk[i][4], m_rawp4_otherMdcKaltrk[i][5])) continue;
        Ncut7++;
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
        double cms = 0;
        double signal_low = 0;
        double signal_up = 0;
        double sidebandlow_low = 0;
        double sidebandlow_up = 0;
        double sidebandup_low = 0;
        double sidebandup_up = 0;
        double low = 0;
        double up = 0;
        cms = m_Ecms;
        signal_low = M_D0 - 0.024/2.;
        signal_up = M_D0 + 0.024/2.;
        sidebandup_low = signal_up + (signal_up - signal_low);
        sidebandup_up = sidebandup_low + (signal_up - signal_low);
        sidebandlow_up = signal_low - (signal_up - signal_low);
        sidebandlow_low = sidebandlow_up - (signal_up - signal_low);
        low = 1.6;
        up = 2.0;
        if (m_debug) {
            std::cout << "Signal region, Sidebandup region, Sidebandlow region..." << std::endl;
            std::cout << "[" << signal_low << "," << signal_up << "], " << "[" << sidebandup_low << ", " << sidebandup_up << "], " << "[" << sidebandlow_low << ", " << sidebandlow_up << "]" << std::endl;
        }
        HepLorentzVector ecms(0.011*cms, 0, 0, cms);
        double rm_Dpi = (ecms - pD - pPi).m();
        chi2_kf_STDDmiss = 999;
        m_chi2_kf_STDDmiss = 999;
        chi2_kf_STDDmiss_low = 999;
        m_chi2_kf_STDDmiss_low = 999;
        chi2_kf_STDDmiss_up = 999;
        m_chi2_kf_STDDmiss_up = 999;
        if (rm_Dpi > low && rm_Dpi< up) {
            m_rm_Dpi_STDDmiss = rm_Dpi;
            chi2_kf_STDDmiss = fitKM_STDDmiss(vwtrkpara_charge, vwtrkpara_photon, vwtrkpara_pi, n_pi-1, birth);
            double sidebandlow_mean = (sidebandlow_low + sidebandlow_up)/2;
            chi2_kf_STDDmiss_low = fitKM_STDDmiss_low(vwtrkpara_charge, vwtrkpara_photon, vwtrkpara_pi, n_pi-1, birth , sidebandlow_mean);
            double sidebandup_mean = (sidebandup_low + sidebandup_up)/2;
            chi2_kf_STDDmiss_up = fitKM_STDDmiss_up(vwtrkpara_charge, vwtrkpara_photon, vwtrkpara_pi, n_pi-1, birth, sidebandup_mean);
        }
        if (m_debug) std::cout << "Start recording region info if passed the requirement" << std::endl;
        if (fabs(chi2_kf_STDDmiss) < 9999.) {
            SmartRefVector<EvtRecTrack> Dtrks = (*dtag_iter)->tracks();
            for (int j = 0; j < n_trkD; j++) {
                RecMdcKalTrack* KalTrk = Dtrks[j]->mdcKalTrack();
                if (dtagTool.isKaon(Dtrks[j])) {
                    KalTrk->setPidType(RecMdcKalTrack::kaon);
                    for (int k = 0; k < 4; k++) m_rawp4_Dtrk_STDDmiss[j][k] = KalTrk->p4(mass[3])[k];
                    m_rawp4_Dtrk_STDDmiss[j][4] = KalTrk->charge();
                    m_rawp4_Dtrk_STDDmiss[j][5] = 3;
                } else {
                    KalTrk->setPidType(RecMdcKalTrack::pion);
                    for (int k = 0; k < 4; k++) m_rawp4_Dtrk_STDDmiss[j][k] = KalTrk->p4(mass[2])[k];
                    m_rawp4_Dtrk_STDDmiss[j][4] = KalTrk->charge();
                    m_rawp4_Dtrk_STDDmiss[j][5] = 2;
                }
                for (int k = 0; k < 4; k++) {
                    m_p4_Dtrkold_STDDmiss[j][k] = m_p4_Dtrk[j][k];
                }
            }
            SmartRefVector<EvtRecTrack> Dshws = (*dtag_iter)->showers();
            for(int j = 0; j < Dshws.size(); j++) {
                RecEmcShower *gTrk = Dshws[j]->emcShower();
                Hep3Vector Gm_Vec(gTrk->x(), gTrk->y(), gTrk->z());
                Hep3Vector Gm_Mom = Gm_Vec - birth.vx();
                Gm_Mom.setMag(gTrk->energy());
                HepLorentzVector Gm_p4(Gm_Mom, gTrk->energy());
                for (int k = 0; k < 4; k++) m_rawp4_Dshw_STDDmiss[j][k] = Gm_p4[k];
                for (int k = 0; k < 4; k++) {
                    m_p4_Dshwold_STDDmiss[j][k] = m_p4_Dshw[j][k];
                }
            }
            // to find the good pions and kaons
            m_n_othertrks_STDDmiss = 0;
            for (int j = 0; j < othertracks.size(); j++) {
                if (j != i) {
                    if (!(dtagTool.isGoodTrack(othertracks[j]))) continue;
                    Ncut8++;
                    if (dtagTool.isPion(othertracks[j])) {
                        RecMdcTrack *mdcTrk = othertracks[j]->mdcTrack();
                        RecMdcKalTrack *mdcKalTrk = othertracks[j]->mdcKalTrack();
                        mdcKalTrk->setPidType(RecMdcKalTrack::pion);
                        for (int k = 0; k < 4; k++) {
                            m_rawp4_otherMdctrk_STDDmiss[m_n_othertrks_STDDmiss][k] = mdcTrk->p4(mass[2])[k];
                            m_rawp4_otherMdcKaltrk_STDDmiss[m_n_othertrks_STDDmiss][k] = mdcKalTrk->p4(mass[2])[k];
                        }
                        m_rawp4_otherMdctrk_STDDmiss[m_n_othertrks_STDDmiss][4] = mdcTrk->chi2();
                        m_rawp4_otherMdctrk_STDDmiss[m_n_othertrks_STDDmiss][5] = mdcTrk->stat(); // stat: status
                        m_rawp4_otherMdcKaltrk_STDDmiss[m_n_othertrks_STDDmiss][4] = mdcKalTrk->charge();
                        m_rawp4_otherMdcKaltrk_STDDmiss[m_n_othertrks_STDDmiss][5] = 2;
                    }
                    if (dtagTool.isKaon(othertracks[j])) {
                        RecMdcTrack *mdcTrk = othertracks[j]->mdcTrack();
                        RecMdcKalTrack *mdcKalTrk = othertracks[j]->mdcKalTrack();
                        mdcKalTrk->setPidType(RecMdcKalTrack::kaon);
                        for (int k = 0; k < 4; k++) {
                            m_rawp4_otherMdctrk_STDDmiss[m_n_othertrks_STDDmiss][k] = mdcTrk->p4(mass[3])[k];
                            m_rawp4_otherMdcKaltrk_STDDmiss[m_n_othertrks_STDDmiss][k] = mdcKalTrk->p4(mass[3])[k];
                        }
                        m_rawp4_otherMdctrk_STDDmiss[m_n_othertrks_STDDmiss][4] = mdcTrk->chi2();
                        m_rawp4_otherMdctrk_STDDmiss[m_n_othertrks_STDDmiss][5] = mdcTrk->stat(); // stat: status
                        m_rawp4_otherMdcKaltrk_STDDmiss[m_n_othertrks_STDDmiss][4] = mdcKalTrk->charge();
                        m_rawp4_otherMdcKaltrk_STDDmiss[m_n_othertrks_STDDmiss][5] = 3;
                    }
                    m_n_othertrks_STDDmiss++;
                    if (m_n_othertrks_STDDmiss >= 20) continue;
                    Ncut9++;
                }
            }
            RecMdcKalTrack *Pi = othertracks[i]->mdcKalTrack();
            for (int j = 0; j < 4; j++) {
                m_rawp4_tagPi_STDDmiss[j] = Pi->p4(mass[2])[j];
            }
            SmartRefVector<EvtRecTrack> othershowers = (*dtag_iter)->otherShowers();
            // to find the good photons in the othershowers list
            m_n_othershws_STDDmiss = 0;
            for (int j = 0; j < othershowers.size(); j++) {
                if (!(dtagTool.isGoodShower(othershowers[j]))) continue;
                Ncut10++;
                RecEmcShower *gTrk = othershowers[j]->emcShower();
                Hep3Vector Gm_Vec(gTrk->x(), gTrk->y(), gTrk->z());
                Hep3Vector Gm_Mom = Gm_Vec - birth.vx();
                Gm_Mom.setMag(gTrk->energy());
                HepLorentzVector Gm_p4(Gm_Mom, gTrk->energy());
                for (int k = 0; k < 4; k++) m_rawp4_othershw_STDDmiss[m_n_othershws_STDDmiss][k] = Gm_p4[k];
                m_n_othershws_STDDmiss++;
                if (m_n_othershws_STDDmiss >= 50) continue;
                Ncut11++;
            }
            recordVariables_STDDmiss();
        }
    }
    if (m_debug) std::cout << " recorded " << m_n_othertrks << " other charged good tracks " << std::endl;
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

bool DDecay::judge_continue(int mode, int charm, double charge_par, double type_par) {
    bool status = false;
    if (mode == 200) {
        if (charm == 1 && type_par == 2 && charge_par == -1) status = true;
        if (charm == -1 && type_par == 2 && charge_par == 1) status = true;
    }
    if (mode == 0 or mode == 1 or mode == 3) {
        if (charm == 1 && type_par == 2 && charge_par == 1) status = true;
        if (charm == -1 && type_par == 2 && charge_par == -1) status = true;
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

    // save DTag inDststfo
    m_n_trkD = n_trkD;
    m_n_shwD = n_shwD;
    m_mode = MODE;
    m_charm = charm;
    m_chi2_vf = chi2_vf;
    m_chi2_kf = chi2_kf;
    m_n_count = n_count;

    m_tuple1->write();
    Ncut12++;

    if (m_debug) std::cout << " entry in ntuple is filled for " << mode << std::endl;
}

void DDecay::recordVariables_STDDmiss() {
    m_runNo_STDDmiss = runNo;
    m_evtNo_STDDmiss = evtNo;
    m_flag1_STDDmiss = flag1;

    // save DTag info
    m_n_trkD_STDDmiss = n_trkD;
    m_n_shwD_STDDmiss = n_shwD;
    m_mode_STDDmiss = MODE;
    m_charm_STDDmiss = charm;
    m_chi2_vf_STDDmiss = chi2_vf;
    m_chi2_kf_STDDmiss = chi2_kf_STDDmiss;
    m_chi2_kf_STDDmiss_low = chi2_kf_STDDmiss_low;
    m_chi2_kf_STDDmiss_up = chi2_kf_STDDmiss_up;

    // save all McTruth info for fitKM_STDDmiss
    if (m_runNo_STDDmiss < 0 && m_isMonteCarlo) {
        m_idxmc_STDDmiss = idxmc;
        for (int i = 0; i< m_idxmc_STDDmiss; i++) {
            m_motheridx_STDDmiss[i] = motheridx[i];
            m_pdgid_STDDmiss[i] = pdgid[i];
        }
    }

    m_tuple2->write();
    Ncut13++;

    if (m_debug) std::cout << " Signal region: entry in ntuple is filled for " << mode << std::endl;
}
