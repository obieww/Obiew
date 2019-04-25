#include "paxos-stubs-map.h"

namespace obiew {

using PaxosStubs = std::map<std::string, std::unique_ptr<MultiPaxos::Stub>>;

std::string PaxosStubsMap::GetCoordinator() {
  std::shared_lock<std::shared_mutex> reader_lock(coordinator_mtx_);
  return coordinator_;
}

bool PaxosStubsMap::SetCoordinator(const std::string& coordinator) {
  {
    std::shared_lock<std::shared_mutex> reader_lock(stubs_mtx_);
    if (stubs_.find(coordinator) == stubs_.end()) return false;
  }
  {
    std::unique_lock<std::shared_mutex> writer_lock(coordinator_mtx_);
    coordinator_ = coordinator;
  }
  return true;
}

MultiPaxos::Stub* PaxosStubsMap::GetCoordinatorStub() {
  std::shared_lock<std::shared_mutex> reader_lock(stubs_mtx_);
  if (stubs_.find(coordinator_) == stubs_.end()) return nullptr;
  return stubs_[coordinator_].get();
}

MultiPaxos::Stub* PaxosStubsMap::GetStub(const std::string& address) {
  std::shared_lock<std::shared_mutex> coordinator_reader_lock(coordinator_mtx_);
  std::shared_lock<std::shared_mutex> stubs_reader_lock(stubs_mtx_);
  if (address.empty() || stubs_.find(address) == stubs_.end()) return nullptr;
  return stubs_[address].get();
}

std::map<std::string, MultiPaxos::Stub*> PaxosStubsMap::GetPaxosStubs() {
  std::map<std::string, MultiPaxos::Stub*> paxos_stubs;
  std::shared_lock<std::shared_mutex> reader_lock(stubs_mtx_);
  for (const auto& kv : stubs_) {
    paxos_stubs[kv.first] = kv.second.get();
  }
  return paxos_stubs;
}

}  // namespace obiew