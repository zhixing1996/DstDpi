//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of models developed at BES collaboration
//      based on the EvtGen framework.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information: See EvtGen/BesCopyright
//      Copyright (A) 2006      Ping Rong-Gang
//
// Module: Modified DIY model, see EvtDeay.cc 
//
// Description: Routine to sample the decays using the amplitude specified by users
// This model allow use to specify the intermediate states
//
// Modification history:
//
// Ping R.-G.    4-25, 2010       Module created
//
//------------------------------------------------------------------------
#ifndef EVTDECAY_H
#define EVTDECAY_H

#include "EvtDecay.h"
#include "EvtGenBase/EvtVector4R.hh"
#include "EvtGenBase/EvtVector3R.hh"
#include "EvtGenBase/EvtHelSys.hh"
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtParticleFactory.hh"
#include "EvtGen.hh"
#include "EvtGenBase/EvtRandomEngine.hh"
#include "EvtGenBase/EvtDecayTable.hh"

#include <stdlib.h>
#include <string.h>
using std::cout;
using std::endl;

double EvtDecay::CalAmpsMDIY(EvtParticle* part ) {

    //---- read out the momentum for each particle, then using them to calculate the amplitude squared, for example:
    /*********** using the following decay cards ****************
    Decay psi(4260)
    1 D2 D PHSP;
    Enddecay
    
    Decay D2
    1 D-bar pi PHSP;
    Enddecay
    ************************************************************/

    EvtVector4R ppsi4260, pD2, pD, ppi;
    ppsi4260 = part->getP4(); // psi(4260) momentum
    pD2      = part->getDaug(0)->getP4(); // D2 momentum
    pD       = part->getDaug(1)->getP4(); // D momentum
    ppi      = part->getDaug(0)->getDaug(1)->getP4(); // pi from D2

    EvtHelSys angles0(ppsi4260, pD);
    double theta = angles0.getHelAng(1);

    EvtHelSys angles1(pD2, ppi);
    double thetap = angles1.getHelAng(1);
    double phip   = angles1.getHelAng(2);

    double amps=(1 + cos(theta)*cos(theta) - sin(theta)*sin(theta)*cos(2*phip))*(3./8.)*sin(2*thetap)*sin(2*thetap);

    ///////////======== don't touch follows =======================
    if (amps <=0) {
        report(INFO,"EvtGen") << "Amplitude square of modified DIY should be positive, but found to be equal "<<amps<<endl;
        abort();
    } else {
        return amps;
    }
}

#endif
