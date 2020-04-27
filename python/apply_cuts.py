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
        cut_Dplus = '(m_rawm_D > 1.85 && m_rawm_D < 1.89)'
        cut_chi2 = '(m_chi2_kf < 20)'
        cut_D0 = '(m_rm_Dpi > 1.855 && m_rm_Dpi < 1.875)'
        cut_D0st = '(m_rm_Dpi > 1.995 && m_rm_Dpi < 2.015)'
        cut = cut_Dplus + ' && ' + cut_chi2 + ' && ' + cut_D0 + ' && ' + cut_D0st
    if D_sample == 'D0':
        cut_D0 = '(m_rawm_D > 1.85 && m_rawm_D < 1.89)'
        cut_chi2 = '(m_chi2_kf < 20)'
        cut_D0 = '(m_rm_Dpi > 1.855 && m_rm_Dpi < 1.875)'
        cut_D0st = '(m_rm_Dpi > 1.995 && m_rm_Dpi < 2.015)'
        cut = cut_D0 + ' && ' + cut_chi2 + ' && ' + cut_D0 + ' && ' + cut_D0st
    
    t = chain.CopyTree(cut)
    t.SaveAs(file_out)

def main():
    args = sys.argv[1:]
    if len(args)<4:
        return usage()
    file_in = args[0]
    file_out = args[1]
    D_sample = args[3]

    print '--> Begin to process file: ' + file_in
    save_before(file_in, file_out, D_sample)
    print '--> End of processing file: ' + file_out

if __name__ == '__main__':
    main()
