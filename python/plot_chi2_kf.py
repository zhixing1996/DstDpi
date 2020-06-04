#!/usr/bin/env python
"""
Plot chi2 of D and 2C kinematic fit
"""

__author__ = "Maoqiang JING <jingmq@ihep.ac.cn>"
__copyright__ = "Copyright (c) Maoqiang JING"
__created__ = "[2019-09-05 Thu 00:04]"

import ROOT
from ROOT import TCanvas, gStyle
from ROOT import TFile, TH1F, TLegend
import sys, os
import logging
from math import *
from tools import convert_name, width_rawm_D, width_rm_Dpi
logging.basicConfig(level=logging.DEBUG, format=' %(asctime)s - %(levelname)s- %(message)s')
gStyle.SetOptTitle(0)
gStyle.SetOptTitle(0)

def usage():
    sys.stdout.write('''
NAME
    plot_chi2_kf.py

SYNOPSIS
    ./plot_chi2_kf.py [ecms] [mode] [D_sample] [D_type]

AUTHOR
    Maoqiang JING <jingmq@ihep.ac.cn>

DATE
    September 2019
\n''')

def set_legend(legend, h1, h2, title):
    legend.AddEntry(h1, 'data')
    legend.AddEntry(h2, 'D_{2}(2460)D')
    legend.SetHeader(title)
    legend.SetBorderSize(0)
    legend.SetFillColor(0)
    legend.SetLineColor(0)

def chi2_kf_fill(t1, t2, entries1, entries2, h1, h2, ecms, mode, D_sample, D_type, M_D1, M_D2):
    for ientry1 in xrange(entries1):
        t1.GetEntry(ientry1)
        if fabs(t1.m_rawm_D - M_D1) < width_rawm_D(D_sample)/2. and mode == 'STD':
            h1.Fill(t1.m_chi2_kf)
        if fabs(t1.m_rawm_D - M_D1) < width_rawm_D(D_sample)/2. and fabs(t1.m_rm_Dpi - M_D2) < width_rm_Dpi(D_sample, D_type)/2. and mode == 'STDDmiss':
            h1.Fill(t1.m_chi2_kf)
    for ientry2 in xrange(entries2):
        t2.GetEntry(ientry2)
        if fabs(t2.m_rawm_D - M_D1) < width_rawm_D(D_sample)/2. and mode == 'STD':
            h2.Fill(t2.m_chi2_kf)
        if fabs(t2.m_rawm_D - M_D1) < width_rawm_D(D_sample)/2. and fabs(t2.m_rm_Dpi - M_D2) < width_rm_Dpi(D_sample, D_type)/2. and mode == 'STDDmiss':
            h2.Fill(t2.m_chi2_kf)

def set_histo_style(h1, h2, xtitle, ytitle):
    h1.GetXaxis().SetNdivisions(509)
    h1.GetYaxis().SetNdivisions(504)
    h1.SetLineWidth(2)
    h2.SetLineWidth(2)
    h1.SetStats(0)
    h2.SetStats(0)
    h1.GetXaxis().SetTitleSize(0.06)
    h1.GetXaxis().SetTitleOffset(1.01)
    h1.GetXaxis().SetLabelOffset(0.01)
    h1.GetYaxis().SetTitleSize(0.06)
    h1.GetYaxis().SetTitleOffset(1.1)
    h1.GetYaxis().SetLabelOffset(0.01)
    h1.GetXaxis().SetTitle(xtitle)
    h1.GetXaxis().CenterTitle()
    h1.GetYaxis().SetTitle(ytitle)
    h1.GetYaxis().CenterTitle()
    h1.SetLineColor(1)
    h2.SetLineColor(2)

def set_canvas_style(mbc):
    mbc.SetFillColor(0)
    mbc.SetLeftMargin(0.15)
    mbc.SetRightMargin(0.15)
    mbc.SetTopMargin(0.1)
    mbc.SetBottomMargin(0.15)

