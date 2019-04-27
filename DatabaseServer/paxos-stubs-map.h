#ifndef PAXOS_STUBS_MAP_H
#define PAXOS_STUBS_MAP_H

#include <grpcpp/grpcpp.h>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <utility>

#include "obiew.grpc.pb.h"

namespace obiew {
using PaxosStubs = std::map<std::string, std::unique_ptr<MultiPaxos::Stub>>;

// Stores a map from address to Paxos stubs.
// Thread-safe.
class PaxosStubsMap {
 public:
  PaxosStubsMap(PaxosStubs stubs) : stubs_(std::move(stubs)) {}
  std::string GetCoordinator();
  bool SetCoordinator(const std::string& coordinator);
  MultiPaxos::Stub* GetCoordinatorStub();
  MultiPaxos::Stub* GetStub(const std::string& address);
  std::map<std::string, MultiPaxos::Stub*> GetPaxosStubs();

  // Returns a set of keys in paxos_logs_map_.
  // std::unordered_set<std::string> GetPaxosLogKeys();

  // Returns a copy of PaxosLogs of a type.
  std::map<int, PaxosLog> GetPaxosLogs(ProposalType type);

  // Returns a copy of sql_paxos_logs_.
  std::map<int, PaxosLog> GetSqlPaxosLogs();

  // Returns a copy of cdnt_paxos_logs_.
  std::map<int, PaxosLog> GetCdntPaxosLogs();

  // Returns the mapped Paxos log for given key & round.
  // A new element will be constructed using its default constructor and
  // inserted if key or round is not found.
  PaxosLog GetPaxosLog(ProposalType type, int round);

  // Returns the latest Paxos round number for the given type.
  int GetLatestRound(ProposalType type);

  // Add PaxosLog when Acceptor receives a proposal.
  void AddPaxosLog(ProposalType type, int round);
  // Add PaxosLog when Acceptor promises a proposal.
  void AddPaxosLog(ProposalType type, int round, int promised_id);
  // Add PaxosLog when Acceptor accepts a proposal.
  void AddPaxosLog(ProposalType type, int round, int accepted_id,
                   Proposal proposal);
  // Add PaxosLog from recovery snapshot.
  void AddPaxosLog(ProposalType type, int round, int promised_id,
                   int accepted_id, Proposal proposal);

 private:
  PaxosStubs stubs_;
  std::mutex stubs_mtx_;
  std::string coordinator_;
  std::mutex coordinator_mtx_;
  std::map<int, PaxosLog> sql_paxos_logs_;
  std::map<int, PaxosLog> cdnt_paxos_logs_;
  std::mutex paxos_logs_mtx_;
};

}  // namespace obiew

#endif