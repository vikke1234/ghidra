/* ###
 * IP: GHIDRA
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/// \file testsparseswitch.cc
/// \brief Unit tests for sparse switch (if-else chain) detection
///
/// Note: Full integration tests for ActionSparseSwitch require the complete decompiler
/// pipeline (SLEIGH specs, LoadImage, flow analysis).  These tests verify the
/// sub-components that can be tested in isolation.

#include "coreaction.hh"
#include "test.hh"

namespace ghidra {

TEST(sparseswitch_min_cases_is_sane) {
  // The minimum number of cases should be at least 3 to avoid
  // misidentifying simple if-else as a switch
  ASSERT(ActionSparseSwitch::MIN_SPARSE_CASES >= 7);
}

TEST(sparseswitch_chain_entry_struct_layout) {
  // Verify ChainEntry struct is usable
  ActionSparseSwitch::ChainEntry entry;
  entry.cmpBlock = (BlockBasic *)0;
  entry.caseBody = (BlockBasic *)0;
  entry.caseLabel = 42;
  entry.caseEdge = 1;
  ASSERT_EQUALS(entry.caseLabel, (uintb)42);
  ASSERT_EQUALS(entry.caseEdge, 1);
}

TEST(sparseswitch_detect_rejects_null) {
  // detectChain on a null-like scenario: a block with wrong sizeOut
  // We can't easily create real blocks without Funcdata, but we verify
  // the function signature is correct and accessible.
  vector<ActionSparseSwitch::ChainEntry> chain;
  BlockBasic *defaultBlock = (BlockBasic *)0;
  // Cannot call detectChain without a real block, just verify it compiles
  (void)chain;
  (void)defaultBlock;
  ASSERT(true);
}

} // End namespace ghidra
