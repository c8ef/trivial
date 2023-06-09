#pragma once

#include <unordered_map>

#include "structure/ir.hpp"

inline void CFGDFS(BasicBlock* bb, u32 dom_level) {
  bb->dom_level = dom_level;
  for (BasicBlock* ch : bb->doms) {
    CFGDFS(ch, dom_level + 1);
  }
}

// reference: Engineering A Compiler, Third Edition
inline void ComputeDomInfo(IrFunc* f) {
  BasicBlock* entry = f->bb.head;
  // calculate dom_by
  entry->dom_by = {entry};
  std::unordered_set<BasicBlock*> all;
  // all is the set of all basic blocks
  // is the default value of dom_by except entry block
  for (BasicBlock* bb = entry; bb; bb = bb->next) {
    all.insert(bb);
    // just clear the doms field, has nothing to do with dom_by
    bb->doms.clear();
  }
  for (BasicBlock* bb = entry->next; bb; bb = bb->next) {
    bb->dom_by = all;
  }
  while (true) {
    bool changed = false;
    for (BasicBlock* bb = entry->next; bb; bb = bb->next) {
      for (auto it = bb->dom_by.begin(); it != bb->dom_by.end();) {
        BasicBlock* x = *it;
        // if basic block bb has a predecessor p, and p's dom_by does not
        // contain x, then bb's dom_by should not contain x
        if (x != bb &&
            std::any_of(bb->pred.begin(), bb->pred.end(), [x](BasicBlock* p) {
              return p->dom_by.find(x) == p->dom_by.end();
            })) {
          changed = true;
          it = bb->dom_by.erase(it);
        } else {
          ++it;
        }
      }
    }
    if (!changed) {
      break;
    }
  }

  // calculate immediate_dom and doms
  entry->immediate_dom = nullptr;
  for (BasicBlock* bb = entry->next; bb; bb = bb->next) {
    for (BasicBlock* d : bb->dom_by) {
      // you cannot find the immediate_dom from the dom_by field from current
      // dom_by set except the current basic block and the immediate_dom
      if (d != bb && std::all_of(bb->dom_by.begin(), bb->dom_by.end(),
                                 [d, bb](BasicBlock* x) {
                                   return x == bb || x == d ||
                                          x->dom_by.find(d) == x->dom_by.end();
                                 })) {
        bb->immediate_dom = d;
        d->doms.push_back(bb);
        break;
      }
    }
  }

  // calculate dom_level
  CFGDFS(entry, 0);
}

// to ∈ DF(x):
// 1. to has a CFG predecessor that x dominates. There exists an y such that
// (y, to) is a CFG edge and x ∈ DOM(y).
// 2. x does not strictly dominate to. That is, x ∉ (DOM(to) − to).
inline std::unordered_map<BasicBlock*, std::unordered_set<BasicBlock*>>
ComputeDF(IrFunc* f) {
  std::unordered_map<BasicBlock*, std::unordered_set<BasicBlock*>> df;
  for (BasicBlock* from = f->bb.head; from; from = from->next) {
    for (BasicBlock* to : from->Succ()) {
      if (to) {
        // enumerate the edges (from, to)
        BasicBlock* x = from;
        // x does not strictly dom to
        while (x == to || to->dom_by.find(x) == to->dom_by.end()) {
          df[x].insert(to);
          x = x->immediate_dom;
        }
      }
    }
  }
  return df;
}

struct Loop {
  Loop* parent{};
  std::vector<Loop*> sub_loops;
  // bbs[0] = loop header
  std::vector<BasicBlock*> bbs;

  explicit Loop(BasicBlock* header) : bbs{header} {}

  BasicBlock* Header() { return bbs[0]; }

  // for top level root, Depth = 1
  u64 Depth() {
    u64 ret = 0;
    for (Loop* x = this; x; x = x->parent) ++ret;
    return ret;
  }

  void GetDeepestLoops(std::vector<Loop*>& deepest) {
    if (sub_loops.empty())
      deepest.push_back(this);
    else
      for (Loop* x : sub_loops) x->GetDeepestLoops(deepest);
  }
};

