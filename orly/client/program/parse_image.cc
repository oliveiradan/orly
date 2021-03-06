/* <orly/client/program/parse_image.cc>

   Implements <orly/client/program/parse_image.h>.

   Copyright 2010-2014 OrlyAtomics, Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <orly/client/program/parse_image.h>

#include <memory>

#include <base/thrower.h>
#include <orly/client/program/throw_syntax_errors.h>
#include <orly/client/program/translate_expr.h>

using namespace std;
using namespace Base;
using namespace Orly;
using namespace Orly::Client::Program;

static bool ForEachXact(const Tools::Nycr::TContextBuilt<TProgram> &program, const TForXact &cb) {
  ThrowSyntaxErrors(program);
  assert(&cb);
  auto image = dynamic_cast<const TImage *>(program.Get()->GetTop());
  if (!image) {
    DEFINE_ERROR(error_t, runtime_error, "not an image");
    THROW_ERROR(error_t);
  }
  auto opt_xact_seq = image->GetOptXactSeq();
  for (;;) {
    auto xact_seq = dynamic_cast<const TXactSeq *>(opt_xact_seq);
    if (!xact_seq) {
      break;
    }
    if (!cb(xact_seq->GetXact())) {
      return false;
    }
    opt_xact_seq = xact_seq->GetOptXactSeq();
  }
  return true;
}

bool Orly::Client::Program::ParseImageFile(const char *path, const TForXact &cb) {
  return ForEachXact(TProgram::ParseFile(path), cb);
}

bool Orly::Client::Program::ParseImageStr(const char *str, const TForXact &cb) {
  return ForEachXact(TProgram::ParseStr(str), cb);
}

bool Orly::Client::Program::TranslateXact(const TXact *xact, const TForKv &cb) {
  assert(xact);
  assert(&cb);
  void
      *lhs_alloc = alloca(Sabot::State::GetMaxStateSize()),
      *rhs_alloc = alloca(Sabot::State::GetMaxStateSize());
  auto opt_seq = xact->GetOptKvSeq();
  for (;;) {
    auto seq = dynamic_cast<const TKvSeq *>(opt_seq);
    if (!seq) {
      break;
    }
    auto kv = dynamic_cast<const TKv *>(seq->GetAnyKv());
    if (!kv) {
      DEFINE_ERROR(error_t, runtime_error, "malformed kv entry in image file");
      THROW_ERROR(error_t);
    }
    Sabot::State::TAny::TWrapper
        lhs(NewStateSabot(kv->GetLhs(), lhs_alloc)),
        rhs(NewStateSabot(kv->GetRhs(), rhs_alloc));
    if (!cb(lhs.get(), rhs.get())) {
      return false;
    }
    opt_seq = seq->GetOptKvSeq();
  }
  return true;
}
