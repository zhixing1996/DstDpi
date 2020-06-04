#!/usr/bin/env python
"""
Plot recoiling mass of tagged pi
"""

__author__ = "Maoqiang JING <jingmq@ihep.ac.cn>"
__copyright__ = "Copyright (c) Maoqiang JING"
__created__ = "[2020-04-30 Thr 22:37]"

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
    plot_rm_pi.py

SYNOPSIS
    ./plot_rm_pi.py [ecms] [D_sample] [D_type]

AUTHOR
    Maoqiang JING <jingmq@ihep.ac.cn>

DATE
    April 2020
\n''')

def set_legend(legend, h1, h2, h3):
    legend.AddEntry(h1, 'data: signal')
    legend.AddEntry(h2, 'data: WS background')
    legend.AddEntry(h3, 'MC: D_{2}(2460)#bar{D}')
    legend.SetBorderSize(0)
    legend.SetFillColor(0)
    legend.SetLineColor(0)

def set_pavetext(pt):
    pt.SetFillStyle(0)
    pt.SetBorderSize(0)
    pt.SetTextAlign(10)
    pt.SetTextSize(0.03)

def rm_pi_fill(t, h, D_sample, D_type):
    for ientry in xrange(t.GetEntries()):
        t.GetEntry(ientry)
        if D_sample == 'Dplus' and D_type == 'D':
            if t.m_rm_D > 2.03:
                h.Fill(t.m_rm_pi)
        elif D_sample == 'D0':
            if t.m_m_Dpi > 2.03:
                h.Fill(t.m_rm_pi)
        else:
            h.Fill(t.m_rm_pi)

def set_histo_style(h, xtitle, ytitle, color, fill_style):
    h.GetXaxis().SetNdivisions(509)
    h.GetYaxis().SetNdivisions(504)
    h.SetLineWidth(2)
    h.SetLineWidth(2)
    h.SetStats(0)
    h.SetStats(0)
    if not fill_style == -1:
        h.SetFillStyle(fill_style) 
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

def set_canvas_style(mbc):
    mbc.SetFillColor(0)
    mbc.SetLeftMargin(0.15)
    mbc.SetRightMargin(0.15)
    mbc.SetTopMargin(0.1)
    mbc.SetBottomMargin(0.15)

def plot(path, leg_title, ecms, xmin, xmax, xbins, D_sample, D_type):
    try:
        f_data = TFile(path[0])
        t_data = f_data.Get('save')
        entries_data = t_data.GetEntries()
        logging.info('data entries :'+str(entries_data))
    except:
        logging.error(path[0] + ' is invalid!')
        sys.exit()
    try:
        f_background = TFile(path[1])
        t_background = f_background.Get('save')
        entries_background = t_background.GetEntries()
        logging.info('data(background) entries :'+str(entries_background))
    except:
        logging.error(path[1] + ' is invalid!')
        sys.exit()
    try:
        f_D2 = TFile(path[2])
        t_D2 = f_D2.Get('save')
        entries_D2 = t_D2.GetEntries()
        logging.info('D2(2460)D entries :'+str(entries_D2))
    except:
        logging.error(path[2] + ' is invalid!')
        sys.exit()

    mbc = TCanvas('mbc', 'mbc', 800, 600)
    set_canvas_style(mbc)
    content = (xmax - xmin)/xbins * 1000
    ytitle = 'Events/%.1f MeV'%content
    xtitle = ''
    if D_sample == 'Dplus':
        xtitle = 'RM(#pi_{0}^{-})(GeV)'
    if D_sample == 'D0':
        xtitle = 'RM(#pi_{0}^{+})(GeV)'
    h_data = TH1F('data', 'data', xbins, xmin, xmax)
    
    set_histo_style(h_data, xtitle, ytitle, 1, -1)
    rm_pi_fill(t_data, h_data, D_sample, D_type)
    h_background = TH1F('background', 'background', xbins, xmin, xmax)
    set_histo_style(h_background, xtitle, ytitle, 3, 3004)
    rm_pi_fill(t_background, h_background, D_sample, D_type)
    h_D2 = TH1F('D2', 'D2', xbins, xmin, xmax)
    set_histo_style(h_D2, xtitle, ytitle, 2, 3003)
    rm_pi_fill(t_D2, h_D2, D_sample, D_type)
    
    path_nbkg = './txts/nbkg_'+str(ecms)+'_'+D_sample+'_'+D_type+'.txt'
    f_nbkg = open(path_nbkg, 'r')
    for line in f_nbkg.readlines():
        rs = line.rstrip('\n')
        rs = filter(None, rs.split(' '))
        nbkg = float(rs[0])
    h_background.Scale(nbkg/entries_background)
    h_D2.Scale(0.1)
    h_data.Draw('E1')
    hs = THStack('hs', 'Stacked')
    hs.Add(h_background)
    hs.Add(h_D2)
    hs.Draw('same')
    h_data.Draw('sameE1')

    if D_sample == 'Dplus':
        pt = TPaveText(0.7, 0.7, 0.85, 0.85, "BRNDC")
        set_pavetext(pt)
        pt.Draw()
        pt.AddText(leg_title)
        if D_type == 'D':
            pt.AddText('e^{+}e^{-}#rightarrowD^{+}#pi_{0}^{-}#bar{D^{0}}')
        if D_type == 'Dst':
            pt.AddText('e^{+}e^{-}#rightarrowD^{+}#pi_{0}^{-}#bar{D^{*0}}')
        pt.AddText('D^{+}#rightarrowK^{-}#pi^{+}#pi^{+}')
    if D_sample == 'D0':
        pt = TPaveText(0.17, 0.6, 0.4, 0.85, "BRNDC")
        set_pavetext(pt)
        pt.Draw()
        pt.AddText(leg_title)
        if D_type == 'D':
            pt.AddText('e^{+}e^{-}#rightarrowD^{0}#pi_{0}^{+}D^{-}')
        if D_type == 'Dst':
            pt.AddText('e^{+}e^{-}#rightarrowD^{0}#pi_{0}^{+}D^{*-}')
        pt.AddText('D^{0}#rightarrowK^{-}#pi^{+}')
        pt.AddText('D^{0}#rightarrowK^{-}#pi^{+}#pi^{0}')
        pt.AddText('D^{0}#rightarrowK^{-}#pi^{+}#pi^{+}#pi^{-}')

    legend = TLegend(0.2, 0.7, 0.35, 0.85)
    set_legend(legend, h_data, h_background, h_D2)
    legend.Draw()

    mbc.SaveAs('./figs/rm_pi_'+str(ecms)+'_'+D_sample+'_'+D_type+'.pdf')

    # raw_input('Press <Enter> to end...')

if __name__ == '__main__':
    args = sys.argv[1:]
    if len(args)<3:
        usage()
        sys.exit()
    ecms = int(args[0])
    D_sample = args[1]
    D_type = args[2]

    path =[]
    path.append('/besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/anaroot/data/'+str(ecms)+'/data_'+str(ecms)+'_DstDpi_'+D_sample+'_'+convert_name(D_sample, D_type)+'_signal_before.root')
    path.append('/besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/anaroot/data/'+str(ecms)+'/data_'+str(ecms)+'_DstDpi_'+D_sample+'_'+convert_name(D_sample, D_type)+'_background_before.root')
    path.append('/besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/anaroot/mc/D2_2460/'+str(ecms)+'/mc_D2_2460_'+str(ecms)+'_DstDpi_'+D_sample+'_'+convert_name(D_sample, D_type)+'_signal_before.root')
    leg_title = str(ecms) + ' MeV'
    if D_type == 'D':
        xmin = 3.6
        xmax = 4.6
        xbins = 200
    if D_type == 'Dst':
        xmin = 3.8
        xmax = 4.6
        xbins = 160
    plot(path, leg_title, ecms, xmin, xmax, xbins, D_sample, D_type)
