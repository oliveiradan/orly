#include <bit/file_info.h>

#include <base/not_implemented.h>

using namespace Base;
using namespace Bit;
using namespace std;

TFileConfig::TFileConfig(const TAbsPath &src)
    : JobConfig(ReadJobConfig(TJson::TryRead(src.Path), src.Path)) {}

TFileInfo::TFileInfo(TRelPath &&path, std::string &&cmd_path, TFileConfig &&src_config, bool is_src)
    : CmdPath(move(cmd_path)),
      RelPath(move(path)),
      SrcConfig(move(src_config)),
      Completed(is_src) {}

bool TFileInfo::IsComplete() const { return Completed; }

#pragma clang diagnostic ignored "-Wmissing-noreturn"

void TFileInfo::Complete(const TJob *, TJobConfig &&) { NOT_IMPLEMENTED(); }

std::ostream &Bit::operator<<(std::ostream &out, const TFileInfo *file_info) {
  // TODO(cmaloney): Make a more interesting representation for file_info based
  // on trees?
  out << file_info->CmdPath;

  return out;
}
