// Copyright 2017 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <fs/service.h>

#include <fcntl.h>

namespace fs {

Service::Service(Connector connector)
    : connector_(fbl::move(connector)) {}

Service::~Service() = default;

zx_status_t Service::Open(uint32_t flags, fbl::RefPtr<Vnode>* out_redirect) {
    if (flags & O_DIRECTORY) {
        return ZX_ERR_NOT_DIR;
    }
    return ZX_OK;
}

zx_status_t Service::Getattr(vnattr_t* attr) {
    // TODO(ZX-1151): V_IRUSR isn't right since we can't read from a service
    // but the node must be readable for enumeration to work until O_PATH
    // support is implemented in FDIO.
    // TODO(ZX-1152): V_TYPE_FILE isn't right, we should use a type for services
    memset(attr, 0, sizeof(vnattr_t));
    attr->mode = V_TYPE_FILE | V_IRUSR;
    attr->nlink = 1;
    return ZX_OK;
}

zx_status_t Service::Serve(Vfs* vfs, zx::channel channel, uint32_t flags) {
    ZX_DEBUG_ASSERT(!(flags & O_DIRECTORY)); // checked by Open

    // TODO(ZX-1151): If the access mode is O_PATH then we should call the
    // superclass Serve function instead of binding the channel.

    if (!connector_) {
        return ZX_ERR_NOT_SUPPORTED;
    }
    return connector_(fbl::move(channel));
}

} // namespace fs