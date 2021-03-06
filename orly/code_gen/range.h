/* <orly/code_gen/range.h>

   TODO

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

#pragma once

#include <orly/code_gen/inline.h>

namespace Orly {

  namespace CodeGen {

    class TRange : public TInline {
      NO_COPY(TRange);
      public:

      TRange(const L0::TPackage *package,
             const TInline::TPtr &start,
             const TInline::TPtr &opt_limit,
             const TInline::TPtr &opt_stride,
             bool include_end);

      void WriteExpr(TCppPrinter &out) const;

      /* Dependency graph */
      virtual void AppendDependsOn(std::unordered_set<TInline::TPtr> &dependency_set) const override {
        assert(this);
        dependency_set.insert(Start);
        Start->AppendDependsOn(dependency_set);
        if (OptLimit) {
          dependency_set.insert(OptLimit);
          OptLimit->AppendDependsOn(dependency_set);
        }
        if (OptStride) {
          dependency_set.insert(OptStride);
          OptStride->AppendDependsOn(dependency_set);
        }
      }

      private:
      TInline::TPtr Start, OptLimit, OptStride;
      bool IncludeEnd;

    }; // TRange

  } // CodeGen

} // Orly