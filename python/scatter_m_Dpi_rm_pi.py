#!/usr/bin/env python
"""
Plot invariant mass of Dpi**2 vs recoiling mass of pi**2
"""

__author__ = "Maoqiang JING <jingmq@ihep.ac.cn>"
__copyright__ = "Copyright (c) Maoqiang JING"
__created__ = "[2020-04-27 Mon 20:26]"

from ROOT import *
import sys, os
import logging
from tools import convert_name
logging.basicConfig(level=logging.DEBUG, format=' %(asctime)s - %(levelname)s- %(message)s')
gStyle.SetOptTitle(0) # quench title
gStyle.SetPadTickX(1) # dicide on boxing on or not of x and y axis  
gStyle.SetPadTickY(1) # dicide on boxing on or not of x and y axis

def usage():
    sys.stdout.write('''
NAME
    scatter_m_Dpi_rm_pi.py

SYNOPSIS
    ./scatter_m_Dpi_rm_pi.py [ecms] [D_sample] [D_type]

AUTHOR
    Maoqiang JING <jingmq@ihep.ac.cn>

DATE
    April 2020
\n''')

def set_pavetext(pt):
    pt.SetFillStyle(0)
    pt.SetBorderSize(0)
    pt.SetTextAlign(10)
    pt.SetTextSize(0.03)

def fill(t, h, D_sample, D_type):
    for ientry in xrange(t.GetEntries()):
        t.GetEntry(ientry)
        if D_sample == 'Dplus' and D_type == 'D':
            if t.m_rm_D > 2.03:
                h.Fill(t.m_m2_Dpi, t.m_rm_pi*t.m_rm_pi)
        elif D_sample == 'D0':
            if t.m_m_Dpi > 2.03:
                h.Fill(t.m_m2_Dpi, t.m_rm_pi*t.m_rm_pi)
        else:
            h.Fill(t.m_m2_Dpi, t.m_rm_pi*t.m_rm_pi)

def set_histo_style(h, xtitle, ytitle):
    h.GetXaxis().SetNdivisions(509)
    h.GetYaxis().SetNdivisions(504)
    h.SetStats(0)
    h.GetXaxis().SetTitleSize(0.06)
    h.GetXaxis().SetTitleOffset(1.)
    h.GetXaxis().SetLabelOffset(0.01)
    h.GetYaxis().SetTitleSize(0.06)
    h.GetYaxis().SetTitleOffset(1.)
    h.GetYaxis().SetLabelOffset(0.01)
    h.GetXaxis().SetTitle(xtitle)
    h.GetYaxis().SetTitle(ytitle)
    h.GetXaxis().CenterTitle()
    h.GetYaxis().CenterTitle()
    h.SetLineColor(1)

def set_canvas_style(mbc):
    mbc.SetFillColor(0)
    mbc.SetLeftMargin(0.15)
    mbc.SetRightMargin(0.15)
    mbc.SetTopMargin(0.1)
    mbc.SetBottomMargin(0.15)

def plot(path, leg_title, ecms, xmin, xmax, ymin, ymax, bins, D_sample, D_type):
    try:
        f_data = TFile(path[0])
        t_data = f_data.Get('save')
        entries_data = t_data.GetEntries()
        logging.info('data entries :'+str(entries_data))
    except:
        logging.error(path[0] + ' is invalid!')
        sys.exit()

    mbc = TCanvas('mbc', 'mbc', 800, 600)
    set_canvas_style(mbc)
    if D_sample == 'Dplus':
        xtitle = 'M^{2}(D^{+}#pi_{0}^{-})(GeV^{2})'
        ytitle = 'RM^{2}(#pi_{0}^{-})(GeV^{2})'
    if D_sample == 'D0':
        xtitle = 'M^{2}(D^{0}#pi_{0}^{+})(GeV^{2})'
        ytitle = 'RM^{2}(#pi_{0}^{+})(GeV^{2})'
    h_data = TH2F('scatter_data', 'scatter plot of M2(Dpi) and RM2(pi)', bins, xmin, xmax, bins, ymin, ymax)

    set_histo_style(h_data, xtitle, ytitle)
    fill(t_data, h_data, D_sample, D_type)
    
    if not os.path.exists('./figs/'):
        os.makedirs('./figs/')
    
    h_data.Draw('COLZ')

    pt = TPaveText(0.65, 0.7, 0.85, 0.85, "BRNDC")
    set_pavetext(pt)
    pt.Draw()
    pt.AddText(leg_title)
    if D_sample == 'Dplus' and D_type == 'D':
        pt.AddText('e^{+}e^{-}#rightarrowD^{+}#pi_{0}^{-}#bar{D^{0}}')
    if D_sample == 'Dplus' and D_type == 'Dst':
        pt.AddText('e^{+}e^{-}#rightarrowD^{+}#pi_{0}^{-}#bar{D^{*0}}')
    if D_sample == 'D0' and D_type == 'D':
        pt.AddText('e^{+}e^{-}#rightarrowD^{0}#pi_{0}^{+}D^{-}')
    if D_sample == 'D0' and D_type == 'Dst':
        pt.AddText('e^{+}e^{-}#rightarrowD^{0}#pi_{0}^{+}D^{*-}')

    mbc.SaveAs('./figs/scatter_m_Dpi_rm_pi_'+str(ecms)+'_'+D_sample+'_'+D_type+'.pdf')

    # raw_input('Press <Enter> to end...')

if __name__ == '__main__':
    args = sys.argv[1:]
    if len(args)<3:
        usage()
        sys.exit()
    ecms = int(args[0])
    D_sample = args[1]
    D_type = args[2]

    path = []
    path.append('/besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/anaroot/data/'+str(ecms)+'/data_'+str(ecms)+'_DstDpi_'+D_sample+'_'+convert_name(D_sample, D_type)+'_signal_before.root')
    leg_title = str(ecms) + ' MeV'
    xmin = 3.0
    xmax = 9.0
    ymin = 12.0
    ymax = 22.0
    if D_sample == 'Dplus':
        bins = 280
    if D_sample == 'D0':
        bins = 330
    plot(path, leg_title, ecms, xmin, xmax, ymin, ymax, bins, D_sample, D_type)