def plot(data_path, MC_path, leg_title, ecms, mode, D_sample, D_type):
    try:
        f_data = TFile(data_path)
        f_MC = TFile(MC_path)
        t_data = f_data.Get('save')
        t_MC = f_MC.Get('save')
        entries_data = t_data.GetEntries()
        entries_MC = t_MC.GetEntries()
        logging.info('data entries :'+str(entries_data))
        logging.info('signal MC(D2_2460) entries :'+str(entries_MC))
    except:
        logging.error(data_path+' or '+MC_path+' is invalid!')
        sys.exit()

    mbc = TCanvas('mbc', 'mbc', 800, 600)
    set_canvas_style(mbc)
    xmin = 0
    xmax = 40
    xbins = 40
    xtutle = ''
    M_D1 = 0.
    M_D2 = 0.
    if D_sample == 'Dplus':
        M_D1 = 1.86965
        M_D2 = 1.86483
    if D_sample == 'D0':
        M_D1 = 1.86483
        M_D2 = 1.86965

    if mode == 'STD':
        if D_sample == 'Dplus':
            xtitle = "#chi^{2}(K^{-}#pi^{+}#pi^{+})"
        if D_sample == 'D0':
            xtitle = "#chi^{2}(K^{-}#pi^{+} or K^{-}#pi^{+}#pi^{0} or K^{-}#pi^{+}#pi^{+}#pi^{-})"
    if mode == 'STDDmiss':
        if D_sample == 'Dplus':
            xtitle = "#chi^{2}(D^{+}#bar{D_{missing}^{0}}#pi^{-}_{0})"
        if D_sample == 'D0':
            xtitle = "#chi^{2}(D^{0}D_{missing}^{-}#pi^{+}_{0})"
    ytitle = 'Events'
    h_data = TH1F('data', 'data', xbins, xmin, xmax)
    h_MC = TH1F('MC', 'MC', xbins, xmin, xmax)

    set_histo_style(h_data, h_MC, xtitle, ytitle)
    chi2_kf_fill(t_data, t_MC, entries_data, entries_MC, h_data, h_MC, ecms, mode, D_sample, D_type, M_D1, M_D2)
    
    if not os.path.exists('./figs/'):
        os.makedirs('./figs/')
    
    if mode == 'STD':
        h_MC.Scale(0.2)
    if mode == 'STDDmiss':
        h_MC.Scale(0.04)
    h_data.Draw('ep')
    h_MC.Draw('same')

    legend = TLegend(0.65, 0.6, 0.82, 0.8)
    set_legend(legend, h_data, h_MC, leg_title)
    legend.Draw()

    if mode == 'STD':
        mbc.SaveAs('./figs/chi2_kf_'+str(ecms)+'_'+mode+'_'+D_sample+'.pdf')
    if mode == 'STDDmiss':
        mbc.SaveAs('./figs/chi2_kf_'+str(ecms)+'_'+mode+'_'+D_sample+'_'+D_type+'.pdf')

    # raw_input('Enter anything to end...')

if __name__ == '__main__':
    args = sys.argv[1:]
    if len(args)<4:
        usage()
        sys.exit()
    ecms = int(args[0])
    mode = args[1]
    D_sample = args[2]
    D_type = args[3]

    if mode == 'STD':
        data_path = '/besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/anaroot/data/'+str(ecms)+'/data_'+str(ecms)+'_DstDpi_'+D_sample+'.root'
        MC_path = '/besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/anaroot/mc/D2_2460/'+str(ecms)+'/mc_D2_2460_'+str(ecms)+'_DstDpi_'+D_sample+'.root'
        leg_title = str(ecms) + ' MeV'
        plot(data_path, MC_path, leg_title, ecms, mode, D_sample, D_type)
    if mode == 'STDDmiss':
        data_path = '/besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/anaroot/data/'+str(ecms)+'/data_'+str(ecms)+'_DstDpi_'+D_sample+'_'+D_type+'_signal.root'
        MC_path = '/besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/anaroot/mc/D2_2460/'+str(ecms)+'/mc_D2_2460_'+str(ecms)+'_DstDpi_'+D_sample+'_'+D_type+'_signal.root'
        leg_title = str(ecms) + ' MeV'
        plot(data_path, MC_path, leg_title, ecms, mode, D_sample, D_type)
