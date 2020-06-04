#!/usr/bin/env python
"""
Fit to recoiling mass of Dpi
"""

__author__ = "Maoqiang JING <jingmq@ihep.ac.cn>"
__copyright__ = "Copyright (c) Maoqiang JING"
__created__ = "[2020-04-27 Mon 11:48]"

import math
from array import array
import sys, os
import logging
from math import *
from tools import luminosity, width_rm_Dpi
from ROOT import *
logging.basicConfig(level=logging.DEBUG, format=' %(asctime)s - %(levelname)s- %(message)s')

# set the paper & margin sizes
gStyle.SetPaperSize(20,26)
gStyle.SetPadColor(0)
gStyle.SetPadBorderMode(0)
gStyle.SetPadTopMargin(0.03)
gStyle.SetPadRightMargin(0.05)
gStyle.SetPadBottomMargin(0.22)
gStyle.SetPadLeftMargin(0.12)
gStyle.SetTitleFillColor(0)
gStyle.SetTitleFont(22, "xyz") # set the all 3 axes title font 
gStyle.SetTitleFont(22, " ") # set the pad title font
gStyle.SetTitleSize(0.06, "xyz") # set the 3 axes title size
gStyle.SetTitleSize(0.06, " ") # set the pad title size
gStyle.SetLabelFont(22, "xyz")
gStyle.SetLabelSize(0.05, "xyz")
gStyle.SetTextFont(22)
gStyle.SetTextSize(0.08)
gStyle.SetStatFont(22)
gStyle.SetFrameBorderMode(0)
gStyle.SetCanvasBorderMode(0)
gStyle.SetCanvasColor(0)
gStyle.SetStatColor(0)
gStyle.SetMarkerStyle(8)
gStyle.SetHistFillColor(0)
gStyle.SetLineWidth(2)
gStyle.SetLineStyleString(2,"[12 12]"); # postscript dashes
gStyle.SetErrorX(0.001)

# do not display any of the standard histogram decorations
gStyle.SetOptTitle(0)
gStyle.SetOptStat(0)
gStyle.SetOptFit(0)

# put tick marks on top and RHS of plots
gStyle.SetPadTickX(0)
gStyle.SetPadTickY(0)

