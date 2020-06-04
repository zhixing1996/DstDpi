#!/usr/bin/env python
"""
Get useful info from raw root files
"""

__author__ = "Maoqiang JING <jingmq@ihep.ac.cn>"
__copyright__ = "Copyright (c) Maoqiang JING"
__created__ = "[2019-09-03 Tue 05:41]"

from math import *
from array import array
import ROOT
from ROOT import TCanvas, gStyle, TLorentzVector, TTree
from ROOT import TFile, TH1F, TLegend, TArrow, TChain, TVector3
import sys, os
import logging
logging.basicConfig(level=logging.DEBUG, format=' %(asctime)s - %(levelname)s- %(message)s')

def usage():
    sys.stdout.write('''
NAME
    get_info.py

SYNOPSIS
    ./get_info.py [file_in] [file_out] [ecms] [D_sample] [sample_type]

AUTHOR
    Maoqiang JING <jingmq@ihep.ac.cn>

DATE
    August 2019
\n''')

def save_raw(f_in, cms, t, D_sample):
    m_runNo = array('i', [0])
    m_evtNo = array('i', [0])
    m_mode = array('i', [0])
    m_charm = array('i', [0])
    m_charge_pi = array('d', [999.])
    m_rawm_D = array('d', [999.])
    m_m_D = array('d', [999.])
    m_p_D = array('d', [999.])
    m_E_D = array('d', [999.])
    m_rm_D = array('d', [999.])
    m_rm2_D = array('d', [999.])
    m_m_Dpi = array('d', [999.])
    m_m2_Dpi = array('d', [999.])
    m_rm_Dpi = array('d', [999.])
    m_rm_pi = array('d', [999.])
    m_chi2_vf = array('d', [999.])
    m_chi2_kf = array('d', [999.])
    m_n_othershws = array('i', [0])
    m_n_othertrks = array('i', [0])
    m_indexmc = array('i', [0])
    m_motheridx = array('i', 100*[0])
    m_pdgid = array('i', 100*[0])
    t.Branch('runNo', m_runNo, 'm_runNo/I')
    t.Branch('evtNo', m_evtNo, 'm_evtNo/I')
    t.Branch('mode', m_mode, 'm_mode/I')
    t.Branch('charm', m_charm, 'm_charm/I')
    t.Branch('charge_pi', m_charge_pi, 'm_charge_pi/D')
    t.Branch('rawm_D', m_rawm_D, 'm_rawm_D/D')
    t.Branch('m_D', m_m_D, 'm_m_D/D')
    t.Branch('p_D', m_p_D, 'm_p_D/D')
    t.Branch('E_D', m_E_D, 'm_E_D/D')
    t.Branch('rm_D', m_rm_D, 'm_rm_D/D')
    t.Branch('rm2_D', m_rm2_D, 'm_rm2_D/D')
    t.Branch('m_Dpi', m_m_Dpi, 'm_m_Dpi/D')
    t.Branch('m2_Dpi', m_m2_Dpi, 'm_m2_Dpi/D')
    t.Branch('rm_Dpi', m_rm_Dpi, 'm_rm_Dpi/D')
    t.Branch('rm_pi', m_rm_pi, 'm_rm_pi/D')
    t.Branch('chi2_vf', m_chi2_vf, 'm_chi2_vf/D')
    t.Branch('chi2_kf', m_chi2_kf, 'm_chi2_kf/D')
    t.Branch('n_othertrks', m_n_othertrks, 'm_n_othertrks/I')
    t.Branch('n_othershws', m_n_othershws, 'm_n_othershws/I')
    t.Branch('indexmc', m_indexmc, 'indexmc/I')
    t.Branch('motheridx', m_motheridx, 'motheridx[100]/I')
    t.Branch('pdgid', m_pdgid, 'pdgid[100]/I')
    t_std = f_in.Get('STD')
    nentries = t_std.GetEntries()
    for ientry in range(nentries):
        t_std.GetEntry(ientry)
        if t_std.mode != 200 and t_std.mode != 0 and t_std.mode != 1 and t_std.mode != 3:
            continue
        pD_raw = TLorentzVector(0, 0, 0, 0)
        pD = TLorentzVector(0, 0, 0, 0)
        for iTrk in range(t_std.n_trkD):
            ptrack_raw = TLorentzVector(0, 0, 0, 0)
            ptrack = TLorentzVector(0, 0, 0, 0)
            ptrack_raw.SetPxPyPzE(t_std.rawp4_Dtrk[iTrk*6+0], t_std.rawp4_Dtrk[iTrk*6+1], t_std.rawp4_Dtrk[iTrk*6+2], t_std.rawp4_Dtrk[iTrk*6+3])
            ptrack.SetPxPyPzE(t_std.p4_Dtrk[iTrk*4+0], t_std.p4_Dtrk[iTrk*4+1], t_std.p4_Dtrk[iTrk*4+2], t_std.p4_Dtrk[iTrk*4+3])
            pD_raw += ptrack_raw
            pD += ptrack
        for iShw in range(t_std.n_shwD):
            pshower_raw = TLorentzVector(0, 0, 0, 0)
            pshower = TLorentzVector(0, 0, 0, 0)
            pshower_raw.SetPxPyPzE(t_std.rawp4_Dshw[iShw*4+0], t_std.rawp4_Dshw[iShw*4+1], t_std.rawp4_Dshw[iShw*4+2], t_std.rawp4_Dshw[iShw*4+3])
            pshower.SetPxPyPzE(t_std.p4_Dshw[iShw*4+0], t_std.p4_Dshw[iShw*4+1], t_std.p4_Dshw[iShw*4+2], t_std.p4_Dshw[iShw*4+3])
            pD_raw += pshower_raw
            pD += pshower
        pPi = TLorentzVector(0,0,0,0)
        for trk in range(t_std.n_othertrks):
            if abs(t_std.rawp4_otherMdcKaltrk[trk*6+4]) != 1:
                continue
            if t_std.rawp4_otherMdcKaltrk[trk*6+5] != 2:
                continue
            pPi.SetPxPyPzE(t_std.rawp4_otherMdcKaltrk[trk*6+0], t_std.rawp4_otherMdcKaltrk[trk*6+1], t_std.rawp4_otherMdcKaltrk[trk*6+2], t_std.rawp4_otherMdcKaltrk[trk*6+3])
            if D_sample == 'Dplus' and t_std.mode == 200:
                if ((t_std.charm == 1 and t_std.rawp4_otherMdcKaltrk[trk*6+4] == -1) or (t_std.charm == -1 and t_std.rawp4_otherMdcKaltrk[trk*6+4] == 1)):
                    m_runNo[0] = t_std.runNo
                    m_evtNo[0] = t_std.evtNo
                    m_mode[0] = t_std.mode
                    m_charm[0] = t_std.charm
                    m_charge_pi[0] = t_std.rawp4_otherMdcKaltrk[trk*6+4]
                    m_rawm_D[0] = pD_raw.M()
                    m_m_D[0] = pD.M()
                    m_p_D[0] = pD.P()
                    m_E_D[0] = pD.E()
                    m_rm_D[0] = (cms-pD).M()
                    m_rm2_D[0] = (cms-pD).M2()
                    m_m_Dpi[0] = (pD+pPi).M()
                    m_m2_Dpi[0] = (pD+pPi).M2()
                    m_rm_Dpi[0] = (cms-pD-pPi).M()
                    m_rm_pi[0] = (cms-pPi).M()
                    m_chi2_vf[0] = t_std.chi2_vf
                    m_chi2_kf[0] = t_std.chi2_kf
                    m_n_othershws[0] = t_std.n_othershws
                    m_n_othertrks[0] = t_std.n_othertrks
                    m_indexmc[0] = t_std.indexmc
                    for i in range(t_std.indexmc):
                        m_motheridx[i] = t_std.motheridx[i]
                        m_pdgid[i] = t_std.pdgid[i]
                    t.Fill()
            if D_sample == 'D0' and (t_std.mode == 0 or t_std.mode == 1 or t_std.mode == 3):
                if (t_std.charm == 1 and t_std.rawp4_otherMdcKaltrk[trk*6+4] == 1) or (t_std.charm == -1 and t_std.rawp4_otherMdcKaltrk[trk*6+4] == -1):
                    m_runNo[0] = t_std.runNo
                    m_evtNo[0] = t_std.evtNo
                    m_mode[0] = t_std.mode
                    m_charm[0] = t_std.charm
                    m_charge_pi[0] = t_std.rawp4_otherMdcKaltrk[trk*6+4]
                    m_rawm_D[0] = pD_raw.M()
                    m_m_D[0] = pD.M()
                    m_p_D[0] = pD.P()
                    m_E_D[0] = pD.E()
                    m_rm_D[0] = (cms-pD).M()
                    m_rm2_D[0] = (cms-pD).M2()
                    m_m_Dpi[0] = (pD+pPi).M()
                    m_m2_Dpi[0] = (pD+pPi).M2()
                    m_rm_Dpi[0] = (cms-pD-pPi).M()
                    m_rm_pi[0] = (cms-pPi).M()
                    m_chi2_vf[0] = t_std.chi2_vf
                    m_chi2_kf[0] = t_std.chi2_kf
                    m_n_othershws[0] = t_std.n_othershws
                    m_n_othertrks[0] = t_std.n_othertrks
                    m_indexmc[0] = t_std.indexmc
                    for i in range(t_std.indexmc):
                        m_motheridx[i] = t_std.motheridx[i]
                        m_pdgid[i] = t_std.pdgid[i]
                    t.Fill()

