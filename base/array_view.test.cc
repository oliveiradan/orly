/* <base/array_view.test.cc>

   Unit test for <base/array_view.h>.

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

#include <base/array_view.h>

#include <test/kit.h>
#include <test/life_tracker.h>

using namespace std;
using namespace Base;

using TLifeTracker = Test::TLifeTracker;
using TCounts = TLifeTracker::TCounts;

FIXTURE(StaticArray) {
  TLifeTracker elems[3];
  TArrayView<TLifeTracker> view(elems);
  EXPECT_TRUE(view);
  EXPECT_EQ(*view.Peek(), TCounts(0, 0, 0, 0, 0 ));
  EXPECT_EQ(view.GetSize(), 3u);
  TLifeTracker elem = view.Pop();
  EXPECT_EQ(view.GetSize(), 2u);
  EXPECT_EQ(*elem, TCounts(0, 1, 0, 0, 0 ));
  TArrayView<const TLifeTracker> copyof_view(view);
  EXPECT_EQ(copyof_view.GetSize(), 2u);
  copyof_view.Skip();
  EXPECT_EQ(copyof_view.GetSize(), 1u);
}
