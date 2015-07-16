/* bit build environment.

Keeps track of all the files which exist, could exist lazily.

Determines whether or not files could exist by lazily seeing if there is a path
from a file which does exist which could be manipulated by known jobs to produce
the needed file. */

#include <base/class_traits.h>
#include <bit/file.h>
#include <bit/job.h>
#include <bit/naming.h>

namespace Bit {

// Simple memory management by making one object (The registry) own all the pointers/memory.
template <typename TKey, typename TValue, typename Hash = std::hash<TKey>>
class TInterner {
  public:
  TValue *Add(TKey &&key, std::unique_ptr<TValue> &&item) {
    // TODO: Add a new InsertOrFail which can handle this
    auto result = ManagedThings.emplace(std::move(key), std::move(item));
    assert(result.second);

    return result.first->second.get();
  }

  TValue *TryGet(const TKey &key) {
    auto it = ManagedThings.find(key);
    if(it != ManagedThings.end()) {
      return it->second.get();
    }
    return nullptr;
  }

  private:
  std::unordered_map<TKey, std::unique_ptr<TValue>, Hash> ManagedThings;
};

class TJobFactory {
  public:

  // TODO(cmaloney): Make this take a list of names to activate, job modules
  // to load (may have more than one name per module).
  TJobFactory(const TJobConfig &JobConfig, const std::unordered_set<std::string> &Jobs);

  // TODO(cmaloney): This seems very suspect....
  // NOTE: We don't just use a tuple because we want a specialized hash
  struct TJobDesc {
    TFile *f;
    const char *job_name;

    bool operator==(const TJobDesc &that) const noexcept {
      return f == that.f && job_name == that.job_name;
    }
  };

  struct TJobDescHash {
    size_t operator()(const TJobDesc &that) const noexcept {
      // TODO: Write a better hash...
      return size_t(that.f) + size_t(that.job_name);
    }
  };

  // A Job registration can verify if they can indeed make a given output file
  // A job registration is used to do longest tail matches and see if the job's input can be
  // produced
  // NOTE: We generate a job object when it's input can be produced
  void Register(TJobProducer &&producer) { JobProducers.emplace_back(std::move(producer)); }

  std::unordered_set<TJob *> GetPotentialJobs(TEnv &env, TFile *out_file);

  private:
  // TODO: Make a better hash function.
  TInterner<TJobDesc, TJob, TJobDescHash> Jobs;

  // NOTE: This is used purely for caching the reverse-lookups used by  GetPotentialJobs.
  // Entry in this map simply means a job exists, not that it's possible to get it's input.
  std::unordered_multimap<TFile *, TJob *> JobsByOutput;

  // TODO: We iterate over this a lot. In practice we go over a lot more than needed
  // Perf test using more concise data structures where we don't scan so much needlessly vs. this
  // "one simple list" we
  // always iterate over all of.
  std::vector<TJobProducer> JobProducers;
};

class TEnvironment {
  public:
  NO_COPY(TEnvironment);
  NO_MOVE(TEnvironment);

  TEnvironment(const TConfig &config, const TTree &src);

  TFile *GetFile(TRelPath name);

  private:
  std::unordered_map<std::string, TFile *> PathLookupCache;
  TInterner<TRelPath, TFile> Files;
  TJobFactory Jobs;

  TTree Src, Out;
};
}  // Bit
