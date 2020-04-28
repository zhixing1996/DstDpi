#!/usr/bin/env python
"""
Apply cuts on root files
"""

__author__ = "Maoqiang JING <jingmq@ihep.ac.cn>"
__copyright__ = "Copyright (c) Maoqiang JING"
__created__ = "[2019-10-03 Tue 17:06]"

import math
from array import array
import ROOT
from ROOT import TCanvas, gStyle, TLorentzVector, TTree
from ROOT import TFile, TH1F, TLegend, TArrow, TChain, TVector3
import sys, os
import logging
from math import *
logging.basicConfig(level=logging.DEBUG, format=' %(asctime)s - %(levelname)s- %(message)s')

def usage():
    sys.stdout.write('''
NAME
    apply_cuts.py

SYNOPSIS
    ./apply_cuts.py [file_in] [file_out] [D_sample]

AUTHOR
    Maoqiang JING <jingmq@ihep.ac.cn>

DATE
    October 2019
\n''')

def save_before(file_in, file_out, D_sample):
    try:
        chain = TChain('save')
        chain.Add(file_in)
    except:
        logging.error(file_in + ' is invalid!')
        sys.exit()

    cut = ''
    if D_sample == 'Dplus':
        cut_Dplus = '(m_rawm_D > 1.858 && m_rawm_D < 1.879)'
        cut_chi2 = '(m_chi2_kf < 20)'
        cut_D0 = '(m_rm_Dpi > 1.85 && m_rm_Dpi < 1.88)'
        cut_D0st = '(m_rm_Dpi > 1.996 && m_rm_Dpi < 2.019)'
        cut = cut_Dplus + ' && ' + cut_chi2 + ' && (' + cut_D0 + ' || ' + cut_D0st + ')'
    if D_sample == 'D0':
        cut_D0 = '(m_rawm_D > 1.851 && m_rawm_D < 1.874)'
        cut_chi2 = '(m_chi2_kf < 20)'
        cut_Dplus = '(m_rm_Dpi > 1.855 && m_rm_Dpi < 1.887)'
        cut_Dplusst = '(m_rm_Dpi > 1.996 && m_rm_Dpi < 2.028)'
        cut = cut_D0 + ' && ' + cut_chi2 + ' && (' + cut_Dplus + ' || ' + cut_Dplusst +')'
    if D_sample == 'Dplus_sideband':
        D0_signal_low = 1.86483 - (1.88 - 1.85)/2.
        D0_signal_up = 1.86483 + (1.88 - 1.85)/2.
        D0_sidebandlow_up = D0_signal_low - (D0_signal_up - D0_signal_low)
        D0_sidebandlow_low = D0_sidebandlow_up - (D0_signal_up - D0_signal_low)
        D0_sidebandup_low = D0_signal_up + (D0_signal_up - D0_signal_low)
        D0_sidebandup_up = D0_sidebandup_low + (D0_signal_up - D0_signal_low)
        D0st_signal_low = 2.00685 - (2.019 - 1.996)/2.
        D0st_signal_up = 2.00685 + (2.019 - 1.996)/2.
        D0st_sidebandlow_up = D0st_signal_low - (D0st_signal_up - D0st_signal_low)
        D0st_sidebandlow_low = D0st_sidebandlow_up - (D0st_signal_up - D0st_signal_low)
        D0st_sidebandup_low = D0st_signal_up + (D0st_signal_up - D0st_signal_low)
        D0st_sidebandup_up = D0st_sidebandup_low + (D0st_signal_up - D0st_signal_low)
        cut_Dplus = '(m_rawm_D > 1.858 && m_rawm_D < 1.879)'
        cut_chi2 = '(m_chi2_kf < 20)'
        cut_D0 = '((m_rm_Dpi > ' + str(D0_sidebandlow_low) + ' && m_rm_Dpi < ' + str(D0_sidebandlow_up) + ') || (m_rm_Dpi > ' + str(D0_sidebandup_low) + ' && m_rm_Dpi < ' + str(D0_sidebandup_up) + '))'
        cut_D0st = '((m_rm_Dpi > ' + str(D0st_sidebandlow_low) + ' && m_rm_Dpi < ' + str(D0st_sidebandlow_up) + ') || (m_rm_Dpi > ' + str(D0st_sidebandup_low) + ' && m_rm_Dpi < ' + str(D0st_sidebandup_up) + '))'
        cut = cut_Dplus + ' && ' + cut_chi2 + ' && (' + cut_D0 + ' || ' + cut_D0st + ')'
    if D_sample == 'D0_sideband':
        Dplus_signal_low = 1.86965 - (1.887 - 1.855)/2.
        Dplus_signal_up = 1.86965 + (1.887 - 1.855)/2.
        Dplus_sidebandlow_up = Dplus_signal_low - (Dplus_signal_up - Dplus_signal_low)
        Dplus_sidebandlow_low = Dplus_sidebandlow_up - (Dplus_signal_up - Dplus_signal_low)
        Dplus_sidebandup_low = Dplus_signal_up + (Dplus_signal_up - Dplus_signal_low)
        Dplus_sidebandup_up = Dplus_sidebandup_low + (Dplus_signal_up - Dplus_signal_low)
        Dplusst_signal_low = 2.01026 - (2.028 - 1.996)/2.
        Dplusst_signal_up = 2.01026 + (2.028 - 1.996)/2.
        Dplusst_sidebandlow_up = Dplusst_signal_low - (Dplusst_signal_up - Dplusst_signal_low)
        Dplusst_sidebandlow_low = Dplusst_sidebandlow_up - (Dplusst_signal_up - Dplusst_signal_low)
        Dplusst_sidebandup_low = Dplusst_signal_up + (Dplusst_signal_up - Dplusst_signal_low)
        Dplusst_sidebandup_up = Dplusst_sidebandup_low + (Dplusst_signal_up - Dplusst_signal_low)
        cut_D0 = '(m_rawm_D > 1.851 && m_rawm_D < 1.874)'
        cut_chi2 = '(m_chi2_kf < 20)'
        cut_Dplus = '((m_rm_Dpi > ' + str(Dplus_sidebandlow_low) + ' && m_rm_Dpi < ' + str(Dplus_sidebandlow_up) + ') || (m_rm_Dpi > ' + str(Dplus_sidebandup_low) + ' && m_rm_Dpi < ' + str(Dplus_sidebandup_up) + '))'
        cut_Dplusst = '((m_rm_Dpi > ' + str(Dplusst_sidebandlow_low) + ' && m_rm_Dpi < ' + str(Dplusst_sidebandlow_up) + ') || (m_rm_Dpi > ' + str(Dplusst_sidebandup_low) + ' && m_rm_Dpi < ' + str(Dplusst_sidebandup_up) + '))'
        cut = cut_D0 + ' && ' + cut_chi2 + ' && (' + cut_Dplus + ' || ' + cut_Dplusst + ')'
    
    print cut
    # t = chain.CopyTree(cut)
    # t.SaveAs(file_out)

def main():
    args = sys.argv[1:]
    if len(args)<3:
        return usage()
    file_in = args[0]
    file_out = args[1]
    D_sample = args[2]

    print '--> Begin to process file: ' + file_in
    save_before(file_in, file_out, D_sample)
    print '--> End of processing file: ' + file_out

if __name__ == '__main__':
    main()
