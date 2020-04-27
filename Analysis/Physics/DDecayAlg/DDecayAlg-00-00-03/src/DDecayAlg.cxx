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

DECLARE_ALGORITHM_FACTORY( DDecayAlg )
DECLARE_FACTORY_ENTRIES( DDecayAlg ) {
    DECLARE_ALGORITHM( DDecayAlg );
}

LOAD_FACTORY_ENTRIES( DDecayAlg )

DDecayAlg::DDecayAlg(const std::string& name, ISvcLocator* pSvcLocator) :
	Algorithm(name, pSvcLocator) {
        m_DModes.push_back(0);
        m_DModes.push_back(1);
        m_DModes.push_back(3);
        m_DModes.push_back(200);
        declareProperty("DMode", m_DModes);
        declareProperty("IsMonteCarlo", m_isMonteCarlo = true);
        declareProperty("UsePID", m_pid = true);
        declareProperty("Debug", m_debug = false);
}

StatusCode DDecayAlg::initialize() {
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
            status = m_tuple1->addItem("charge_otherMdctrk", m_charge_otherMdctrk, 0, 10);
            status = m_tuple1->addItem("n_othershws", m_n_othershws, 0, 50);
            status = m_tuple1->addIndexedItem("rawp4_othershw", m_n_othershws, 4, m_rawp4_othershw);
            status = m_tuple1->addItem("indexmc", m_idxmc, 0, 100);
            status = m_tuple1->addIndexedItem("pdgid", m_idxmc, m_pdgid);
            status = m_tuple1->addIndexedItem("motheridx", m_idxmc, m_motheridx);
            status = m_tuple1->addItem("DpId", m_Id_Dp);
            status = m_tuple1->addItem("DmId", m_Id_Dm);
        }
        else {
            log << MSG::ERROR << "Cannot book N-tuple:" << long(m_tuple1) << endmsg;
            return StatusCode::FAILURE;
        }
    }

    log << MSG::INFO << "successfully return from initialize()" << endmsg;
    return StatusCode::SUCCESS;
}

