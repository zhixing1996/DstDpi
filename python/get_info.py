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
    ./get_info.py [file_in] [file_out] [ecms] [D_sample]

AUTHOR
    Maoqiang JING <jingmq@ihep.ac.cn>

DATE
    August 2019
\n''')

def save_raw(f_in, cms, t, sample):
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
    m_m2_Kpip = array('d', [999.])
    m_m2_Kpim = array('d', [999.])
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
    t.Branch('m2_Kpip', m_m2_Kpip, 'm_m2_Kpip/D')
    t.Branch('m2_Kpim', m_m2_Kpim, 'm_m2_Kpim/D')
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
        pKpip = TLorentzVector(0, 0, 0, 0)
        pKpim = TLorentzVector(0, 0, 0, 0)
        for iTrk in range(t_std.n_trkD):
            ptrack_raw = TLorentzVector(0, 0, 0, 0)
            ptrack_Kpip = TLorentzVector(0, 0, 0, 0)
            ptrack_Kpim = TLorentzVector(0, 0, 0, 0)
            ptrack = TLorentzVector(0, 0, 0, 0)
            ptrack_raw.SetPxPyPzE(t_std.rawp4_Dtrk[iTrk*6+0], t_std.rawp4_Dtrk[iTrk*6+1], t_std.rawp4_Dtrk[iTrk*6+2], t_std.rawp4_Dtrk[iTrk*6+3])
            ptrack.SetPxPyPzE(t_std.p4_Dtrk[iTrk*4+0], t_std.p4_Dtrk[iTrk*4+1], t_std.p4_Dtrk[iTrk*4+2], t_std.p4_Dtrk[iTrk*4+3])
            pD_raw += ptrack_raw
            pD += ptrack
            if t_std.rawp4_Dtrk[iTrk*6+5] == 3:
                ptrack_Kpip.SetPxPyPzE(t_std.rawp4_Dtrk[iTrk*6+0], t_std.rawp4_Dtrk[iTrk*6+1], t_std.rawp4_Dtrk[iTrk*6+2], t_std.rawp4_Dtrk[iTrk*6+3])
                ptrack_Kpim.SetPxPyPzE(t_std.rawp4_Dtrk[iTrk*6+0], t_std.rawp4_Dtrk[iTrk*6+1], t_std.rawp4_Dtrk[iTrk*6+2], t_std.rawp4_Dtrk[iTrk*6+3])
                pKpip += ptrack_Kpip
                pKpim += ptrack_Kpim
            if t_std.rawp4_Dtrk[iTrk*6+4] == 1 and t_std.rawp4_Dtrk[iTrk*6+5] == 2:
                ptrack_Kpip.SetPxPyPzE(t_std.rawp4_Dtrk[iTrk*6+0], t_std.rawp4_Dtrk[iTrk*6+1], t_std.rawp4_Dtrk[iTrk*6+2], t_std.rawp4_Dtrk[iTrk*6+3])
                pKpip += ptrack_Kpip
            if t_std.rawp4_Dtrk[iTrk*6+4] == -1 and t_std.rawp4_Dtrk[iTrk*6+5] == 2:
                ptrack_Kpim.SetPxPyPzE(t_std.rawp4_Dtrk[iTrk*6+0], t_std.rawp4_Dtrk[iTrk*6+1], t_std.rawp4_Dtrk[iTrk*6+2], t_std.rawp4_Dtrk[iTrk*6+3])
                pKpim += ptrack_Kpim
        for iShw in range(t_std.n_shwD):
            pshower_raw = TLorentzVector(0, 0, 0, 0)
            pshower = TLorentzVector(0, 0, 0, 0)
            pshower_raw.SetPxPyPzE(t_std.rawp4_Dshw[iShw*4+0], t_std.rawp4_Dshw[iShw*4+1], t_std.rawp4_Dshw[iShw*4+2], t_std.rawp4_Dshw[iShw*4+3])
            pshower.SetPxPyPzE(t_std.p4_Dshw[iShw*4+0], t_std.p4_Dshw[iShw*4+1], t_std.p4_Dshw[iShw*4+2], t_std.p4_Dshw[iShw*4+3])
            pD_raw += pshower_raw
            pD += pshower
        pPi = TLorentzVector(0,0,0,0)
        for trk in range(t_std.n_othertrks):
            if abs(t_std.rawp4_otherMdcKaltrk[trk*7+4]) != 1:
                continue
            if t_std.rawp4_otherMdcKaltrk[trk*7+5] != 2:
                continue
            pPi.SetPxPyPzE(t_std.rawp4_otherMdcKaltrk[trk*7+0], t_std.rawp4_otherMdcKaltrk[trk*7+1], t_std.rawp4_otherMdcKaltrk[trk*7+2], t_std.rawp4_otherMdcKaltrk[trk*7+3])
            if sample == 'Dplus' and t_std.mode == 200:
                if ((t_std.charm == 1 and t_std.rawp4_otherMdcKaltrk[trk*7+4] == -1) or (t_std.charm == -1 and t_std.rawp4_otherMdcKaltrk[trk*7+4] == 1)):
                    m_runNo[0] = t_std.runNo
                    m_evtNo[0] = t_std.evtNo
                    m_mode[0] = t_std.mode
                    m_charm[0] = t_std.charm
                    m_charge_pi[0] = t_std.rawp4_otherMdcKaltrk[trk*7+4]
                    m_rawm_D[0] = pD_raw.M()
                    m_m_D[0] = pD.M()
                    m_p_D[0] = pD.P()
                    m_E_D[0] = pD.E()
                    m_rm_D[0] = (cms-pD).M()
                    m_rm2_D[0] = (cms-pD).M2()
                    m_m_Dpi[0] = (pD+pPi).M()
                    m_m2_Dpi[0] = (pD+pPi).M2()
                    m_rm_Dpi[0] = (cms-pD-pPi).M()
                    m_m2_Kpip[0] = pKpip.M2()
                    m_m2_Kpim[0] = pKpim.M2()
                    m_chi2_vf[0] = t_std.chi2_vf
                    m_chi2_kf[0] = t_std.chi2_kf
                    m_n_othershws[0] = t_std.n_othershws
                    m_n_othertrks[0] = t_std.n_othertrks
                    m_indexmc[0] = t_std.indexmc
                    for i in range(t_std.indexmc):
                        m_motheridx[i] = t_std.motheridx[i]
                        m_pdgid[i] = t_std.pdgid[i]
                    t.Fill()
            if sample == 'D0' and (t_std.mode == 0 or t_std.mode == 1 or t_std.mode == 3):
                if (t_std.charm == 1 and t_std.rawp4_otherMdcKaltrk[trk*7+4] == 1) or (t_std.charm == -1 and t_std.rawp4_otherMdcKaltrk[trk*7+4] == -1):
                    m_runNo[0] = t_std.runNo
                    m_evtNo[0] = t_std.evtNo
                    m_mode[0] = t_std.mode
                    m_charm[0] = t_std.charm
                    m_charge_pi[0] = t_std.rawp4_otherMdcKaltrk[trk*7+4]
                    m_rawm_D[0] = pD_raw.M()
                    m_m_D[0] = pD.M()
                    m_p_D[0] = pD.P()
                    m_E_D[0] = pD.E()
                    m_rm_D[0] = (cms-pD).M()
                    m_rm2_D[0] = (cms-pD).M2()
                    m_m_Dpi[0] = (pD+pPi).M()
                    m_m2_Dpi[0] = (pD+pPi).M2()
                    m_rm_Dpi[0] = (cms-pD-pPi).M()
                    m_m2_Kpip[0] = pKpip.M2()
                    m_m2_Kpim[0] = pKpim.M2()
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
    if len(args)<3:
        return usage()
    file_in = args[0]
    file_out = args[1]
    ecms = float(args[2])
    D_sample = args[3]

    f_in = TFile(file_in)
    f_out = TFile(file_out, 'recreate')
    t_out = TTree('save', 'save')
    cms = TLorentzVector(0.011*ecms, 0, 0, ecms)
    save_raw(f_in, cms, t_out, D_sample)

    f_out.cd()
    t_out.Write()
    f_out.Close()

if __name__ == '__main__':
    main()
