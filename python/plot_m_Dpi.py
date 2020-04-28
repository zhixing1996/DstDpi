#!/usr/bin/env python
"""
Plot invariant mass of tagged pi and D
"""

__author__ = "Maoqiang JING <jingmq@ihep.ac.cn>"
__copyright__ = "Copyright (c) Maoqiang JING"
__created__ = "[2020-04-26 Sun 22:17]"

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
    plot_m_Dpi.py

SYNOPSIS
    ./plot_m_Dpi.py [ecms] [D_sample]

AUTHOR
    Maoqiang JING <jingmq@ihep.ac.cn>

DATE
    April 2020
\n''')

def set_legend(legend, h1, h2):
    legend.AddEntry(h1, 'data')
    legend.AddEntry(h2, 'sideband')
    legend.SetBorderSize(0)
    legend.SetFillColor(0)
    legend.SetLineColor(0)

def set_pavetext(pt):
    pt.SetFillStyle(0)
    pt.SetBorderSize(0)
    pt.SetTextAlign(10)
    pt.SetTextSize(0.03)

def m_Dpi_fill(t, h):
    for ientry in xrange(t.GetEntries()):
        t.GetEntry(ientry)
        h.Fill(t.m_m_Dpi)

def set_histo_style(h, xtitle, ytitle, color):
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
    h.SetFillColor(color)

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
    try:
        f_sideband = TFile(path[1])
        t_sideband = f_sideband.Get('save')
        entries_sideband = t_sideband.GetEntries()
        logging.info('data(sideband) entries :'+str(entries_sideband))
    except:
        logging.error(path[1] + ' is invalid!')
        sys.exit()

    mbc = TCanvas('mbc', 'mbc', 800, 600)
    set_canvas_style(mbc)
    mbc.cd()
    content = (xmax - xmin)/xbins * 1000
    ytitle = 'Events/%.1f MeV'%content
    xtitle = ''
    if D_sample == 'Dplus':
        xtitle = 'M(D^{+}#pi^{-}_{0})(GeV)'
    if D_sample == 'D0':
        xtitle = 'M(D^{0}#pi^{+}_{0})(GeV)'
    h_data = TH1F('data', 'data', xbins, xmin, xmax)
    set_histo_style(h_data, xtitle, ytitle, 1)
    m_Dpi_fill(t_data, h_data)
    h_sideband = TH1F('sideband', 'sideband', xbins, xmin, xmax)
    set_histo_style(h_sideband, xtitle, ytitle, 3)
    m_Dpi_fill(t_sideband, h_sideband)
    
    if not os.path.exists('./figs/'):
        os.makedirs('./figs/')
    
    if D_sample == 'Dplus':
        h_data.Draw('E1')
        h_sideband.Scale(0.5)
        h_sideband.Draw('same')
        legend = TLegend(0.3, 0.75, 0.5, 0.85)
        set_legend(legend, h_data, h_sideband)
        legend.Draw()
        pt = TPaveText(0.7, 0.7, 0.85, 0.85, "BRNDC")
        set_pavetext(pt)
        pt.Draw()
        pt.AddText(leg_title)
        pt.AddText('D^{+}#rightarrowK^{-}#pi^{+}#pi^{+}')

    if D_sample == 'D0':
        p1 = TPad("pad", "pad", 0.0, 0.0, 1.0, 1.0)
        set_pad_style(p1)
        p1.Draw()
        p2 = TPad("pad", "pad", 0.45, 0.25, 0.85, 0.75)
        set_pad_style(p2)
        p2.Draw()
        p1.cd()
        h_data.Draw('E1')
        h_sideband.Scale(0.5)
        h_sideband.Draw('same')
        legend = TLegend(0.6, 0.75, 0.9, 0.85)
        set_legend(legend, h_data, h_sideband)
        legend.Draw()
        pt = TPaveText(0.2, 0.7, 0.45, 0.85, "BRNDC")
        set_pavetext(pt)
        pt.Draw()
        pt.AddText(leg_title)
        pt.AddText('D^{0}#rightarrowK^{-}#pi^{+}')
        pt.AddText('D^{0}#rightarrowK^{-}#pi^{+}#pi^{0}')
        pt.AddText('D^{0}#rightarrowK^{-}#pi^{+}#pi^{+}#pi^{-}')
        h_data_sub = TH1F('data_sub', 'data_sub', 400, 2.02, 2.82)
        set_histo_style(h_data_sub, xtitle, ytitle, 1)
        m_Dpi_fill(t_data, h_data_sub)
        h_sideband_sub = TH1F('sideband_sub', 'sideband_sub', 400, 2.02, 2.82)
        set_histo_style(h_sideband_sub, xtitle, ytitle, 3)
        m_Dpi_fill(t_sideband, h_sideband_sub)
        p2.cd()
        h_data_sub.Draw('E1')
        h_sideband_sub.Scale(0.5)
        h_sideband_sub.Draw('same')

    mbc.SaveAs('./figs/m_Dpi_'+str(ecms)+'_'+D_sample+'.pdf')
    raw_input('Press <Enter> to end...')

if __name__ == '__main__':
    args = sys.argv[1:]
    if len(args)<2:
        usage()
        sys.exit()
    ecms = int(args[0])
    D_sample = args[1]

    path = []
    path.append('/besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/anaroot/data/' + str(ecms) + '/data_' + str(ecms) + '_DstDpi_'+D_sample+'_before.root')
    path.append('/besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/anaroot/data/' + str(ecms) + '/data_' + str(ecms) + '_DstDpi_'+D_sample+'_sideband_before.root')
    leg_title = str(ecms) + ' MeV'
    if D_sample == 'Dplus':
        xmin = 2.0
        xmax = 2.9
        xbins = 450
    if D_sample == 'D0':
        xmin = 2.0
        xmax = 2.9
        xbins = 450
    plot(path, leg_title, ecms, xmin, xmax, xbins, D_sample)
