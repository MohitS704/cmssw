#ifndef __PFClusterCollectionMerger__
#define __PFClusterCollectionMerger__

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/global/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/ParticleFlowReco/interface/PFCluster.h"
#include "DataFormats/ParticleFlowReco/interface/PFClusterFwd.h"

class PFClusterCollectionMerger : public edm::global::EDProducer<> {
public:
  PFClusterCollectionMerger(const edm::ParameterSet& conf) {
    const std::vector<edm::InputTag>& inputs = conf.getParameter<std::vector<edm::InputTag>>("inputs");
    for (const auto& input : inputs) {
      _inputs.push_back(consumes<reco::PFClusterCollection>(input));
    }
    produces<reco::PFClusterCollection>();
  }

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<std::vector<edm::InputTag>>("inputs", {});
    descriptions.addWithDefaultLabel(desc);
  }

  void produce(edm::StreamID, edm::Event& e, const edm::EventSetup& es) const override {
    auto output = std::make_unique<reco::PFClusterCollection>();
    for (const auto& input : _inputs) {
      edm::Handle<reco::PFClusterCollection> handle;
      e.getByToken(input, handle);
      output->insert(output->end(), handle->begin(), handle->end());
    }
    e.put(std::move(output));
  }

private:
  std::vector<edm::EDGetTokenT<reco::PFClusterCollection>> _inputs;
};

DEFINE_FWK_MODULE(PFClusterCollectionMerger);

#endif