StatusCode DDecayAlg::execute() {
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

StatusCode DDecayAlg::finalize() {
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

void DDecayAlg::clearVariables() {
    // common info
    runNo = 0;
    evtNo = 0;
    flag1 = 0;
    m_runNo = 0;
    m_evtNo = 0;
    m_flag1 = 0;

    // all McTruth info
    m_idxmc = 0;

    // DpDm McTruth info
    DpId = -999;
    DmId = -999;

    // single D tag
    m_n_trkD = 0;
    n_trkD = 0;
    m_n_shwD = 0;
    n_shwD = 0;
    MODE = -999;
    mode = -999;
    m_mode = -999;
    charm = -999;
    m_charm = -999;
    chi2_vf = 999;
    m_chi2_vf = 999;
    chi2_kf = 999;
    m_chi2_kf = 999;
    mDcand = 0;
    charge_otherMdctrk = 0;
    n_count = 0;
    m_n_count = 0;

    // judgement variables
    stat_DTagTool = false;
    stat_tagSD = false;
    stat_saveCandD = false;
    stat_saveOthertrks = false;
    stat_saveOthershws = false;
}

void DDecayAlg::saveAllMcTruthInfo() {
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

void DDecayAlg::saveDpDmMcTruthInfo() {
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

bool DDecayAlg::useDTagTool() {
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

bool DDecayAlg::tagSingleD() {
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

bool DDecayAlg::saveCandD(VWTrkPara &vwtrkpara_charge, VWTrkPara &vwtrkpara_photon) {
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

double DDecayAlg::fitVertex(VWTrkPara &vwtrkpara, VertexParameter &birth) {
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

double DDecayAlg::fitKM(VWTrkPara &vwtrkpara_charge, VWTrkPara &vwtrkpara_photon, VertexParameter &birth) {
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

bool DDecayAlg::saveOthertrks(VWTrkPara &vwtrkpara_charge, VWTrkPara &vwtrkpara_photon, VertexParameter &birth) {
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
            charge_otherMdctrk = mdcTrk->charge();
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

    if (m_debug) std::cout << " recorded " << m_n_othertrks << " other charged good tracks " << std::endl;
    if (m_n_othertrks >= 20) return false;
    else return true;
}

bool DDecayAlg::saveOthershws() {
    HepLorentzVector pD;
    pD.setPx(0.);
    pD.setPy(0.);
    pD.setPz(0.);
    pD.setE(0.);
    for (int k = 0; k < n_trkD; k++) {
        HepLorentzVector ptrack;
        ptrack.setPx(m_p4_Dtrk[k][0]);
        ptrack.setPy(m_p4_Dtrk[k][1]);
        ptrack.setPz(m_p4_Dtrk[k][2]);
        ptrack.setE(m_p4_Dtrk[k][3]);
        pD += ptrack;
    }
    for(int k = 0; k < n_shwD; k++) {
        HepLorentzVector pshower;
        pshower.setPx(m_p4_Dshw[k][0]);
        pshower.setPy(m_p4_Dshw[k][1]);
        pshower.setPz(m_p4_Dshw[k][2]);
        pshower.setE(m_p4_Dshw[k][3]);
        pD += pshower;
    }
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

double DDecayAlg::ECMS(int runNo) {
    if (runNo >= 33659 && runNo <= 33719) return 4.08545;
    else if (runNo >= 47543 && runNo <= 48170) return 4.1888;
    else if (runNo >= 30372 && runNo <= 30437) return 4.1886;
    else if (runNo >= 48172 && runNo <= 48713) return 4.1989;
    else if (runNo >= 48714 && runNo <= 49239) return 4.2092;
    else if (runNo >= 31983 && runNo <= 32045) return 4.2077;
    else if (runNo >= 49270 && runNo <= 49787) return 4.2187;
    else if (runNo >= 32046 && runNo <= 32140) return 4.2171;
    else if (runNo >= 30438 && runNo <= 30491) return 4.2263;
    else if (runNo >= 32239 && runNo <= 33484) return 4.2263;
    else if (runNo >= 49788 && runNo <= 50254) return 4.2357;
    else if (runNo >= 32141 && runNo <= 32226) return 4.2417;
    else if (runNo >= 50255 && runNo <= 50793) return 4.2438;
    else if (runNo >= 29677 && runNo <= 30367) return 4.25797;
    else if (runNo >= 31561 && runNo <= 31981) return 4.25797;
    else if (runNo >= 50796 && runNo <= 51302) return 4.2668; 
    else if (runNo >= 51303 && runNo <= 51498) return 4.2777;
    else if (runNo >= 30492 && runNo <= 30557) return 4.3079;
    else if (runNo >= 30616 && runNo <= 31279) return 4.3583;
    else if (runNo >= 31281 && runNo <= 31325) return 4.3874;
    else if (runNo >= 36245 && runNo <= 36393) return 4.4671;
    else if (runNo >= 36398 && runNo <= 36588) return 4.5271;
    else if (runNo >= 36603 && runNo <= 36699) return 4.5745;
    else if (runNo >= 59163 && runNo <= 59573) return 4.12848;
    else if (runNo >= 59574 && runNo <= 59896) return 4.15744;
    else if (runNo >= 59902 && runNo <= 60363) return 4.28788;
    else if (runNo >= 60364 && runNo <= 60805) return 4.31205;
    else if (runNo >= 60808 && runNo <= 61242) return 4.33739;
    else if (runNo >= 61249 && runNo <= 61762) return 4.37737;
    else if (runNo >= 61763 && runNo <= 62285) return 4.39645;
    else if (runNo >= 62286 && runNo <= 62823) return 4.43624;
    else if (runNo >= 34272 && runNo <= 34276) return 4.0200;
    else if (runNo >= 34277 && runNo <= 34281) return 4.0250;
    else if (runNo >= 34282 && runNo <= 34297) return 4.0300;
    else if (runNo >= 34314 && runNo <= 34320) return 4.0350;
    else if (runNo >= 34321 && runNo <= 34327) return 4.0400;
    else if (runNo >= 34328 && runNo <= 34335) return 4.0500;
    else if (runNo >= 34339 && runNo <= 34345) return 4.0550;
    else if (runNo >= 34346 && runNo <= 34350) return 4.0600;
    else if (runNo >= 34351 && runNo <= 34358) return 4.0650;
    else if (runNo >= 34359 && runNo <= 34367) return 4.0700;
    else if (runNo >= 34368 && runNo <= 34373) return 4.0800;
    else if (runNo >= 34373 && runNo <= 34381) return 4.0900;
    else if (runNo >= 34382 && runNo <= 34389) return 4.1000;
    else if (runNo >= 34390 && runNo <= 34396) return 4.1100;
    else if (runNo >= 34397 && runNo <= 34403) return 4.1200;
    else if (runNo >= 34404 && runNo <= 34411) return 4.1300;
    else if (runNo >= 34412 && runNo <= 34417) return 4.1400;
    else if (runNo >= 34418 && runNo <= 34427) return 4.1450;
    else if (runNo >= 34428 && runNo <= 34436) return 4.1500;
    else if (runNo >= 34437 && runNo <= 34446) return 4.1600;
    else if (runNo >= 34447 && runNo <= 34460) return 4.1700;
    else if (runNo >= 34478 && runNo <= 34485) return 4.1800;
    else if (runNo >= 34486 && runNo <= 34493) return 4.1900;
    else if (runNo >= 34494 && runNo <= 34502) return 4.1950;
    else if (runNo >= 34503 && runNo <= 34511) return 4.2000;
    else if (runNo >= 34512 && runNo <= 34526) return 4.2030;
    else if (runNo >= 34530 && runNo <= 34540) return 4.2060;
    else if (runNo >= 34541 && runNo <= 34554) return 4.2100;
    else if (runNo >= 34555 && runNo <= 34563) return 4.2150;
    else if (runNo >= 34564 && runNo <= 34573) return 4.2200;
    else if (runNo >= 34574 && runNo <= 34584) return 4.2250;
    else if (runNo >= 34585 && runNo <= 34592) return 4.2300;
    else if (runNo >= 34593 && runNo <= 34601) return 4.2350;
    else if (runNo >= 34603 && runNo <= 34612) return 4.2400;
    else if (runNo >= 34613 && runNo <= 34622) return 4.2430;
    else if (runNo >= 34623 && runNo <= 34633) return 4.2450;
    else if (runNo >= 34634 && runNo <= 34641) return 4.2480;
    else if (runNo >= 34642 && runNo <= 34651) return 4.2500;
    else if (runNo >= 34652 && runNo <= 34660) return 4.2550;
    else if (runNo >= 34661 && runNo <= 34673) return 4.2600;
    else if (runNo >= 34674 && runNo <= 34684) return 4.2650;
    else if (runNo >= 34685 && runNo <= 34694) return 4.2700;
    else if (runNo >= 34695 && runNo <= 34704) return 4.2750;
    else if (runNo >= 34705 && runNo <= 34718) return 4.2800;
    else if (runNo >= 34719 && runNo <= 34728) return 4.2850;
    else if (runNo >= 34729 && runNo <= 34739) return 4.2900;
    else if (runNo >= 34740 && runNo <= 34753) return 4.3000;
    else if (runNo >= 34754 && runNo <= 34762) return 4.3100;
    else if (runNo >= 34763 && runNo <= 34776) return 4.3200;
    else if (runNo >= 34777 && runNo <= 34784) return 4.3300;
    else if (runNo >= 34785 && runNo <= 34793) return 4.3400;
    else if (runNo >= 34794 && runNo <= 34803) return 4.3500;
    else if (runNo >= 34804 && runNo <= 34811) return 4.3600;
    else if (runNo >= 34812 && runNo <= 34824) return 4.3700;
    else if (runNo >= 34825 && runNo <= 34836) return 4.3800;
    else if (runNo >= 34837 && runNo <= 34847) return 4.3900;
    else if (runNo >= 34848 && runNo <= 34860) return 4.3950;
    else if (runNo >= 34861 && runNo <= 34868) return 4.4000;
    else if (runNo >= 34869 && runNo <= 34881) return 4.4100;
    else if (runNo >= 34883 && runNo <= 34890) return 4.4200;
    else if (runNo >= 34891 && runNo <= 34899) return 4.4250;
    else if (runNo >= 34901 && runNo <= 34912) return 4.4300;
    else if (runNo >= 34913 && runNo <= 34925) return 4.4400;
    else if (runNo >= 34926 && runNo <= 34935) return 4.4500;
    else if (runNo >= 34937 && runNo <= 34946) return 4.4600;
    else if (runNo >= 34947 && runNo <= 34957) return 4.4800;
    else if (runNo >= 34958 && runNo <= 34967) return 4.5000;
    else if (runNo >= 34968 && runNo <= 34981) return 4.5200;
    else if (runNo >= 34982 && runNo <= 35009) return 4.5400;
    else if (runNo >= 35010 && runNo <= 35026) return 4.5500;
    else if (runNo >= 35027 && runNo <= 35040) return 4.5600;
    else if (runNo >= 35041 && runNo <= 35059) return 4.5700;
    else if (runNo >= 35060 && runNo <= 35081) return 4.5800;
    else if (runNo >= 35099 && runNo <= 35116) return 4.5900;
    else if (runNo >= 63075 && runNo <= 63515) return 4.6260;
    else if (runNo >= 63516 && runNo <= 63715) return 4.6400;
    else if (runNo >= 63718 && runNo <= 63852) return 4.6600;
    else if (runNo >= 63867 && runNo <= 64015) return 4.6800;
    else if (runNo >= 64028 && runNo <= 64289) return 4.7000;
    // else if (runNo >= 35227 && runNo <= 35473) return 4.6260;
    // else if (runNo >= 35474 && runNo <= 35720) return 4.6400;
    // else if (runNo >= 35721 && runNo <= 35967) return 4.6600;
    // else if (runNo >= 35968 && runNo <= 36113) return 4.6800;
    // else if (runNo >= 36114 && runNo <= 36213) return 4.7000;
    else return 999.;
}

void DDecayAlg::recordVariables() {
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
