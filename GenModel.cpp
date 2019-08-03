//
// Created by eugene on 8/3/19.
//

#include "GenModel.h"

ClassImp(TGenRunHeader)

ClassImp(TGenEventHeader)

ClassImp(TGenParticle)

void TGenParticle::Streamer(TBuffer &R__b) {
  if (R__b.IsReading()) {
    TGenParticle::Class()->ReadBuffer(R__b, this);
    momentum.SetPxPyPzE(px, py, pz, energy);
    position.SetXYZT(x, y, z, time);
  } else {
    TGenParticle::Class()->WriteBuffer(R__b, this);
  }
}

void TGenEventHeader::Streamer(TBuffer &R__b) {
  if (R__b.IsReading()) {
    TGenEventHeader::Class()->ReadBuffer(R__b, this);
    nParticles = particles.size();
  } else {
    TGenEventHeader::Class()->WriteBuffer(R__b, this);
  }
}
