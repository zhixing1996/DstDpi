#!/usr/bin/env python
"""
Optiomize recoiling mass of tagged D and pi
"""

__author__ = "Maoqiang JING <jingmq@ihep.ac.cn>"
__copyright__ = "Copyright (c) Maoqiang JING"
__created__ = "[2020-05-18 Mon 10:53]"

import ROOT
from ROOT import TCanvas, gStyle
from ROOT import TFile, TH2F, TPaveText, TArrow
import sys, os
import logging
from math import *
from tools import width_rawm_D
logging.basicConfig(level=logging.DEBUG, format=' %(asctime)s - %(levelname)s- %(message)s')
gStyle.SetPaperSize(20,30)
gStyle.SetPadTopMargin(0.05)
gStyle.SetPadRightMargin(0.08)
gStyle.SetPadBottomMargin(0.18)
gStyle.SetPadLeftMargin(0.16)
gStyle.SetOptTitle(0)
gStyle.SetOptTitle(0)

def usage():
    sys.stdout.write('''
NAME
    opt_rm_Dpi.py

SYNOPSIS
    ./opt_rm_Dpi.py [ecms] [D_sample] [D_type]

AUTHOR
    Maoqiang JING <jingmq@ihep.ac.cn>

DATE
    May 2020
\n''')

def set_pavetext(pt):
    pt.SetFillStyle(0)
    pt.SetBorderSize(0)
    pt.SetTextAlign(10)
    pt.SetTextSize(0.05)

def set_arrow(arrow):
    arrow.SetLineWidth(0)
    arrow.SetLineColor(2)
    arrow.SetFillColor(2)

def set_histo_style(h, xtitle, ytitle):
    h.GetXaxis().SetNdivisions(509)
    h.GetYaxis().SetNdivisions(504)
    h.SetLineWidth(2)
    h.SetLineWidth(2)
    h.SetStats(0)
    h.SetStats(0)
    h.GetXaxis().SetTitleSize(0.05)
    h.GetXaxis().SetTitleOffset(1.13)
    h.GetXaxis().SetLabelOffset(0.01)
    h.GetYaxis().SetTitleSize(0.05)
    h.GetYaxis().SetTitleOffset(1.13)
    h.GetYaxis().SetLabelOffset(0.01)
    h.GetXaxis().SetTitle(xtitle)
    h.GetXaxis().CenterTitle()
    h.GetYaxis().SetTitle(ytitle)
    h.GetYaxis().CenterTitle()
    h.SetMarkerStyle(8)
    h.SetMarkerSize(0.65)
    h.SetLineColor(1)

def cal_significance(t1, t2, N, step, ecms, M_D1, M_D2, D_sample, D_type):
    ymax = 0
    NEntry = 0
    B_list = []
    print 'Start of incMC...'
    for i in xrange(N):
        B = 0
        for j in xrange(76*2000):
            t1.GetEntry(j)
            if fabs(t1.m_rm_Dpi - M_D1) < (step + i*step) and fabs(t1.m_rawm_D - M_D2) < width_rawm_D(D_sample):
                B = B + 1
        print 'processing: ' + str(i)
        B_list.append(B)
    S_list = []
    print 'Start of sigMC...'
    for i in xrange(N):
        S = 0
        for j in xrange(2000):
            t2.GetEntry(j)
            if fabs(t2.m_rm_Dpi - M_D1) < (step + i*step) and fabs(t2.m_rawm_D - M_D2) < width_rawm_D(D_sample):
                S = S + 1
        print 'processing: ' + str(i)
        S_list.append(S)
    Ratio_list = []
    for i in xrange(N):
        if B_list[i] == 0:
            significance = 0
        else:
            significance = S_list[i]/sqrt(S_list[i] + B_list[i])
        Ratio_list.append(significance)
        if significance > ymax:
            ymax = significance
            NEntry = i
    xmin = step
    xmax = N*step
    if D_sample == 'Dplus' and D_type == 'D':
        xtitle = '|RM(D^{+}#pi^{-})-m_{#bar{D^{0}}}|(GeV)'
    if D_sample == 'D0' and D_type == 'D':
        xtitle = '|RM(D^{0}#pi^{+})-m_{D^{-}}|(GeV)'
    ytitle = '#frac{S}{#sqrt{S+B}}'
    h_FOM = TH2F('h_FOM', 'FOM', N, xmin, xmax, N, 0, ymax + 2)
    set_histo_style(h_FOM, xtitle, ytitle)
    for i in xrange(N):
        h_FOM.Fill(step + i*step, Ratio_list[i])
    return h_FOM, NEntry, ymax

