#!/usr/bin/env python
"""
Plot recoiling mass of tagged pi and D
"""

__author__ = "Maoqiang JING <jingmq@ihep.ac.cn>"
__copyright__ = "Copyright (c) Maoqiang JING"
__created__ = "[2020-04-26 Sun 21:22]"

from ROOT import *
import sys, os
import logging
from math import *
logging.basicConfig(level=logging.DEBUG, format=' %(asctime)s - %(levelname)s- %(message)s')
gStyle.SetOptTitle(0) # quench title
gStyle.SetPadTickX(1) # dicide on boxing on or not of x and y axis  
gStyle.SetPadTickY(1) # dicide on boxing on or not of x and y axis

def usage():
    sys.stdout.write('''
NAME
    plot_rm_Dpi.py

SYNOPSIS
    ./plot_rm_Dpi.py [ecms] [D_sample]

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

def set_arrow(arrow, color):
    arrow.SetLineWidth(0)
    arrow.SetLineColor(color)
    arrow.SetFillColor(color)

def rm_Dpi_fill(t, h, D_sample):
    for ientry in xrange(t.GetEntries()):
        t.GetEntry(ientry)
        if D_sample == 'Dplus':
            h.Fill(t.m_rm_Dpi)
        if D_sample == 'D0':
            h.Fill(t.m_rm_Dpi)

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
    content = (xmax - xmin)/xbins * 1000
    ytitle = 'Events/%.1f MeV'%content
    xtitle = ''
    if D_sample == 'Dplus':
        xtitle = 'RM(D^{+}#pi^{-}_{0})(GeV)'
    if D_sample == 'D0':
        xtitle = 'RM(D^{0}#pi^{+}_{0})(GeV)'
    h_data = TH1F('data', 'data', xbins, xmin, xmax)
    
    set_histo_style(h_data, xtitle, ytitle)
    rm_Dpi_fill(t_data, h_data, D_sample)
    
    if not os.path.exists('./figs/'):
        os.makedirs('./figs/')
    
    h_data.Draw('E1')

    if D_sample == 'Dplus':
        pt = TPaveText(0.2, 0.7, 0.45, 0.85, "BRNDC")
        set_pavetext(pt)
        pt.Draw()
        pt.AddText(leg_title)
        pt.AddText('D^{+}#rightarrowK^{-}#pi^{+}#pi^{+}')
    if D_sample == 'D0':
        pt = TPaveText(0.2, 0.7, 0.45, 0.85, "BRNDC")
        set_pavetext(pt)
        pt.Draw()
        pt.AddText(leg_title)
        pt.AddText('D^{0}#rightarrowK^{-}#pi^{+}')
        pt.AddText('D^{0}#rightarrowK^{-}#pi^{+}#pi^{0}')
        pt.AddText('D^{0}#rightarrowK^{-}#pi^{+}#pi^{+}#pi^{-}')

    mbc.SaveAs('./figs/rm_Dpi_'+str(ecms)+'_'+D_sample+'.pdf')

    # raw_input('Press <Enter> to end...')

if __name__ == '__main__':
    args = sys.argv[1:]
    if len(args)<2:
        usage()
        sys.exit()
    ecms = int(args[0])
    D_sample = args[1]

    path =[]
    path.append('/besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/anaroot/data/'+str(ecms)+'/data_'+str(ecms)+'_DstDpi_'+D_sample+'_before.root')
    leg_title = str(ecms) + ' MeV'
    xmin = 1.75
    xmax = 2.15
    xbins = 80
    plot(path, leg_title, ecms, xmin, xmax, xbins, D_sample)