def usage():
    sys.stdout.write('''
NAME
    fit_rm_Dpi.py

SYNOPSIS
    ./fit_rm_Dpi.py [ecms] [D_sample] [D_type]

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

def set_xframe_style(xframe, xtitle, ytitle):
    xframe.GetXaxis().SetTitle(xtitle)
    xframe.GetXaxis().SetTitleSize(0.06)
    xframe.GetXaxis().SetLabelSize(0.06)
    xframe.GetXaxis().SetTitleOffset(1.0)
    xframe.GetXaxis().SetLabelOffset(0.008)
    xframe.GetXaxis().SetNdivisions(508)
    xframe.GetXaxis().CenterTitle()
    xframe.GetYaxis().SetNdivisions(504)
    xframe.GetYaxis().SetTitleSize(0.06)
    xframe.GetYaxis().SetLabelSize(0.06)
    xframe.GetYaxis().SetTitleOffset(1.0)
    xframe.GetYaxis().SetLabelOffset(0.008)
    xframe.GetYaxis().SetTitle(ytitle)
    xframe.GetYaxis().CenterTitle()

def set_pad_style(pad):
    pad.SetLeftMargin(0.15)
    pad.SetRightMargin(0.15)
    pad.SetTopMargin(0.1)
    pad.SetBottomMargin(0.15)
    pad.SetFrameLineColor(kBlack)

def set_canvas_style(mbc):
    mbc.SetFillColor(0)
    mbc.SetLeftMargin(0.15)
    mbc.SetRightMargin(0.15)
    mbc.SetTopMargin(0.1)
    mbc.SetBottomMargin(0.15)
    mbc.SetGrid()

def fit(path, ecms, D_sample, D_type):
    try:
        f_data = TFile(path[0])
        t_data = f_data.Get('save')
        entries_data = t_data.GetEntries()
        logging.info('data entries :'+str(entries_data))
    except:
        logging.error(path[0] + ' is invalid!')

    mbc = TCanvas('mbc', 'mbc', 1000, 700)
    set_canvas_style(mbc)

    pad = TPad("pad", "pad", 0.0, 0.0, 1.0, 1.0)
    set_pad_style(pad)
    pad.Draw()

    if D_type == 'D':
        xmin = 1.75
        xmax = 1.95
        xbins = 100
    if D_type == 'Dst':
        xmin = 1.9
        xmax = 2.1
        xbins = 100

    rm_Dpi = RooRealVar('rm_Dpi', 'rm_Dpi', xmin, xmax)
    data = RooDataSet('data', 'dataset', t_data, RooArgSet(rm_Dpi))

    # signal
    if D_type == 'D':
        mean = RooRealVar('mean', 'mean of gaussian', 1.864, 1.85, 1.88)
        sigma = RooRealVar('sigma', 'sigma of gaussian', 0.001, 0, 0.01)
    if D_type == 'Dst':
        mean = RooRealVar('mean', 'mean of gaussian', 2.01026, 2.001, 2.02)
        sigma = RooRealVar('sigma', 'sigma of gaussian', 0.0000834, 0, 0.01)
    gauss = RooGaussian('gauss', 'gaussian', rm_Dpi, mean, sigma)

    # background
    a = RooRealVar('a', 'a', 0, -99, 99)
    b = RooRealVar('b', 'b', 0, -99, 99)
    c = RooRealVar('c', 'c', 0, -99, 99)
    bkgpdf = RooChebychev('bkgpdf', 'bkgpdf', rm_Dpi, RooArgList(a, b))

    # event number
    nsig = RooRealVar('nsig', 'nsig', 500, 0, 10000000)
    nbkg = RooRealVar('nbkg', 'nbkg', 1000, 0, 10000000)

    # fit model
    model = RooAddPdf('model', 'gauss+bkgpdf', RooArgList(gauss, bkgpdf), RooArgList(nsig, nbkg))
    model.fitTo(data)

    # plot results
    xframe = rm_Dpi.frame(RooFit.Bins(xbins), RooFit.Range(xmin, xmax))
    data.plotOn(xframe)
    model.plotOn(xframe)
    model.plotOn(xframe, RooFit.Components('gauss'), RooFit.LineColor(kRed), RooFit.LineWidth(2), RooFit.LineStyle(1))
    model.plotOn(xframe, RooFit.Components('bkgpdf'), RooFit.LineColor(kGreen), RooFit.LineWidth(2), RooFit.LineStyle(1))
    xtitle = ''
    if D_sample == 'Dplus':
        xtitle = 'RM(D^{+}#pi_{0}^{-})(GeV)'
    if D_sample == 'D0':
        xtitle = 'RM(D^{0}#pi_{0}^{+})(GeV)'
    content = (xmax - xmin)/xbins * 1000
    ytitle = 'Events/%.1f MeV'%content
    set_xframe_style(xframe, xtitle, ytitle)
    xframe.Draw()
    range = 'Mass Window: [' + str(round(mean.getVal() - 2*sigma.getVal(), 3)) + ', ' + str(round(mean.getVal() + 2*sigma.getVal(), 3)) + '] GeV'
    print range

    pt = TPaveText(0.2, 0.7, 0.35, 0.85, "BRNDC")
    set_pavetext(pt)
    pt.Draw()
    title_pt = str(ecms) + ' MeV'
    pt.AddText(title_pt)
    if D_sample == 'Dplus' and D_type == 'D':
        pt.AddText('e^{+}e^{-}#rightarrowD^{+}#pi_{0}^{-}#bar{D^{0}}')
    if D_sample == 'Dplus' and D_type == 'Dst':
        pt.AddText('e^{+}e^{-}#rightarrowD^{+}#pi_{0}^{-}#bar{D^{*0}}')
    if D_sample == 'D0' and D_type == 'D':
        pt.AddText('e^{+}e^{-}#rightarrowD^{0}#pi_{0}^{+}D^{-}')
    if D_sample == 'D0' and D_type == 'Dst':
        pt.AddText('e^{+}e^{-}#rightarrowD^{0}#pi_{0}^{+}D^{*-}')
    title_mean = 'Mean: ' + str(round(mean.getVal()*1000, 1) ) + ' MeV'
    title_sigma = 'Sigma: ' + str(round(sigma.getVal()*1000, 1) ) + ' MeV'
    pt.AddText(title_mean)
    pt.AddText(title_sigma)

    if not os.path.exists('./txts/'):
        os.makedirs('./txts/')
    path_num = './txts/events_'+str(ecms)+'_'+D_sample+'_'+D_type+'.txt'
    f_num = open(path_num, 'w')
    num = str(nsig.getVal()/luminosity(ecms)) + ' ' + str(nsig.getError()/luminosity(ecms)) + '\n'
    f_num.write(num)
    f_num.close()
    path_param = './txts/param_'+str(ecms)+'_'+D_sample+'_'+D_type+'.txt'
    f_param = open(path_param, 'w')
    param = str(mean.getVal()) + ' ' + str(sigma.getVal()) + '\n'
    f_param.write(param)
    f_param.write(range)
    f_param.close()

    rm_Dpi_low = 1.86483 - width_rm_Dpi(D_sample, D_type)/2.
    rm_Dpi_up = 1.86483 + width_rm_Dpi(D_sample, D_type)/2.
    rm_Dpi.setRange('srange', rm_Dpi_low, rm_Dpi_up)
    rm_Dpi.setRange('allrange', xmin, xmax)
    sbkg = bkgpdf.createIntegral(RooArgSet(rm_Dpi), RooFit.NormSet(RooArgSet(rm_Dpi)), RooFit.Range('srange'))
    allbkg = bkgpdf.createIntegral(RooArgSet(rm_Dpi), RooFit.NormSet(RooArgSet(rm_Dpi)), RooFit.Range('allrange'))
    n_bkg = sbkg.getVal()/allbkg.getVal() * (nbkg.getVal())

    path_nbkg = './txts/nbkg_'+str(ecms)+'_'+D_sample+'_'+D_type+'.txt'
    f_nbkg = open(path_nbkg, 'w')
    f_nbkg.write(str(n_bkg))
    f_nbkg.close()

    if not os.path.exists('./figs/'):
        os.makedirs('./figs/')

    mbc.SaveAs('./figs/fit_rm_Dpi_'+str(ecms)+'_'+D_sample+'_'+D_type+'.pdf')

    # raw_input('Press <Enter> to end...')

def main():
    args = sys.argv[1:]
    if len(args)<3:
        return usage()
    ecms = int(args[0])
    D_sample = args[1]
    D_type = args[2]

    path = []
    path.append('/besfs/groups/cal/dedx/$USER/bes/DstDpi/run/DstDpi/anaroot/data/'+str(ecms)+'/data_'+str(ecms)+'_DstDpi_'+D_sample+'_signal_missing_before.root')
    fit(path, ecms, D_sample, D_type)

if __name__ == '__main__':
    main()
