// Copyright 2018 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "wml/native_library.h"

#include <dlfcn.h>
#include <fcntl.h>

namespace wml {

NativeLibrary::NativeLibrary(const char* path) {
  ::dlerror();
  handle_ = ::dlopen(path, RTLD_NOW);
  if (handle_ == nullptr) {
    WML_LOG(ERROR) << "Could not open library '" << path << "' due to error '"
                   << ::dlerror() << "'.";
  }
}

NativeLibrary::NativeLibrary(Handle handle, bool close_handle)
    : handle_(handle), close_handle_(close_handle) {}

NativeLibrary::~NativeLibrary() {
  if (handle_ == nullptr) {
    return;
  }

  if (close_handle_) {
    ::dlerror();
    if (::dlclose(handle_) != 0) {
      handle_ = nullptr;
      WML_LOG(ERROR) << "Could not close library due to error '" << ::dlerror()
                     << "'.";
    }
  }
}

NativeLibrary::Handle NativeLibrary::GetHandle() const {
  return handle_;
}

wml::RefPtr<NativeLibrary> NativeLibrary::Create(const char* path) {
  auto library = wml::AdoptRef(new NativeLibrary(path));
  return library->GetHandle() != nullptr ? library : nullptr;
}

wml::RefPtr<NativeLibrary> NativeLibrary::CreateWithHandle(
    Handle handle,
    bool close_handle_when_done) {
  auto library =
      wml::AdoptRef(new NativeLibrary(handle, close_handle_when_done));
  return library->GetHandle() != nullptr ? library : nullptr;
}

wml::RefPtr<NativeLibrary> NativeLibrary::CreateForCurrentProcess() {
  return wml::AdoptRef(new NativeLibrary(RTLD_DEFAULT, false));
}

const uint8_t* NativeLibrary::ResolveSymbol(const char* symbol) {
  auto resolved_symbol = static_cast<const uint8_t*>(::dlsym(handle_, symbol));
  if (resolved_symbol == nullptr) {
    WML_DLOG(INFO) << "Could not resolve symbol in library: " << symbol;
  }
  return resolved_symbol;
}

}  // namespace wml
