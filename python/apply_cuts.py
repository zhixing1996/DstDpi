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
from tools import width_rawm_D, width_rm_Dpi
logging.basicConfig(level=logging.DEBUG, format=' %(asctime)s - %(levelname)s- %(message)s')

def usage():
    sys.stdout.write('''
NAME
    apply_cuts.py

SYNOPSIS
    ./apply_cuts.py [file_in] [file_out] [D_sample] [D_type]

AUTHOR
    Maoqiang JING <jingmq@ihep.ac.cn>

DATE
    October 2019
\n''')

def save_before(file_in, file_out, D_sample, D_type):
    try:
        chain = TChain('save')
        chain.Add(file_in)
    except:
        logging.error(file_in + ' is invalid!')
        sys.exit()

    cut = ''
    if D_sample == 'Dplus' and D_type == 'raw':
        rawm_D_low = 1.86965 - width_rawm_D(D_sample)/2.
        rawm_D_up = 1.86965 + width_rawm_D(D_sample)/2.
        cut_Dplus = '(m_rawm_D > ' + str(rawm_D_low) + ' && m_rawm_D < ' + str(rawm_D_up) + ')'
        cut_chi2 = '(m_chi2_kf < 20)'
        cut = cut_Dplus + ' && ' + cut_chi2

    if D_sample == 'D0' and D_type == 'raw':
        rawm_D_low = 1.86483 - width_rawm_D(D_sample)/2.
        rawm_D_up = 1.86483 + width_rawm_D(D_sample)/2.
        cut_D0 = '(m_rawm_D > ' + str(rawm_D_low) + ' && m_rawm_D < ' + str(rawm_D_up) + ')'
        cut_chi2 = '(m_chi2_kf < 20)'
        cut = cut_D0 + ' && ' + cut_chi2

    if D_sample == 'Dplus' and D_type == 'D':
        rawm_D_low = 1.86965 - width_rawm_D(D_sample)/2.
        rawm_D_up = 1.86965 + width_rawm_D(D_sample)/2.
        cut_Dplus = '(m_rawm_D > ' + str(rawm_D_low) + ' && m_rawm_D < ' + str(rawm_D_up) + ')'
        rm_Dpi_low = 1.86483 - width_rm_Dpi(D_sample, D_type)/2.
        rm_Dpi_up = 1.86483 + width_rm_Dpi(D_sample, D_type)/2.
        cut_D0 = '(m_rm_Dpi > ' + str(rm_Dpi_low) + ' && m_rm_Dpi < ' + str(rm_Dpi_up) + ')'
        cut = cut_Dplus + ' && ' + cut_D0

    if D_sample == 'Dplus' and D_type == 'Dst':
        rawm_D_low = 1.86965 - width_rawm_D(D_sample)/2.
        rawm_D_up = 1.86965 + width_rawm_D(D_sample)/2.
        cut_Dplus = '(m_rawm_D > ' + str(rawm_D_low) + ' && m_rawm_D < ' + str(rawm_D_up) + ')'
        cut_D0st = '(m_rm_Dpi > 1.996 && m_rm_Dpi < 2.019)'
        cut = cut_Dplus + ' && ' + cut_D0st

    if D_sample == 'D0' and D_type == 'D':
        rawm_D_low = 1.86483 - width_rawm_D(D_sample)/2.
        rawm_D_up = 1.86483 + width_rawm_D(D_sample)/2.
        cut_D0 = '(m_rawm_D > ' + str(rawm_D_low) + ' && m_rawm_D < ' + str(rawm_D_up) + ')'
        rm_Dpi_low = 1.86965 - width_rm_Dpi(D_sample, D_type)/2.
        rm_Dpi_up = 1.86965 + width_rm_Dpi(D_sample, D_type)/2.
        cut_Dplus = '(m_rm_Dpi > ' + str(rm_Dpi_low) + ' && m_rm_Dpi < ' +  str(rm_Dpi_up) + ')'
        cut = cut_D0 + ' && ' + cut_Dplus

    if D_sample == 'D0' and D_type == 'Dst':
        rawm_D_low = 1.86483 - width_rawm_D(D_sample)/2.
        rawm_D_up = 1.86483 + width_rawm_D(D_sample)/2.
        cut_D0 = '(m_rawm_D > ' + str(rawm_D_low) + ' && m_rawm_D < ' + str(rawm_D_up) + ')'
        cut_Dplusst = '(m_rm_Dpi > 1.996 && m_rm_Dpi < 2.028)'
        cut = cut_D0 + ' && ' + cut_Dplusst

    if D_sample == 'Dplus' and D_type == 'missing':
        rawm_D_low = 1.86965 - width_rawm_D(D_sample)/2.
        rawm_D_up = 1.86965 + width_rawm_D(D_sample)/2.
        cut_Dplus = '(m_rawm_D > ' + str(rawm_D_low) + ' && m_rawm_D < ' + str(rawm_D_up) + ')'
        cut = cut_Dplus

    t = chain.CopyTree(cut)
    t.SaveAs(file_out)

def main():
    args = sys.argv[1:]
    if len(args)<4:
        return usage()
    file_in = args[0]
    file_out = args[1]
    D_sample = args[2]
    D_type = args[3]

    print '--> Begin to process file: ' + file_in
    save_before(file_in, file_out, D_sample, D_type)
    print '--> End of processing file: ' + file_out

if __name__ == '__main__':
    main()