def set_canvas_style(mbc):
    mbc.SetFillColor(0)
    mbc.SetLeftMargin(0.15)
    mbc.SetRightMargin(0.15)
    mbc.SetTopMargin(0.1)
    mbc.SetBottomMargin(0.15)

def plot(path, pt_title, ecms, arrow_left, arrow_bottom, arrow_right, arrow_top, D_sample, D_type):
    try:
        f_incMC = TFile(path[0])
        f_sigMC = TFile(path[1])
        t_incMC = f_incMC.Get('save')
        t_sigMC = f_sigMC.Get('save')
        entries_incMC = t_incMC.GetEntries()
        entries_sigMC = t_sigMC.GetEntries()
        logging.info('inclusive MC entries :'+str(entries_incMC))
        logging.info('D2(2460) MC entries :'+str(entries_sigMC))
    except:
        logging.error('Files are invalid!')
        sys.exit()

    mbc = TCanvas('mbc', 'mbc', 800, 600)
    set_canvas_style(mbc)
    xbins = 100
    M_D1 = 0.
    M_D2 = 0.
    step = 0.
    if D_sample == 'Dplus' and D_type == 'D':
        M_D1 = 1.86483
        M_D2 = 1.86965
        step = (1.89555 - M_D1)/xbins
    if D_sample == 'D0' and D_type == 'D':
        M_D1 = 1.86965
        M_D2 = 1.86483
        step = (1.95 - M_D1)/xbins

    h_FOM, ientry, arrow_top = cal_significance(t_incMC, t_sigMC, xbins, step, ecms, M_D1, M_D2, D_sample, D_type)
    h_FOM.Draw()
    
    # arrow_left = ientry*step + step
    # arrow_right = ientry*step + step
    # arrow_top = 70.
    arrow_bottom = 0.
    arrow = TArrow(arrow_left, arrow_bottom, arrow_right, arrow_top, 0.01,'>')
    set_arrow(arrow)
    arrow.Draw()

    pt = TPaveText(0.6, 0.8, 0.85, 0.85, "BRNDC")
    set_pavetext(pt)
    pt.Draw()
    pt.AddText(pt_title)

    mass_low = 0.
    mass_up = 0.
    window = 0.
    if D_sample == 'Dplus' or D_sample == 'D0':
        mass_low = str(M_D1 - arrow_left)
        mass_up = str(M_D1 + arrow_left)
        window = str(2*arrow_left)
    # mass_low = str(M_D1 - (ientry*step + step))
    # mass_up = str(M_D1 + (ientry*step + step))
    # window = str(2*(ientry*step + step))
    range = 'Mass window of rawm_D : [' + mass_low + ', ' + mass_up + '] GeV/c2' + ' with mass window width: ' + window + ' GeV/c2'
    print range

    if not os.path.exists('./txts/'):
        os.makedirs('./txts/')

    path_out = './txts/window_rm_Dpi_' + str(ecms) + '_' + D_sample + '_' + D_type +'.txt'
    f_out = open(path_out, 'w')
    f_out.write(range)
    f_out.close()

    if not os.path.exists('./figs/'):
        os.makedirs('./figs/')

    mbc.Update()
    mbc.SaveAs('./figs/opt_rm_Dpi_' + str(ecms) + '_' + D_sample + '_' + D_type + '.pdf')

    raw_input('Enter anything to end...')

def main():
    args = sys.argv[1:]
    if len(args)<3:
        return usage()
    ecms = int(args[0])
    D_sample = args[1]
    D_type = args[2]

    if ecms == 4600:
        path = []
        path.append('/besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/anaroot/mc/DD/4600/incMC_DD_4600_DstDpi_'+D_sample+'_'+D_type+'_signal.root')
        path.append('/besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/anaroot/mc/D2_2460/4600/mc_D2_2460_4600_DstDpi_'+D_sample+'_'+D_type+'_signal.root')
        pt_title = str(ecms) + ' MeV'
        arrow_left = 0.012
        arrow_right = 0.012
        arrow_bottom = 0
        arrow_top = 0
        plot(path, pt_title, ecms, arrow_left, arrow_bottom, arrow_right, arrow_top, D_sample, D_type)

if __name__ == '__main__':
    main()
