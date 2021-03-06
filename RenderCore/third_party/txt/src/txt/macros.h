// Copyright 2018 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WEEX_RENDERER_FML_MACROS_H_
#define WEEX_RENDERER_FML_MACROS_H_

#include <memory>

#ifndef FML_USED_ON_EMBEDDER

#define FML_EMBEDDER_ONLY [[deprecated]]

#else  // FML_USED_ON_EMBEDDER

#define FML_EMBEDDER_ONLY

#endif  // FML_USED_ON_EMBEDDER

#define FML_DISALLOW_COPY(TypeName) TypeName(const TypeName&) = delete

#define FML_DISALLOW_ASSIGN(TypeName) \
  TypeName& operator=(const TypeName&) = delete

#define FML_DISALLOW_MOVE(TypeName) \
  TypeName(TypeName&&) = delete;    \
  TypeName& operator=(TypeName&&) = delete

#define FML_DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&) = delete;          \
  TypeName& operator=(const TypeName&) = delete

#define FML_DISALLOW_COPY_ASSIGN_AND_MOVE(TypeName) \
  TypeName(const TypeName&) = delete;               \
  TypeName(TypeName&&) = delete;                    \
  TypeName& operator=(const TypeName&) = delete;    \
  TypeName& operator=(TypeName&&) = delete

#define FML_DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName) \
  TypeName() = delete;                               \
  FML_DISALLOW_COPY_ASSIGN_AND_MOVE(TypeName)

#endif  // WEEX_RENDERER_FML_MACROS_H_
