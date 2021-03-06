/* <orly/synth/effecting_expr.h>

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

#include <base/class_traits.h>
#include <orly/orly.package.cst.h>
#include <orly/expr/effect.h>
#include <orly/synth/stmt_block.h>
#include <orly/synth/thatable_expr.h>

namespace Orly {

  namespace Synth {

    /* TODO */
    class TExprFactory;

    /* TODO */
    class TEffectingExpr
        : public TThatableExpr {
      NO_COPY(TEffectingExpr);
      public:

      /* TODO */
      TEffectingExpr(const TExprFactory *expr_factory, const Package::Syntax::TEffectingExpr *effecting_expr);

      /* TODO */
      virtual ~TEffectingExpr();

      /* TODO */
      virtual Expr::TExpr::TPtr Build() const;

      /* TODO */
      virtual void ForEachInnerScope(const std::function<void (TScope *)> &cb);

      /* TODO */
      virtual void ForEachRef(const std::function<void (TAnyRef &)> &cb);

      /* TODO */
      const Expr::TEffect::TPtr &GetSymbol() const;

      /* TODO */
      virtual Expr::TThatable::TPtr GetThatableSymbol() const;

      private:

      /* TODO */
      const Package::Syntax::TEffectingExpr *EffectingExpr;

      /* TODO */
      TExpr *Expr;

      /* TODO */
      TStmtBlock *StmtBlock;

      /* TODO */
      mutable Expr::TEffect::TPtr Symbol;

    };  // TEffectingExpr

  }  // Synth

}  // Orly