def save_missing(f_in, cms, t, D_sample, D_type, sample_type):
    m_runNo = array('i', [0])
    m_evtNo = array('i', [0])
    m_mode = array('i', [0])
    m_charm = array('i', [0])
    m_rawm_D = array('d', [999.])
    m_m_D = array('d', [999.])
    m_p_D = array('d', [999.])
    m_E_D = array('d', [999.])
    m_rm_D = array('d', [999.])
    m_rm_Dmiss = array('d', [999.])
    m_rm2_D = array('d', [999.])
    m_m_Dpi = array('d', [999.])
    m_m_Dmisspi = array('d', [999.])
    m_m2_Dpi = array('d', [999.])
    m_rm_Dpi = array('d', [999.])
    m_rm_Dmisspi = array('d', [999.])
    m_rm_pi = array('d', [999.])
    m_chi2_vf = array('d', [999.])
    m_chi2_kf = array('d', [999.])
    m_n_othershws = array('i', [0])
    m_n_othertrks = array('i', [0])
    m_indexmc = array('i', [0])
    m_motheridx = array('i', 100*[0])
    m_pdgid = array('i', 100*[0])
    if D_sample == 'Dplus' and D_type == 'D' and sample_type == 'signal':
        m_p_pi = array('d', [999.])
        m_charge_pi = array('d', [999.])
        m_costheta_pi = array('d', [999.])
        m_phi_pi = array('d', [999.])
        m_chi_e_pi = array('d', [999.])
        m_chi_mu_pi = array('d', [999.])
        m_chi_pi_pi = array('d', [999.])
        m_chi_K_pi = array('d', [999.])
        m_chi_p_pi = array('d', [999.])
        m_prob_e_pi = array('d', [999.])
        m_prob_mu_pi = array('d', [999.])
        m_prob_pi_pi = array('d', [999.])
        m_prob_K_pi = array('d', [999.])
        m_prob_p_pi = array('d', [999.])
        m_p_DK = array('d', [999.])
        m_charge_DK = array('d', [999.])
        m_costheta_DK = array('d', [999.])
        m_phi_DK = array('d', [999.])
        m_chi_e_DK = array('d', [999.])
        m_chi_mu_DK = array('d', [999.])
        m_chi_pi_DK = array('d', [999.])
        m_chi_K_DK = array('d', [999.])
        m_chi_p_DK = array('d', [999.])
        m_prob_e_DK = array('d', [999.])
        m_prob_mu_DK = array('d', [999.])
        m_prob_pi_DK = array('d', [999.])
        m_prob_K_DK = array('d', [999.])
        m_prob_p_DK = array('d', [999.])
        m_p_Dpi1 = array('d', [999.])
        m_charge_Dpi1 = array('d', [999.])
        m_costheta_Dpi1 = array('d', [999.])
        m_phi_Dpi1 = array('d', [999.])
        m_chi_e_Dpi1 = array('d', [999.])
        m_chi_mu_Dpi1 = array('d', [999.])
        m_chi_pi_Dpi1 = array('d', [999.])
        m_chi_K_Dpi1 = array('d', [999.])
        m_chi_p_Dpi1 = array('d', [999.])
        m_prob_e_Dpi1 = array('d', [999.])
        m_prob_mu_Dpi1 = array('d', [999.])
        m_prob_pi_Dpi1 = array('d', [999.])
        m_prob_K_Dpi1 = array('d', [999.])
        m_prob_p_Dpi1 = array('d', [999.])
        m_p_Dpi2 = array('d', [999.])
        m_charge_Dpi2 = array('d', [999.])
        m_costheta_Dpi2 = array('d', [999.])
        m_phi_Dpi2 = array('d', [999.])
        m_chi_e_Dpi2 = array('d', [999.])
        m_chi_mu_Dpi2 = array('d', [999.])
        m_chi_pi_Dpi2 = array('d', [999.])
        m_chi_K_Dpi2 = array('d', [999.])
        m_chi_p_Dpi2 = array('d', [999.])
        m_prob_e_Dpi2 = array('d', [999.])
        m_prob_mu_Dpi2 = array('d', [999.])
        m_prob_pi_Dpi2 = array('d', [999.])
        m_prob_K_Dpi2 = array('d', [999.])
        m_prob_p_Dpi2 = array('d', [999.])
    t.Branch('runNo', m_runNo, 'm_runNo/I')
    t.Branch('evtNo', m_evtNo, 'm_evtNo/I')
    t.Branch('mode', m_mode, 'm_mode/I')
    t.Branch('charm', m_charm, 'm_charm/I')
    t.Branch('rawm_D', m_rawm_D, 'm_rawm_D/D')
    t.Branch('m_D', m_m_D, 'm_m_D/D')
    t.Branch('p_D', m_p_D, 'm_p_D/D')
    t.Branch('E_D', m_E_D, 'm_E_D/D')
    t.Branch('rm_D', m_rm_D, 'm_rm_D/D')
    t.Branch('rm_Dmiss', m_rm_Dmiss, 'm_rm_Dmiss/D')
    t.Branch('rm2_D', m_rm2_D, 'm_rm2_D/D')
    t.Branch('m_Dpi', m_m_Dpi, 'm_m_Dpi/D')
    t.Branch('m_Dmisspi', m_m_Dmisspi, 'm_m_Dmisspi/D')
    t.Branch('m2_Dpi', m_m2_Dpi, 'm_m2_Dpi/D')
    t.Branch('rm_Dpi', m_rm_Dpi, 'm_rm_Dpi/D')
    t.Branch('rm_Dmisspi', m_rm_Dmisspi, 'm_rm_Dmisspi/D')
    t.Branch('rm_pi', m_rm_pi, 'm_rm_pi/D')
    t.Branch('chi2_vf', m_chi2_vf, 'm_chi2_vf/D')
    t.Branch('chi2_kf', m_chi2_kf, 'm_chi2_kf/D')
    t.Branch('n_othertrks', m_n_othertrks, 'm_n_othertrks/I')
    t.Branch('n_othershws', m_n_othershws, 'm_n_othershws/I')
    t.Branch('indexmc', m_indexmc, 'indexmc/I')
    t.Branch('motheridx', m_motheridx, 'motheridx[100]/I')
    t.Branch('pdgid', m_pdgid, 'pdgid[100]/I')
    if D_sample == 'Dplus' and D_type == 'D' and sample_type == 'signal':
        t.Branch('p_pi', m_p_pi, 'm_p_pi/D')
        t.Branch('charge_pi', m_charge_pi, 'm_charge_pi/D')
        t.Branch('costheta_pi', m_costheta_pi, 'm_costheta_pi/D')
        t.Branch('phi_pi', m_phi_pi, 'm_phi_pi/D')
        t.Branch('chi_e_pi', m_chi_e_pi, 'm_chi_e_pi/D')
        t.Branch('chi_mu_pi', m_chi_mu_pi, 'm_chi_mu_pi/D')
        t.Branch('chi_pi_pi', m_chi_pi_pi, 'm_chi_pi_pi/D')
        t.Branch('chi_K_pi', m_chi_K_pi, 'm_chi_K_pi/D')
        t.Branch('chi_p_pi', m_chi_p_pi, 'm_chi_p_pi/D')
        t.Branch('prob_e_pi', m_prob_e_pi, 'm_prob_e_pi/D')
        t.Branch('prob_mu_pi', m_prob_mu_pi, 'm_prob_mu_pi/D')
        t.Branch('prob_pi_pi', m_prob_pi_pi, 'm_prob_pi_pi/D')
        t.Branch('prob_K_pi', m_prob_K_pi, 'm_prob_K_pi/D')
        t.Branch('prob_p_pi', m_prob_p_pi, 'm_prob_p_pi/D')
        t.Branch('p_DK', m_p_DK, 'm_p_DK/D')
        t.Branch('charge_DK', m_charge_DK, 'm_charge_DK/D')
        t.Branch('costheta_DK', m_costheta_DK, 'm_costheta_DK/D')
        t.Branch('phi_DK', m_phi_DK, 'm_phi_DK/D')
        t.Branch('chi_e_DK', m_chi_e_DK, 'm_chi_e_DK/D')
        t.Branch('chi_mu_DK', m_chi_mu_DK, 'm_chi_mu_DK/D')
        t.Branch('chi_pi_DK', m_chi_pi_DK, 'm_chi_pi_DK/D')
        t.Branch('chi_K_DK', m_chi_K_DK, 'm_chi_K_DK/D')
        t.Branch('chi_p_DK', m_chi_p_DK, 'm_chi_p_DK/D')
        t.Branch('prob_e_DK', m_prob_e_DK, 'm_prob_e_DK/D')
        t.Branch('prob_mu_DK', m_prob_mu_DK, 'm_prob_mu_DK/D')
        t.Branch('prob_pi_DK', m_prob_pi_DK, 'm_prob_pi_DK/D')
        t.Branch('prob_K_DK', m_prob_K_DK, 'm_prob_K_DK/D')
        t.Branch('prob_p_DK', m_prob_p_DK, 'm_prob_p_DK/D')
        t.Branch('p_Dpi1', m_p_Dpi1, 'm_p_Dpi1/D')
        t.Branch('charge_Dpi1', m_charge_Dpi1, 'm_charge_Dpi1/D')
        t.Branch('costheta_Dpi1', m_costheta_Dpi1, 'm_costheta_Dpi1/D')
        t.Branch('phi_Dpi1', m_phi_Dpi1, 'm_phi_Dpi1/D')
        t.Branch('chi_e_Dpi1', m_chi_e_Dpi1, 'm_chi_e_Dpi1/D')
        t.Branch('chi_mu_Dpi1', m_chi_mu_Dpi1, 'm_chi_mu_Dpi1/D')
        t.Branch('chi_pi_Dpi1', m_chi_pi_Dpi1, 'm_chi_pi_Dpi1/D')
        t.Branch('chi_K_Dpi1', m_chi_K_Dpi1, 'm_chi_K_Dpi1/D')
        t.Branch('chi_p_Dpi1', m_chi_p_Dpi1, 'm_chi_p_Dpi1/D')
        t.Branch('prob_e_Dpi1', m_prob_e_Dpi1, 'm_prob_e_Dpi1/D')
        t.Branch('prob_mu_Dpi1', m_prob_mu_Dpi1, 'm_prob_mu_Dpi1/D')
        t.Branch('prob_pi_Dpi1', m_prob_pi_Dpi1, 'm_prob_pi_Dpi1/D')
        t.Branch('prob_K_Dpi1', m_prob_K_Dpi1, 'm_prob_K_Dpi1/D')
        t.Branch('prob_p_Dpi1', m_prob_p_Dpi1, 'm_prob_p_Dpi1/D')
        t.Branch('p_Dpi2', m_p_Dpi2, 'm_p_Dpi2/D')
        t.Branch('charge_Dpi2', m_charge_Dpi2, 'm_charge_Dpi2/D')
        t.Branch('costheta_Dpi2', m_costheta_Dpi2, 'm_costheta_Dpi2/D')
        t.Branch('phi_Dpi2', m_phi_Dpi2, 'm_phi_Dpi2/D')
        t.Branch('chi_e_Dpi2', m_chi_e_Dpi2, 'm_chi_e_Dpi2/D')
        t.Branch('chi_mu_Dpi2', m_chi_mu_Dpi2, 'm_chi_mu_Dpi2/D')
        t.Branch('chi_pi_Dpi2', m_chi_pi_Dpi2, 'm_chi_pi_Dpi2/D')
        t.Branch('chi_K_Dpi2', m_chi_K_Dpi2, 'm_chi_K_Dpi2/D')
        t.Branch('chi_p_Dpi2', m_chi_p_Dpi2, 'm_chi_p_Dpi2/D')
        t.Branch('prob_e_Dpi2', m_prob_e_Dpi2, 'm_prob_e_Dpi2/D')
        t.Branch('prob_mu_Dpi2', m_prob_mu_Dpi2, 'm_prob_mu_Dpi2/D')
        t.Branch('prob_pi_Dpi2', m_prob_pi_Dpi2, 'm_prob_pi_Dpi2/D')
        t.Branch('prob_K_Dpi2', m_prob_K_Dpi2, 'm_prob_K_Dpi2/D')
        t.Branch('prob_p_Dpi2', m_prob_p_Dpi2, 'm_prob_p_Dpi2/D')
    if D_type == 'D' and sample_type == 'signal':
        t_std = f_in.Get('STDDmiss_sig_D')
    elif D_type == 'Dst' and sample_type == 'signal':
        t_std = f_in.Get('STDDmiss_sig_Dst')
    elif D_type == 'D' and sample_type == 'background':
        t_std = f_in.Get('STDDmiss_bkg_D')
    elif D_type == 'Dst' and sample_type == 'background':
        t_std = f_in.Get('STDDmiss_bkg_Dst')
    nentries = t_std.GetEntries()
    for ientry in range(nentries):
        t_std.GetEntry(ientry)
        if t_std.mode != 200 and t_std.mode != 0 and t_std.mode != 1 and t_std.mode != 3:
            continue
        pD_raw = TLorentzVector(0, 0, 0, 0)
        pD = TLorentzVector(0, 0, 0, 0)
        pKpi = TLorentzVector(0, 0, 0, 0)
        tag = 0
        for iTrk in range(t_std.n_trkD):
            ptrack_raw = TLorentzVector(0, 0, 0, 0)
            ptrack = TLorentzVector(0, 0, 0, 0)
            ptrack_raw.SetPxPyPzE(t_std.rawp4_Dtrk[iTrk*6+0], t_std.rawp4_Dtrk[iTrk*6+1], t_std.rawp4_Dtrk[iTrk*6+2], t_std.rawp4_Dtrk[iTrk*6+3])
            ptrack.SetPxPyPzE(t_std.p4_Dtrk[iTrk*4+0], t_std.p4_Dtrk[iTrk*4+1], t_std.p4_Dtrk[iTrk*4+2], t_std.p4_Dtrk[iTrk*4+3])
            pD_raw += ptrack_raw
            pD += ptrack
            ptrack_Kpi = TLorentzVector(0, 0, 0, 0)
            if t_std.rawp4_Dtrk[iTrk*6+4] == 1 and t_std.rawp4_Dtrk[iTrk*6+5] == 3 and t_std.mode == 200:
                ptrack_Kpi.SetPxPyPzE(t_std.rawp4_Dtrk[iTrk*6+0], t_std.rawp4_Dtrk[iTrk*6+1], t_std.rawp4_Dtrk[iTrk*6+2], t_std.rawp4_Dtrk[iTrk*6+3])
                pKpi += ptrack_Kpi
            if t_std.rawp4_Dtrk[iTrk*6+4] == -1 and t_std.rawp4_Dtrk[iTrk*6+5] == 3 and t_std.mode == 200:
                ptrack_Kpi.SetPxPyPzE(t_std.rawp4_Dtrk[iTrk*6+0], t_std.rawp4_Dtrk[iTrk*6+1], t_std.rawp4_Dtrk[iTrk*6+2], t_std.rawp4_Dtrk[iTrk*6+3])
                pKpi += ptrack_Kpi
            if t_std.rawp4_Dtrk[iTrk*6+5] == 2 and t_std.mode == 200 and tag == 0:
                tag = 1
                ptrack_Kpi.SetPxPyPzE(t_std.rawp4_Dtrk[iTrk*6+0], t_std.rawp4_Dtrk[iTrk*6+1], t_std.rawp4_Dtrk[iTrk*6+2], t_std.rawp4_Dtrk[iTrk*6+3])
                pKpi += ptrack_Kpi
        for iShw in range(t_std.n_shwD):
            pshower_raw = TLorentzVector(0, 0, 0, 0)
            pshower = TLorentzVector(0, 0, 0, 0)
            pshower_raw.SetPxPyPzE(t_std.rawp4_Dshw[iShw*4+0], t_std.rawp4_Dshw[iShw*4+1], t_std.rawp4_Dshw[iShw*4+2], t_std.rawp4_Dshw[iShw*4+3])
            pshower.SetPxPyPzE(t_std.p4_Dshw[iShw*4+0], t_std.p4_Dshw[iShw*4+1], t_std.p4_Dshw[iShw*4+2], t_std.p4_Dshw[iShw*4+3])
            pD_raw += pshower_raw
            pD += pshower
        pPi = TLorentzVector(0,0,0,0)
        pPi.SetPxPyPzE(t_std.p4_pi[0], t_std.p4_pi[1], t_std.p4_pi[2], t_std.p4_pi[3])
        pDmiss = TLorentzVector(0,0,0,0)
        pDmiss.SetPxPyPzE(t_std.p4_Dmiss[0], t_std.p4_Dmiss[1], t_std.p4_Dmiss[2], t_std.p4_Dmiss[3])
        if D_sample == 'Dplus' and t_std.mode == 200:
            m_runNo[0] = t_std.runNo
            m_evtNo[0] = t_std.evtNo
            m_mode[0] = t_std.mode
            m_charm[0] = t_std.charm
            m_rawm_D[0] = pD_raw.M()
            m_m_D[0] = pD.M()
            m_p_D[0] = pD.P()
            m_E_D[0] = pD.E()
            m_rm_D[0] = (cms-pD).M()
            m_rm_Dmiss[0] = (cms-pDmiss).M()
            m_rm2_D[0] = (cms-pD).M2()
            m_m_Dpi[0] = (pD+pPi).M()
            m_m_Dmisspi[0] = (pDmiss+pPi).M()
            m_m2_Dpi[0] = (pD+pPi).M2()
            m_rm_Dpi[0] = t_std.rm_Dpi
            m_rm_Dmisspi[0] = (cms-pDmiss-pPi).M()
            m_rm_pi[0] = (cms-pPi).M()
            m_chi2_vf[0] = t_std.chi2_vf
            m_chi2_kf[0] = t_std.chi2_kf
            m_n_othershws[0] = t_std.n_othershws
            m_n_othertrks[0] = t_std.n_othertrks
            m_indexmc[0] = t_std.indexmc
            for i in range(t_std.indexmc):
                m_motheridx[i] = t_std.motheridx[i]
                m_pdgid[i] = t_std.pdgid[i]
            if D_sample == 'Dplus' and D_type == 'D' and sample_type == 'signal':
                m_p_pi[0] = t_std.pid_trk[14*0 + 0]
                m_charge_pi[0] = t_std.pid_trk[14*0 + 1]
                m_costheta_pi[0] = t_std.pid_trk[14*0 + 2]
                m_phi_pi[0] = t_std.pid_trk[14*0 + 3]
                m_chi_e_pi[0] = t_std.pid_trk[14*0 + 4]
                m_chi_mu_pi[0] = t_std.pid_trk[14*0 + 5]
                m_chi_pi_pi[0] = t_std.pid_trk[14*0 + 6]
                m_chi_K_pi[0] = t_std.pid_trk[14*0 + 7]
                m_chi_p_pi[0] = t_std.pid_trk[14*0 + 8]
                m_prob_e_pi[0] = t_std.pid_trk[14*0 + 9]
                m_prob_mu_pi[0] = t_std.pid_trk[14*0 + 10]
                m_prob_pi_pi[0] = t_std.pid_trk[14*0 + 11]
                m_prob_K_pi[0] = t_std.pid_trk[14*0 + 12]
                m_prob_p_pi[0] = t_std.pid_trk[14*0 + 13]
                m_p_DK[0] = t_std.pid_trk[14*1 + 0]
                m_charge_DK[0] = t_std.pid_trk[14*1 + 1]
                m_costheta_DK[0] = t_std.pid_trk[14*1 + 2]
                m_phi_DK[0] = t_std.pid_trk[14*1 + 3]
                m_chi_e_DK[0] = t_std.pid_trk[14*1 + 4]
                m_chi_mu_DK[0] = t_std.pid_trk[14*1 + 5]
                m_chi_pi_DK[0] = t_std.pid_trk[14*1 + 6]
                m_chi_K_DK[0] = t_std.pid_trk[14*1 + 7]
                m_chi_p_DK[0] = t_std.pid_trk[14*1 + 8]
                m_prob_e_DK[0] = t_std.pid_trk[14*1 + 9]
                m_prob_mu_DK[0] = t_std.pid_trk[14*1 + 10]
                m_prob_pi_DK[0] = t_std.pid_trk[14*1 + 11]
                m_prob_K_DK[0] = t_std.pid_trk[14*1 + 12]
                m_prob_p_DK[0] = t_std.pid_trk[14*1 + 13]
                m_p_Dpi1[0] = t_std.pid_trk[14*2 + 0]
                m_charge_Dpi1[0] = t_std.pid_trk[14*2 + 1]
                m_costheta_Dpi1[0] = t_std.pid_trk[14*2 + 2]
                m_phi_Dpi1[0] = t_std.pid_trk[14*2 + 3]
                m_chi_e_Dpi1[0] = t_std.pid_trk[14*2 + 4]
                m_chi_mu_Dpi1[0] = t_std.pid_trk[14*2 + 5]
                m_chi_pi_Dpi1[0] = t_std.pid_trk[14*2 + 6]
                m_chi_K_Dpi1[0] = t_std.pid_trk[14*2 + 7]
                m_chi_p_Dpi1[0] = t_std.pid_trk[14*2 + 8]
                m_prob_e_Dpi1[0] = t_std.pid_trk[14*2 + 9]
                m_prob_mu_Dpi1[0] = t_std.pid_trk[14*2 + 10]
                m_prob_pi_Dpi1[0] = t_std.pid_trk[14*2 + 11]
                m_prob_K_Dpi1[0] = t_std.pid_trk[14*2 + 12]
                m_prob_p_Dpi1[0] = t_std.pid_trk[14*2 + 13]
                m_p_Dpi2[0] = t_std.pid_trk[14*3 + 0]
                m_charge_Dpi2[0] = t_std.pid_trk[14*3 + 1]
                m_costheta_Dpi2[0] = t_std.pid_trk[14*3 + 2]
                m_phi_Dpi2[0] = t_std.pid_trk[14*3 + 3]
                m_chi_e_Dpi2[0] = t_std.pid_trk[14*3 + 4]
                m_chi_mu_Dpi2[0] = t_std.pid_trk[14*3 + 5]
                m_chi_pi_Dpi2[0] = t_std.pid_trk[14*3 + 6]
                m_chi_K_Dpi2[0] = t_std.pid_trk[14*3 + 7]
                m_chi_p_Dpi2[0] = t_std.pid_trk[14*3 + 8]
                m_prob_e_Dpi2[0] = t_std.pid_trk[14*3 + 9]
                m_prob_mu_Dpi2[0] = t_std.pid_trk[14*3 + 10]
                m_prob_pi_Dpi2[0] = t_std.pid_trk[14*3 + 11]
                m_prob_K_Dpi2[0] = t_std.pid_trk[14*3 + 12]
                m_prob_p_Dpi2[0] = t_std.pid_trk[14*3 + 13]
            t.Fill()
        if D_sample == 'D0' and (t_std.mode == 0 or t_std.mode == 1 or t_std.mode == 3):
            m_runNo[0] = t_std.runNo
            m_evtNo[0] = t_std.evtNo
            m_mode[0] = t_std.mode
            m_charm[0] = t_std.charm
            m_rawm_D[0] = pD_raw.M()
            m_m_D[0] = pD.M()
            m_p_D[0] = pD.P()
            m_E_D[0] = pD.E()
            m_rm_D[0] = (cms-pD).M()
            m_rm_Dmiss[0] = (cms-pDmiss).M()
            m_rm2_D[0] = (cms-pD).M2()
            m_m_Dpi[0] = (pD+pPi).M()
            m_m_Dmisspi[0] = (pDmiss+pPi).M()
            m_m2_Dpi[0] = (pD+pPi).M2()
            m_rm_Dpi[0] = t_std.rm_Dpi
            m_rm_Dmisspi[0] = (cms-pDmiss-pPi).M()
            m_rm_pi[0] = (cms-pPi).M()
            m_chi2_vf[0] = t_std.chi2_vf
            m_chi2_kf[0] = t_std.chi2_kf
            m_n_othershws[0] = t_std.n_othershws
            m_n_othertrks[0] = t_std.n_othertrks
            m_indexmc[0] = t_std.indexmc
            for i in range(t_std.indexmc):
                m_motheridx[i] = t_std.motheridx[i]
                m_pdgid[i] = t_std.pdgid[i]
            t.Fill()

def main():
    args = sys.argv[1:]
    if len(args)==5:
        file_in = args[0]
        file_out = args[1]
        ecms = float(args[2])
        D_sample = args[3]
        sample_type = args[4]
    elif len(args)==6:
        file_in = args[0]
        file_out = args[1]
        ecms = float(args[2])
        D_sample = args[3]
        D_type = args[4]
        sample_type = args[5]
    else:
        return usage()

    f_in = TFile(file_in)
    f_out = TFile(file_out, 'recreate')
    t_out = TTree('save', 'save')
    cms = TLorentzVector(0.011*ecms, 0, 0, ecms)
    if sample_type == 'raw':
        save_raw(f_in, cms, t_out, D_sample)
    if sample_type == 'signal' or sample_type == 'background':
        save_missing(f_in, cms, t_out, D_sample, D_type, sample_type)

    f_out.cd()
    t_out.Write()
    f_out.Close()

if __name__ == '__main__':
    main()
