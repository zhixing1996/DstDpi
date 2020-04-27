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
    ./plot_rm_Dpi.py [ecms] [D_type]

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

def rm_Dpi_fill(t, h, D_type):
    for ientry in xrange(t.GetEntries()):
        t.GetEntry(ientry)
        if D_type == 'Dplus':
            if t.m_mode == 200:
                h.Fill(t.m_rm_Dpi)
        if D_type == 'D0':
            if t.m_mode == 0 or t.m_mode == 1 or t.m_mode == 3:
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

def plot(data_path, leg_title, ecms, xmin, xmax, xbins, D_type):
    try:
        f_data = TFile(data_path)
        t_data = f_data.Get('save')
        entries_data = t_data.GetEntries()
        logging.info('data entries :'+str(entries_data))
    except:
        logging.error(data_path + ' is invalid!')
        sys.exit()

    mbc = TCanvas('mbc', 'mbc', 800, 600)
    set_canvas_style(mbc)
    content = (xmax - xmin)/xbins * 1000
    ytitle = 'Events/%.1f MeV'%content
    xtitle = ''
    if D_type == 'Dplus':
        xtitle = 'RM(D^{+}#pi^{-}_{0})(GeV)'
    if D_type == 'D0':
        xtitle = 'RM(D^{0}#pi^{+}_{0})(GeV)'
    h_data = TH1F('data', 'data', xbins, xmin, xmax)
    
    set_histo_style(h_data, xtitle, ytitle)
    rm_Dpi_fill(t_data, h_data, D_type)
    
    if not os.path.exists('./figs/'):
        os.makedirs('./figs/')
    
    h_data.Draw('E1')

    if D_type == 'Dplus':
        pt = TPaveText(0.2, 0.7, 0.45, 0.85, "BRNDC")
        set_pavetext(pt)
        pt.Draw()
        pt.AddText(leg_title)
        pt.AddText('D^{+}#rightarrowK^{-}#pi^{+}#pi^{+}')
    if D_type == 'D0':
        pt = TPaveText(0.2, 0.7, 0.45, 0.85, "BRNDC")
        set_pavetext(pt)
        pt.Draw()
        pt.AddText(leg_title)
        pt.AddText('D^{0}#rightarrowK^{-}#pi^{+}')
        pt.AddText('D^{0}#rightarrowK^{-}#pi^{+}#pi^{0}')
        pt.AddText('D^{0}#rightarrowK^{-}#pi^{+}#pi^{+}#pi^{-}')

    mbc.SaveAs('./figs/rm_Dpi_'+str(ecms)+'_'+D_type+'.pdf')

    raw_input('Enter anything to end...')

if __name__ == '__main__':
    args = sys.argv[1:]
    if len(args)<2:
        usage()
        sys.exit()
    ecms = int(args[0])
    D_type = args[1]

    data_path = '/besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/anaroot/data/' + str(ecms) + '/data_' + str(ecms) + '_DstDpi_raw.root'
    leg_title = str(ecms) + ' MeV'
    xmin = 1.8
    xmax = 2.1
    xbins = 150
    plot(data_path, leg_title, ecms, xmin, xmax, xbins, D_type)
