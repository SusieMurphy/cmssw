#include "../interface/PresampleClient.h"

#include "DQM/EcalCommon/interface/EcalDQMCommonUtils.h"

#include "CondFormats/EcalObjects/interface/EcalDQMStatusHelper.h"

#include <cmath>

namespace ecaldqm {

  PresampleClient::PresampleClient(edm::ParameterSet const& _workerParams, edm::ParameterSet const& _commonParams) :
    DQWorkerClient(_workerParams, _commonParams, "PresampleClient"),
    minChannelEntries_(_workerParams.getUntrackedParameter<int>("minChannelEntries")),
    minTowerEntries_(_workerParams.getUntrackedParameter<int>("minTowerEntries")),
    expectedMean_(_workerParams.getUntrackedParameter<double>("expectedMean")),
    toleranceMean_(_workerParams.getUntrackedParameter<double>("toleranceMean")),
    toleranceRMS_(_workerParams.getUntrackedParameter<double>("toleranceRMS")),
    toleranceRMSFwd_(_workerParams.getUntrackedParameter<double>("toleranceRMSFwd"))
  {
  }

  void
  PresampleClient::beginRun(const edm::Run &, const edm::EventSetup &)
  {
    MEs_[kQuality]->resetAll(-1.);
    MEs_[kRMSMap]->resetAll(-1.);
    MEs_[kQualitySummary]->resetAll(-1.);

    MEs_[kQuality]->reset(kUnknown);
    MEs_[kQualitySummary]->reset(kUnknown);
  }

  void
  PresampleClient::producePlots()
  {
    MEs_[kMean]->reset();
    MEs_[kRMS]->reset();

    uint32_t mask(1 << EcalDQMStatusHelper::PEDESTAL_ONLINE_HIGH_GAIN_MEAN_ERROR |
		  1 << EcalDQMStatusHelper::PEDESTAL_ONLINE_HIGH_GAIN_RMS_ERROR);

    MESet::iterator qEnd(MEs_[kQuality]->end());

    MESet::const_iterator pItr(sources_[kPedestal]);
    double maxEB(0.), minEB(0.), maxEE(0.), minEE(0.);
    double rmsMaxEB(0.), rmsMaxEE(0.);
    for(MESet::iterator qItr(MEs_[kQuality]->beginChannel()); qItr != qEnd; qItr.toNextChannel()){

      pItr = qItr;

      DetId id(qItr->getId());

      bool doMask(applyMask_(kQuality, id, mask));

      double rmsThresh(toleranceRMS_);

      if(isForward(id)) rmsThresh = toleranceRMSFwd_;

      double entries(pItr->getBinEntries());

      if(entries < minChannelEntries_){
        qItr->setBinContent(doMask ? kMUnknown : kUnknown);
        MEs_[kQualitySummary]->setBinContent(id, doMask ? kMUnknown : kUnknown);
        MEs_[kRMSMap]->setBinContent(id, -1.);
        continue;
      }

      double mean(pItr->getBinContent());
      double rms(pItr->getBinError() * std::sqrt(entries));

      unsigned dccid(dccId(id));

      MEs_[kMean]->fill(dccid, mean);
      MEs_[kRMS]->fill(dccid, rms);
      MEs_[kRMSMap]->setBinContent(id, rms);

      if(std::abs(mean - expectedMean_) > toleranceMean_ || rms > rmsThresh){
        qItr->setBinContent(doMask ? kMBad : kBad);
        MEs_[kQualitySummary]->setBinContent(id, doMask ? kMBad : kBad);
      }
      else{
        qItr->setBinContent(doMask ? kMGood : kGood);
        MEs_[kQualitySummary]->setBinContent(id, doMask ? kMGood : kGood);
      }

      if(id.subdetId() == EcalBarrel){
        if(mean > maxEB) maxEB = mean;
        if(mean < minEB) minEB = mean;
        if(rms > rmsMaxEB) rmsMaxEB = rms;
      }
      else{
        if(mean > maxEE) maxEE = mean;
        if(mean < minEE) minEE = mean;
        if(rms > rmsMaxEE) rmsMaxEE = rms;
      }
    }

    if(online){
      MEs_[kTrendMean]->fill(unsigned(BinService::kEB + 1), double(iLumi), maxEB - minEB);
      MEs_[kTrendMean]->fill(unsigned(BinService::kEE + 1), double(iLumi), maxEE - minEE);
      MEs_[kTrendRMS]->fill(unsigned(BinService::kEB + 1), double(iLumi), rmsMaxEB);
      MEs_[kTrendRMS]->fill(unsigned(BinService::kEE + 1), double(iLumi), rmsMaxEE);
    }
  }

  /*static*/
  void
  PresampleClient::setMEOrdering(std::map<std::string, unsigned>& _nameToIndex)
  {
    _nameToIndex["Quality"] = kQuality;
    _nameToIndex["Mean"] = kMean;
    _nameToIndex["RMS"] = kRMS;
    _nameToIndex["RMSMap"] = kRMSMap;
    _nameToIndex["QualitySummary"] = kQualitySummary;
    _nameToIndex["TrendMean"] = kTrendMean;
    _nameToIndex["TrendRMS"] = kTrendRMS;

    _nameToIndex["Pedestal"] = kPedestal;
  }

  DEFINE_ECALDQM_WORKER(PresampleClient);
}

