//
// Created by eugene on 8/3/19.
//

#ifndef JAMREADER_GENMODEL_H
#define JAMREADER_GENMODEL_H

#include <iostream>
#include <Rtypes.h>
#include <TObject.h>
#include <TDatabasePDG.h>
#include <TLorentzVector.h>
#include <TClass.h>
#include <TBuffer.h>
#include <iomanip>

class TGenRunHeader;
class TGenEventHeader;
class TGenParticle;

enum class EObjectStatus {
  kNotInitialized,
  kOk,
};

class TGenRunHeader : public TObject {

 public:
  unsigned int nEvents{0};

 ClassDef(TGenRunHeader, 1)
};

class TGenEventHeader : public TObject {

 public:

  unsigned int getEventId() const {
    return eventId;
  }
  UInt_t getNBaryons() const {
    return nBaryons;
  }
  UInt_t getNMesons() const {
    return nMesons;
  }
  Float_t getImpactParameter() const {
    return impactParameter;
  }
  UInt_t getNParticipants() const {
    return nParticipants;
  }
  UInt_t getNCollisions() const {
    return nCollisions;
  }
  const std::vector<TGenParticle> &getParticles() const {
    return particles;
  }
  UInt_t getNParticles() const {
    return nParticles;
  }

  EObjectStatus getStatus() const {
    return status;
  }

  void print() const {
    using std::cout;
    using std::endl;
    using std::setw;

    cout
      << "Event #" << setw(3) << eventId << " "
      << "b = " << setw(5) << impactParameter << " (fm) "
      << "Npa = " << setw(4) << particles.size()
      << endl;
  }

  void clear() {
    eventId = 0;
    nBaryons = 0;
    nMesons = 0;
    impactParameter = -1;
    nParticipants = 0;
    nCollisions = 0;
    particles.clear();
    nParticles = 0;
    status = EObjectStatus::kNotInitialized;
  }

 private:
  friend void parseEventHeader(const std::string &line);
  friend void addCurrentParticleToEvent();

  unsigned int eventId{0}; // event index

  UInt_t nBaryons{0}; // number of baryons
  UInt_t nMesons{0}; // number of mesons

  Float16_t impactParameter{-1}; // impact parameter (fm)

  UInt_t nParticipants{0}; // number of participants
  UInt_t nCollisions{0}; // number of collisions

  std::vector<TGenParticle> particles; // list of particles

  UInt_t nParticles{0};//! transient field: duplicates particles.size()
  EObjectStatus status{EObjectStatus::kNotInitialized}; //!

 ClassDef(TGenEventHeader, 1)

};

class TGenParticle : public TObject {

 public:

  int getPar0() const {
    return par0;
  }
  int getPdgCode() const {
    return pdgCode;
  }
  int getPar2() const {
    return par2;
  }
  Float_t getMass() const {
    return mass;
  }
  Float_t getPx() const {
    return px;
  }
  Float_t getPy() const {
    return py;
  }
  Float_t getPz() const {
    return pz;
  }
  Float_t getEnergy() const {
    return energy;
  }
  const TLorentzVector &getMomentum() const {
    return momentum;
  }
  Float_t getX() const {
    return x;
  }
  Float_t getY() const {
    return y;
  }
  Float_t getZ() const {
    return z;
  }
  Float_t getTime() const {
    return time;
  }
  const TLorentzVector &getPosition() const {
    return position;
  }

  void print() const {
    using std::cout;
    using std::endl;
    using std::setw;
    cout << "\t"
    << setw(6) << pdgCode << " "
    << "p = (" << px << "," << py << ", " << pz << ")"
    << endl;
  }

 private:
  friend void parseParticle(const std::string &);

  int par0{0};
  int pdgCode{0};
  int par2{0};
  Float16_t mass{0.};

  Float16_t px{0.};
  Float16_t py{0.};
  Float16_t pz{0.};
  Float16_t energy{0.};

  TLorentzVector momentum; //!

  Float16_t x{0.};
  Float16_t y{0.};
  Float16_t z{0.};
  Float16_t time{0.};

  TLorentzVector position; //!



 ClassDef(TGenParticle, 1)
};

#endif //JAMREADER_GENMODEL_H
