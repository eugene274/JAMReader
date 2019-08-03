#include <iostream>
#include <sstream>
#include <memory>

#include <TFile.h>
#include <TTree.h>
#include <fstream>

#include "GenModel.h"

namespace {
using std::ifstream;
using std::stringstream;
using std::string;
using std::cout;
using std::endl;
}

enum class ELineType {
  kRunHeader,
  kEventHeader,
  kParticle,
  kNotInitializedYet,
  kUnknown
};

enum class ELoggingVerbosity : short {
  kQuiet = 0,
  kNormal,
  kVerbose
};

ELineType gCurrentLineType = ELineType::kNotInitializedYet;
ELoggingVerbosity gLoggingVerbosity = ELoggingVerbosity::kNormal;

std::unique_ptr<TGenRunHeader> gCurrentRunHeader;
std::unique_ptr<TGenEventHeader> gCurrentEventHeader;
std::unique_ptr<TGenParticle> gCurrentParticle;

std::unique_ptr<TFile> gOutputFile;
TTree *gEventTree{nullptr};

unsigned int countWordsInTheString(const std::string &str) {
  std::stringstream stream(str);
  std::string word;
  unsigned int words = 0;
  while (stream >> word) {
    ++words;
  }
  return words;
}

void parseRunHeader(const string &line) {
  stringstream stream(line);

  string hashStr;
  stream >> hashStr;
  if (hashStr != "#") {
    throw std::runtime_error("Malformed run header: must start with hash");
  }

  if (!gCurrentRunHeader) {
    gCurrentRunHeader.reset(new TGenRunHeader);
  }

  stream >> gCurrentRunHeader->nEvents;
}

void parseEventHeader(const string &line) {
  stringstream stream(line);

  string hashStr;
  stream >> hashStr;
  if (hashStr != "#") {
    throw std::runtime_error("Malformed event header: must start with hash");
  }

  stream
      >> gCurrentEventHeader->eventId
      >> gCurrentEventHeader->nParticles
      >> gCurrentEventHeader->nBaryons
      >> gCurrentEventHeader->nMesons
      >> gCurrentEventHeader->impactParameter
      >> gCurrentEventHeader->nParticipants
      >> gCurrentEventHeader->nCollisions;

  gCurrentEventHeader->status = EObjectStatus::kOk;
}

void eventPostProcess() {

  if (!gCurrentEventHeader) {
    gCurrentEventHeader.reset(new TGenEventHeader);
    return;
  } else if (gCurrentEventHeader->getStatus() == EObjectStatus::kNotInitialized) {
    return;
  }

  if (gCurrentEventHeader->getNParticles() != gCurrentEventHeader->getParticles().size()) {
    throw std::logic_error("Number of particles in the event header mismatches with actual");
  }

  if (gLoggingVerbosity >= ELoggingVerbosity::kNormal) gCurrentEventHeader->print();

  if (gEventTree) {
    gEventTree->Fill();
  } else {
    Warning(__func__, "No tree was initialized");
  }

  gCurrentEventHeader->clear();

}

void parseParticle(const string &line) {
  stringstream stream(line);

  if (!gCurrentParticle) {
    gCurrentParticle.reset(new TGenParticle);
  }

  stream
      >> gCurrentParticle->par0
      >> gCurrentParticle->pdgCode
      >> gCurrentParticle->par2
      >> gCurrentParticle->mass
      >> gCurrentParticle->px
      >> gCurrentParticle->py
      >> gCurrentParticle->pz
      >> gCurrentParticle->energy
      >> gCurrentParticle->x
      >> gCurrentParticle->y
      >> gCurrentParticle->z
      >> gCurrentParticle->time;
}

void addCurrentParticleToEvent() {
  gCurrentEventHeader->particles.emplace_back(*gCurrentParticle);
}

void particlePostProcess() {
  if (gLoggingVerbosity >= ELoggingVerbosity::kVerbose) gCurrentParticle->print();

  if (gCurrentEventHeader) {
    addCurrentParticleToEvent();
  } else {
    throw std::logic_error("A particle was parsed but no event header is present");
  }
}

void parseLine(const string &line) {
  unsigned int nWords = countWordsInTheString(line);

  switch (nWords) {
    case 6:gCurrentLineType = ELineType::kRunHeader;
      parseRunHeader(line);
      break;
    case 9:gCurrentLineType = ELineType::kEventHeader;
      eventPostProcess();
      parseEventHeader(line);
      break;
    case 13:gCurrentLineType = ELineType::kParticle;
      parseParticle(line);
      particlePostProcess();
      break;
    default:gCurrentLineType = ELineType::kUnknown;
  }

}

int main(int argc, char **argv) {
  std::cout << "go!" << std::endl;

  if (argc < 2) {
    Warning(__func__, "No input file set. Exiting...");
    return 1;
  }

  ifstream inputFile(argv[1]);

  TGenEventHeader::Class()->IgnoreTObjectStreamer();
  TGenParticle::Class()->IgnoreTObjectStreamer();

  string outputFileName{"output.root"};
  if (argc < 3) {
    Warning(__func__, "Default output %s", outputFileName.c_str());
  } else {
    outputFileName = string(argv[2]);
  }

  gOutputFile.reset(TFile::Open(outputFileName.c_str(), "recreate"));

  if (gOutputFile) {
    gEventTree = new TTree("events", "");

    gCurrentEventHeader.reset(new TGenEventHeader);
    gEventTree->Branch("EventData", gCurrentEventHeader.get());
  }

  const size_t lineBufSize = 300;
  string line;
  while (std::getline(inputFile, line)) {
    parseLine(line);
    line[0] = '\0';
  }

  eventPostProcess();

  if (gEventTree) {
    gEventTree->Write();
  }
  if (gOutputFile) {
    gOutputFile->Close();
    gOutputFile.reset();
  }

  return 0;
}