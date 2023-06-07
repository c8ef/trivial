#pragma once

#include <cstdint>
#include <map>

#include "argparse/argparse.hpp"
#include "spdlog/spdlog.h"

#define DBG_MACRO_NO_WARNING
#include "thirdparty/dbg.h"

#define ERROR(...)              \
  do {                          \
    spdlog::error(__VA_ARGS__); \
    exit(1);                    \
  } while (0)

#define UNREACHABLE() \
  ERROR("control flow should never reach here: {}:{}", __FILE__, __LINE__)

using i32 = int32_t;
using u32 = uint32_t;
using i64 = int64_t;
using u64 = uint64_t;

#define DEFINE_CLASSOF(cls, cond) \
  static bool classof(const cls* p) { return cond; }

#define DEFINE_LIST(type) \
  type* prev;             \
  type* next;

template <class T>
struct IndexMapper {
  std::map<T*, u32> mapping;
  u32 index_max = 0;

  u32 alloc() { return index_max++; }

  u32 get(T* t) {
    auto [it, inserted] = mapping.insert({t, index_max});
    index_max += inserted;
    return it->second;
  }
};

// see https://alisdair.mcdiarmid.org/arm-immediate-value-encoding/
inline bool can_encode_imm(i32 imm) {
  u32 encoding = imm;
  for (int ror = 0; ror < 32; ror += 2) {
    if (!(encoding & ~0xFFu)) {
      return true;
    }
    encoding = (encoding << 2u) | (encoding >> 30u);
  }
  return false;
}

template <typename Node>
struct IntrusiveList {
  Node* head;
  Node* tail;

  IntrusiveList() { head = tail = nullptr; }

  void InsertBefore(Node* new_node, Node* insert_before) {
    new_node->prev = insert_before->prev;
    new_node->next = insert_before;
    if (insert_before->prev) {
      insert_before->prev->next = new_node;
    }
    insert_before->prev = new_node;

    if (head == insert_before) {
      head = new_node;
    }
  }

  void InsertAfter(Node* new_node, Node* insert_after) {
    new_node->prev = insert_after;
    new_node->next = insert_after->next;
    if (insert_after->next) {
      insert_after->next->prev = new_node;
    }
    insert_after->next = new_node;

    if (tail == insert_after) {
      tail = new_node;
    }
  }

  void InsertAtEnd(Node* new_node) {
    new_node->prev = tail;
    new_node->next = nullptr;

    if (!tail) {
      head = tail = new_node;
    } else {
      tail->next = new_node;
      tail = new_node;
    }
  }

  void InsertAtBegin(Node* new_node) {
    new_node->prev = nullptr;
    new_node->next = head;

    if (!head) {
      head = tail = new_node;
    } else {
      head->prev = new_node;
      head = new_node;
    }
  }

  void Remove(Node* node) {
    if (node->prev) {
      node->prev->next = node->next;
    } else {
      head = node->next;
    }

    if (node->next) {
      node->next->prev = node->prev;
    } else {
      tail = node->prev;
    }
  }
};

inline bool debug_mode{false};
