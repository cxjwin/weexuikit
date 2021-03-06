// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WEEX_UIKIT_WML_THREAD_LOCAL_H_
#define WEEX_UIKIT_WML_THREAD_LOCAL_H_

#include <functional>

#include "wml/build_config.h"
#include "wml/logging.h"
#include "wml/macros.h"

#define WML_THREAD_LOCAL_PTHREADS OS_MACOSX || OS_LINUX || OS_ANDROID

#if WML_THREAD_LOCAL_PTHREADS
#include <pthread.h>
#endif

namespace wml {

using ThreadLocalDestroyCallback = std::function<void(intptr_t)>;

#if WML_THREAD_LOCAL_PTHREADS

// thread_local is unavailable and we have to resort to pthreads.

#define WML_THREAD_LOCAL static

class ThreadLocal {
 private:
  class Box {
   public:
    Box(ThreadLocalDestroyCallback destroy, intptr_t value)
        : destroy_(destroy), value_(value) {}

    intptr_t Value() const { return value_; }

    void SetValue(intptr_t value) {
      if (value == value_) {
        return;
      }

      DestroyValue();
      value_ = value;
    }

    void DestroyValue() {
      if (destroy_) {
        destroy_(value_);
      }
    }

   private:
    ThreadLocalDestroyCallback destroy_;
    intptr_t value_;

    WML_DISALLOW_COPY_AND_ASSIGN(Box);
  };

  static inline void ThreadLocalDestroy(void* value) {
    WML_CHECK(value != nullptr);
    auto box = reinterpret_cast<Box*>(value);
    box->DestroyValue();
    delete box;
  }

 public:
  ThreadLocal() : ThreadLocal(nullptr) {}

  ThreadLocal(ThreadLocalDestroyCallback destroy) : destroy_(destroy) {
    auto callback =
        reinterpret_cast<void (*)(void*)>(&ThreadLocal::ThreadLocalDestroy);
    WML_CHECK(pthread_key_create(&_key, callback) == 0);
  }

  void Set(intptr_t value) {
    auto box = reinterpret_cast<Box*>(pthread_getspecific(_key));
    if (box == nullptr) {
      box = new Box(destroy_, value);
      WML_CHECK(pthread_setspecific(_key, box) == 0);
    } else {
      box->SetValue(value);
    }
  }

  intptr_t Get() {
    auto box = reinterpret_cast<Box*>(pthread_getspecific(_key));
    return box != nullptr ? box->Value() : 0;
  }

  ~ThreadLocal() {
    // This will NOT call the destroy callbacks on thread local values still
    // active in other threads. Those must be cleared manually. The usage
    // of this class should be similar to the thread_local keyword but with
    // with a static storage specifier

    // Collect the container
    delete reinterpret_cast<Box*>(pthread_getspecific(_key));

    // Finally, collect the key
    WML_CHECK(pthread_key_delete(_key) == 0);
  }

 private:
  pthread_key_t _key;
  ThreadLocalDestroyCallback destroy_;

  WML_DISALLOW_COPY_AND_ASSIGN(ThreadLocal);
};

#else  // WML_THREAD_LOCAL_PTHREADS

#define WML_THREAD_LOCAL thread_local

class ThreadLocal {
 public:
  ThreadLocal() : ThreadLocal(nullptr) {}

  ThreadLocal(ThreadLocalDestroyCallback destroy)
      : destroy_(destroy), value_(0) {}

  void Set(intptr_t value) {
    if (value_ == value) {
      return;
    }

    if (value_ != 0 && destroy_) {
      destroy_(value_);
    }

    value_ = value;
  }

  intptr_t Get() { return value_; }

  ~ThreadLocal() {
    if (value_ != 0 && destroy_) {
      destroy_(value_);
    }
  }

 private:
  ThreadLocalDestroyCallback destroy_;
  intptr_t value_;

  WML_DISALLOW_COPY_AND_ASSIGN(ThreadLocal);
};

#endif  // WML_THREAD_LOCAL_PTHREADS

#ifndef WML_THREAD_LOCAL

#error Thread local storage unavailable on the platform.

#endif  // WML_THREAD_LOCAL

}  // namespace wml

#endif  // WEEX_UIKIT_WML_THREAD_LOCAL_H_
