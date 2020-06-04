#!/usr/bin/env python
"""
Plot raw invariant mass of tagged D
"""

__author__ = "Maoqiang JING <jingmq@ihep.ac.cn>"
__copyright__ = "Copyright (c) Maoqiang JING"
__created__ = "[2020-05-19 Tue 17:39]"

from ROOT import *
import sys, os
import logging
from math import *
from tools import convert_name
logging.basicConfig(level=logging.DEBUG, format=' %(asctime)s - %(levelname)s- %(message)s')
gStyle.SetOptTitle(0) # quench title
gStyle.SetPadTickX(1) # dicide on boxing on or not of x and y axis  
gStyle.SetPadTickY(1) # dicide on boxing on or not of x and y axis

def usage():
    sys.stdout.write('''
NAME
    plot_rawm_D.py

SYNOPSIS
    ./plot_rawm_D.py [ecms] [D_sample]

AUTHOR
    Maoqiang JING <jingmq@ihep.ac.cn>

DATE
    May 2020
\n''')

def set_pavetext(pt):
    pt.SetFillStyle(0)
    pt.SetBorderSize(0)
    pt.SetTextAlign(10)
    pt.SetTextSize(0.03)

def rawm_D_fill(t, h):
    for ientry in xrange(t.GetEntries()):
        t.GetEntry(ientry)
        h.Fill(t.m_rawm_D)

def set_histo_style(h, xtitle, ytitle):
    h.GetXaxis().SetNdivisions(509)
    h.GetYaxis().SetNdivisions(504)
    h.SetLineWidth(2)
    h.SetLineWidth(2)
    h.SetStats(0)
    h.SetStats(0)
    h.GetXaxis().SetTitleSize(0.06)
    h.GetXaxis().SetTitleOffset(1.)
    h.GetXaxis().SetLabelOffset(0.01)
    h.GetYaxis().SetTitleSize(0.06)
    h.GetYaxis().SetTitleOffset(1.)
    h.GetYaxis().SetLabelOffset(0.01)
    h.GetXaxis().SetTitle(xtitle)
    h.GetXaxis().CenterTitle()
    h.GetYaxis().SetTitle(ytitle)
    h.GetYaxis().CenterTitle()
    h.SetFillColor(1)

def set_pad_style(pad):
    pad.SetLeftMargin(0.15)
    pad.SetRightMargin(0.1)
    pad.SetTopMargin(0.1)
    pad.SetBottomMargin(0.15)
    pad.SetFrameLineColor(kBlack)

def set_canvas_style(mbc):
    mbc.SetFillColor(0)
    mbc.SetLeftMargin(0.15)
    mbc.SetRightMargin(0.15)
    mbc.SetTopMargin(0.1)
    mbc.SetBottomMargin(0.15)

def plot(path, leg_title, ecms, xmin, xmax, xbins, D_sample):
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
    mbc.cd()
    content = (xmax - xmin)/xbins * 1000
    ytitle = 'Events/%.1f MeV'%content
    xtitle = ''
    if D_sample == 'Dplus':
        xtitle = 'M(K^{-}#pi^{+}#pi^{+})(GeV)'
    if D_sample == 'D0':
        xtitle = 'M(K^{-}#pi^{+} or K^{-}#pi^{+}#pi^{0} or K^{-}#pi^{+}#pi^{+}#pi^{-})(GeV)'
    h_data = TH1F('data', 'data', xbins, xmin, xmax)
    set_histo_style(h_data, xtitle, ytitle)
    rawm_D_fill(t_data, h_data)
    
    if not os.path.exists('./figs/'):
        os.makedirs('./figs/')
    
    h_data.Draw('E1')

    if D_sample == 'Dplus':
        pt = TPaveText(0.7, 0.7, 0.85, 0.85, "BRNDC")
        set_pavetext(pt)
        pt.Draw()
        pt.AddText(leg_title)
        pt.AddText('D^{+}#rightarrowK^{-}#pi^{+}#pi^{+}')
    if D_sample == 'D0':
        pt = TPaveText(0.2, 0.65, 0.45, 0.85, "BRNDC")
        set_pavetext(pt)
        pt.Draw()
        pt.AddText(leg_title)
        pt.AddText('D^{0}#rightarrowK^{-}#pi^{+}')
        pt.AddText('D^{0}#rightarrowK^{-}#pi^{+}#pi^{0}')
        pt.AddText('D^{0}#rightarrowK^{-}#pi^{+}#pi^{+}#pi^{-}')

    mbc.SaveAs('./figs/rawm_D_'+str(ecms)+'_'+D_sample+'.pdf')

    # raw_input('Press <Enter> to end...')

if __name__ == '__main__':
    args = sys.argv[1:]
    if len(args)<2:
        usage()
        sys.exit()
    ecms = int(args[0])
    D_sample = args[1]

    path = []
    path.append('/besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/anaroot/data/'+str(ecms)+'/data_'+str(ecms)+'_DstDpi_'+D_sample+'.root')
    leg_title = str(ecms) + ' MeV'
    if D_sample == 'Dplus':
        xmin = 1.81
        xmax = 1.93
        xbins = 24
    if D_sample == 'D0':
        xmin = 1.75
        xmax = 1.95
        xbins = 40
    plot(path, leg_title, ecms, xmin, xmax, xbins, D_sample)