struct LoopInfo {
  std::unordered_map<BasicBlock*, Loop*> loop_of_bb;
  std::vector<Loop*> top_level;

  // return the deepest loop the bb lies in
  // is bb is not in any loop, return 0
  u32 DepthOf(BasicBlock* bb) {
    auto it = loop_of_bb.find(bb);
    return it == loop_of_bb.end() ? 0 : it->second->Depth();
  }

  std::vector<Loop*> DeepestLoops() {
    std::vector<Loop*> deepest;
    for (Loop* l : top_level) l->GetDeepestLoops(deepest);
    return deepest;
  }
};

inline void Populate(LoopInfo& info, BasicBlock* bb) {
  if (bb->vis) return;
  bb->vis = true;
  for (BasicBlock* s : bb->Succ()) {
    if (s) Populate(info, s);
  }
  auto it = info.loop_of_bb.find(bb);
  Loop* sub = it == info.loop_of_bb.end() ? nullptr : it->second;
  if (sub && sub->Header() == bb) {
    (sub->parent ? sub->parent->sub_loops : info.top_level).push_back(sub);
    std::reverse(sub->bbs.begin() + 1, sub->bbs.end());
    std::reverse(sub->sub_loops.begin(), sub->sub_loops.end());
    sub = sub->parent;
  }
  for (; sub; sub = sub->parent) sub->bbs.push_back(bb);
}

// post order traverse the dom tree
// the worklist between each call has nothing to do with each other
inline void CollectLoops(LoopInfo& info, std::vector<BasicBlock*>& worklist,
                         BasicBlock* header) {
  for (BasicBlock* s : header->doms) {
    if (s) CollectLoops(info, worklist, s);
  }
  assert(worklist.empty());
  for (BasicBlock* p : header->pred) {
    // exist edge between p and header and header dominates p
    if (p->dom_by.find(header) != p->dom_by.end()) {
      // back edge
      worklist.push_back(p);
    }
  }
  if (!worklist.empty()) {
    Loop* l = new Loop(header);
    while (!worklist.empty()) {
      BasicBlock* pred = worklist.back();
      worklist.pop_back();
      if (auto [it, inserted] = info.loop_of_bb.insert({pred, l}); inserted) {
        // 插入成功意味着 pred 原先不属于任何 loop，现在它属于这个 loop 了
        if (pred != header) {
          worklist.insert(worklist.end(), pred->pred.begin(), pred->pred.end());
        }
      } else {
        // 这是一个已经发现的 loop
        Loop* sub = it->second;
        while (Loop* p = sub->parent) sub = p;  // 找到已发现的最外层的 loop
        if (sub != l) {
          sub->parent = l;
          // 只需考虑 sub 的 header 的 pred，因为根据循环的性质，循环中其他 bb
          // 的 pred 必然都在循环内
          for (BasicBlock* pred : sub->Header()->pred) {
            auto it = info.loop_of_bb.find(pred);
            if (it == info.loop_of_bb.end() || it->second != sub) {
              worklist.push_back(pred);
            }
          }
        }
      }
    }
  }
}

inline LoopInfo ComputeLoopInfo(IrFunc* f) {
  ComputeDomInfo(f);
  LoopInfo info;
  std::vector<BasicBlock*> worklist;
  CollectLoops(info, worklist, f->bb.head);
  f->ClearAllVis();
  Populate(info, f->bb.head);
  return info;
}

inline void CFGDFS(std::vector<BasicBlock*>& po, BasicBlock* bb) {
  if (!bb->vis) {
    bb->vis = true;
    for (BasicBlock* x : bb->Succ()) {
      if (x) CFGDFS(po, x);
    }
    po.push_back(bb);
  }
}

// calculate basic block reverse post order
inline std::vector<BasicBlock*> ComputeRPO(IrFunc* f) {
  std::vector<BasicBlock*> ret;
  f->ClearAllVis();
  CFGDFS(ret, f->bb.head);
  std::reverse(ret.begin(), ret.end());
  return ret;
}
