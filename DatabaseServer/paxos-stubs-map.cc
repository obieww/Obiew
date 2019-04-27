#include "paxos-stubs-map.h"

namespace obiew {

using PaxosStubs = std::map<std::string, std::unique_ptr<MultiPaxos::Stub>>;

std::string PaxosStubsMap::GetCoordinator() {
  std::unique_lock<std::mutex> lock(coordinator_mtx_);
  return coordinator_;
}

bool PaxosStubsMap::SetCoordinator(const std::string& coordinator) {
  {
    std::unique_lock<std::mutex> lock(stubs_mtx_);
    if (stubs_.find(coordinator) == stubs_.end()) return false;
  }
  {
    std::unique_lock<std::mutex> writer_lock(coordinator_mtx_);
    coordinator_ = coordinator;
  }
  return true;
}

MultiPaxos::Stub* PaxosStubsMap::GetCoordinatorStub() {
  std::unique_lock<std::mutex> lock(stubs_mtx_);
  if (stubs_.find(coordinator_) == stubs_.end()) return nullptr;
  return stubs_[coordinator_].get();
}

MultiPaxos::Stub* PaxosStubsMap::GetStub(const std::string& address) {
  std::unique_lock<std::mutex> coordinator_lock(coordinator_mtx_);
  std::unique_lock<std::mutex> stubs_lock(stubs_mtx_);
  if (address.empty() || stubs_.find(address) == stubs_.end()) return nullptr;
  return stubs_[address].get();
}

std::map<std::string, MultiPaxos::Stub*> PaxosStubsMap::GetPaxosStubs() {
  std::map<std::string, MultiPaxos::Stub*> paxos_stubs;
  std::unique_lock<std::mutex> lock(stubs_mtx_);
  for (const auto& stub : stubs_) {
    paxos_stubs[stub.first] = stub.second.get();
  }
  return paxos_stubs;
}

// Returns the mapped Paxos log for given key & round.
// A new element will be constructed using its default constructor and inserted
// if key or round is not found.
PaxosLog PaxosStubsMap::GetPaxosLog(ProposalType type, int round) {
  std::unique_lock<std::mutex> lock(paxos_logs_mtx_);
  if (type == ProposalType::SQL) {
    return sql_paxos_logs_[round];
  } else {
    return cdnt_paxos_logs_[round];
  }
}

// Returns a copy of sql_paxos_logs_.
std::map<int, PaxosLog> PaxosStubsMap::GetSqlPaxosLogs() {
  return sql_paxos_logs_;
}

// Returns a copy of cdnt_paxos_logs_.
std::map<int, PaxosLog> PaxosStubsMap::GetCdntPaxosLogs() {
  return cdnt_paxos_logs_;
}

// Returns the latest Paxos round number for the given type.
int PaxosStubsMap::GetLatestRound(ProposalType type) {
  std::unique_lock<std::mutex> lock(paxos_logs_mtx_);
  if (type == ProposalType::SQL) {
    return sql_paxos_logs_.empty() ? 0 : sql_paxos_logs_.rbegin()->first;
  } else {
    return cdnt_paxos_logs_.empty() ? 0 : cdnt_paxos_logs_.rbegin()->first;
  }
}
// Add PaxosLog when Acceptor receives a proposal.
void PaxosStubsMap::AddPaxosLog(ProposalType type, int round) {
  std::unique_lock<std::mutex> writer_lock(paxos_logs_mtx_);
  if (type == ProposalType::SQL) {
    sql_paxos_logs_[round];
  } else {
    cdnt_paxos_logs_[round];
  }
}
// Update the promised_id in paxos_logs_map_ for the given key and round.
void PaxosStubsMap::AddPaxosLog(ProposalType type, int round,
                                   int promised_id) {
  std::unique_lock<std::mutex> writer_lock(paxos_logs_mtx_);
  if (type == ProposalType::SQL) {
    sql_paxos_logs_[round].set_promised_id(promised_id);
  } else {
    cdnt_paxos_logs_[round].set_promised_id(promised_id);
  }
}

// Update the acceptance info in paxos_logs_map_ for the given key and round.
void PaxosStubsMap::AddPaxosLog(ProposalType type, int round,
                                   int accepted_id, Proposal proposal) {
  std::unique_lock<std::mutex> writer_lock(paxos_logs_mtx_);
  if (type == ProposalType::SQL) {
    sql_paxos_logs_[round].set_accepted_id(accepted_id);
    *sql_paxos_logs_[round].mutable_proposal() = proposal;
  } else {
    cdnt_paxos_logs_[round].set_accepted_id(accepted_id);
    *cdnt_paxos_logs_[round].mutable_proposal() = proposal;
  }

}

// Add PaxosLog from recovery snapshot.
void PaxosStubsMap::AddPaxosLog(ProposalType type, int round, int promised_id,
                                int accepted_id, Proposal proposal) {
  std::unique_lock<std::mutex> writer_lock(paxos_logs_mtx_);
  if (type == ProposalType::SQL) {
    sql_paxos_logs_[round].set_promised_id(promised_id);
    sql_paxos_logs_[round].set_accepted_id(accepted_id);
    *sql_paxos_logs_[round].mutable_proposal() = proposal;
  } else {
    cdnt_paxos_logs_[round].set_promised_id(promised_id);
    cdnt_paxos_logs_[round].set_accepted_id(accepted_id);
    *cdnt_paxos_logs_[round].mutable_proposal() = proposal;
  }
}

}  // namespace obiew