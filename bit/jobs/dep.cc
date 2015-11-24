#include <bit/jobs/dep.h>

#include <fstream>
#include <ostream>

#include <bit/file_info.h>
#include <bit/jobs/dep_c.h>

using namespace Base;
using namespace Bit;
using namespace Bit::Job;
using namespace std;

static std::unordered_set<TRelPath> GetOutputName(const TRelPath &input) {
  return {TRelPath(input.AddExtension(".dep"))};
}

static TOpt<TRelPath> TryGetInputName(const TRelPath &output) {
  auto result = output.TryRemoveExtension(".dep");
  if (result) {
    return *result;
  } else {
    return TOpt<TRelPath>();
  }
}

TJobProducer TDep::GetProducer(const TJobConfig &job_config) {
  return TJobProducer{"dependency_file",
                      {".dep"},
                      TryGetInputName,
                      GetOutputName,
                      // TODO: Should be able to eliminate the lambda wrapper here...
                      [job_config](TJob::TMetadata &&metadata) -> unique_ptr<TJob> {
                        return unique_ptr<TDep>(new TDep(std::move(metadata), &job_config));
                      }};
}

TJob::TOutput TDep::Run() {
  TJob::TOutput result;

  // Figure out which dep program to run, the right arguments for it

  // If the source is a C or C++ file, give extra arguments as the extra arguments we'd pass to the
  // compiler
  const TRelPath &input_path = GetInput()->RelPath;

  vector<string> cmd;
  // Only C, C++ are supported currently.
  assert(input_path.EndsWith(".cc") || input_path.EndsWith(".c"));

  if (input_path.EndsWith(".cc")) {
    // TODO(cmaloney): Make a helper utility in <jobs/compile_c_family.h> to get the command.
    cmd.push_back(Bit::GetCmd<Tools::Cc>(Env.GetConfig()));
  } else {
    cmd.push_back(Bit::GetCmd<Tools::C>(Env.GetConfig()));
  }

  // TODO: move array append
  for (auto &arg : TCompileCFamily::GetStandardArgs(GetInput(), ext == "cc", Env)) {
    cmd.push_back(move(arg));
  }
  cmd.push_back("-M");
  cmd.push_back("-MG");
  cmd.push_back(GetInput()->GetPath());

  TSubprocess::Run(cmd);

  return cmd;
}

void TDep::ProcessOutput(TFd stdout, TFd) {
  NeedsWork = false;
  Deps = ParseDeps(ReadAll(move(stdout)));
  for (const auto &dep : Deps) {
    TFileInfo *file = Env.TryGetFileFromPath(dep);
    if (file) {
      // Add to needs. If it's new in the Needs array, we aren't done yet.
      NeedsWork |= Needs.insert(file).second;
    }
  }
}

TJson ToJson(vector<string> &&that) {
  vector<TJson> json_elems(that.size());
  for (uint64_t i = 0; i < that.size(); ++i) {
    json_elems[i] = TJson(move(that[i]));
  }

  return TJson(move(json_elems));
}

bool TDep::IsComplete() {
  assert(this);

  if (NeedsWork) {
    return false;
  }

  // Everything has been found, save the information.
  auto deps_json = ToJson(move(Deps));

  // TODO(cmaloney): Writing the dep file doesn't actually provide us any
  // benefit, but is required by the work finder since it is a file expected to
  // exist.
  // TODO(cmaloney): Stream from C++ struct -> json rather than doing this
  // "copy into something that looks like JSON".
  /* out_file */ {
    ofstream out_file(GetSoleOutput()->GetPath());
    deps_json.Write(out_file);
  }

  // Stash the info on the input file as computed config
  // TODO: In pushing this as strings, we effectively discard all the file
  // lookups we've already
  // done
  GetSoleOutput()->PushComputedConfig(
      TJson::TObject{{"c++", TJson::TObject{{"include", move(deps_json)}}}});

  return true;
}

TDep::TDep(TEnv &env, TFileInfo *in_file)
    : TJob(in_file, {env.GetFile(GetOutputName(in_file->GetRelPath()))}), Env(env) {}