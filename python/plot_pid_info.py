#!/usr/bin/env python
"""
Plot pid info of charged tracks
"""

__author__ = "Maoqiang JING <jingmq@ihep.ac.cn>"
__copyright__ = "Copyright (c) Maoqiang JING"
__created__ = "[2020-05-24 Sun 16:49]"

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
    plot_pid_info.py

SYNOPSIS
    ./plot_rm_pi.py [ecms] [D_sample] [D_type] [track]

AUTHOR
    Maoqiang JING <jingmq@ihep.ac.cn>

DATE
    May 2020
\n''')

def set_legend(legend, h_list, track):
    count = 0
    for h in h_list:
        if track == 'K':
            legend.AddEntry(h, 'K in D, mis identified as #pi')
        if track == 'pi':
            if count == 0:
                legend.AddEntry(h, '#pi_{1} in D, mis identified as K')
            if count == 1:
                legend.AddEntry(h, '#pi_{2} in D, mis identified as K')
            if count == 2:
                legend.AddEntry(h, '#pi_{0} out of D, mis identified as K')
            count += 1
    legend.SetBorderSize(0)
    legend.SetFillColor(0)
    legend.SetLineColor(0)

def set_pavetext(pt):
    pt.SetFillStyle(0)
    pt.SetBorderSize(0)
    pt.SetTextAlign(10)
    pt.SetTextSize(0.03)

def pid_prob_fill(t, h_list, track):
    count = 0
    for h in h_list:
        for ientry in xrange(t.GetEntries()):
            t.GetEntry(ientry)
            if track == 'K':
                h.Fill(t.m_prob_pi_DK)
            if track == 'pi':
                if count == 0:
                    h_list[0].Fill(t.m_prob_K_Dpi1)
                if count == 1:
                    h_list[1].Fill(t.m_prob_K_Dpi2)
                if count == 2:
                    h_list[2].Fill(t.m_prob_K_pi)
        count += 1

def set_histo_style(h_list, xtitle, ytitle):
    color = 1
    for h in h_list:
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
        h.SetLineColor(color)
        color += 1

def set_canvas_style(mbc):
    mbc.SetFillColor(0)
    mbc.SetLeftMargin(0.15)
    mbc.SetRightMargin(0.15)
    mbc.SetTopMargin(0.1)
    mbc.SetBottomMargin(0.15)

def plot(path, leg_title, ecms, xmin, xmax, xbins, track):
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
    ytitle = 'Events'
    xtitle = 'Prob'
    h_list = []
    if track == 'K':
        h_list.append(TH1F('D_K_2_pi', 'D_K_2_pi', xbins, xmin, xmax))
    if track == 'pi':
        h_list.append(TH1F('D_pi1_2_K', 'D_pi1_2_K', xbins, xmin, xmax))
        h_list.append(TH1F('D_pi2_2_K', 'D_pi2_2_K', xbins, xmin, xmax))
        h_list.append(TH1F('pi_2_K', 'pi_2_K', xbins, xmin, xmax))
    set_histo_style(h_list, xtitle, ytitle)
    pid_prob_fill(t_data, h_list, track)
    
    if not os.path.exists('./figs/'):
        os.makedirs('./figs/')

    count = 0
    for h in h_list:
        if count == 0:
            h.Draw()
        else:
            h.Draw('same')
        count += 1

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

    legend = TLegend(0.2, 0.5, 0.55, 0.85)
    set_legend(legend, h_list, track)
    legend.Draw()

    mbc.SaveAs('./figs/pid_info_'+str(ecms)+'_'+D_sample+'_'+D_type+'_'+track+'.pdf')

    # raw_input('Press <Enter> to end...')

if __name__ == '__main__':
    args = sys.argv[1:]
    if len(args)<4:
        usage()
        sys.exit()
    ecms = int(args[0])
    D_sample = args[1]
    D_type = args[2]
    track = args[3]

    path =[]
    path.append('/besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/anaroot/data/'+str(ecms)+'/data_'+str(ecms)+'_DstDpi_'+D_sample+'_'+convert_name(D_sample, D_type)+'_signal_before.root')
    leg_title = str(ecms) + ' MeV'
    xmin = 0
    xmax = 1
    xbins = 200
    plot(path, leg_title, ecms, xmin, xmax, xbins, track)
