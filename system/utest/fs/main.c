// Copyright 2016 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <fcntl.h>
#include <limits.h>
#include <unistd.h>

#include <fvm/fvm.h>
#include <unittest/unittest.h>
#include <zircon/device/device.h>

#include "filesystems.h"

int main(int argc, char** argv) {
    use_real_disk = false;
    int i = 1;
    while (i < argc - 1) {
        if ((strlen(argv[i]) == 2) && (argv[i][0] == '-') && (argv[i][1] == 'd')) {
            if (strnlen(argv[i + 1], PATH_MAX) > 0) {
                int fd = open(argv[i + 1], O_RDWR);
                if (fd < 0) {
                    fprintf(stderr, "[fs] Could not open block device\n");
                    return -1;
                } else if (ioctl_device_get_topo_path(fd, test_disk_path, PATH_MAX) < 0) {
                    fprintf(stderr, "[fs] Could not acquire topological path of block device\n");
                    return -1;
                }
                // If we previously tried running tests on this disk, it may
                // have created an FVM and failed. (Try to) clean up from previous state
                // before re-running.
                fvm_destroy(test_disk_path);
                close(fd);
                use_real_disk = true;
                break;
            }
        }
        i += 1;
    }

    return unittest_run_all_tests(argc, argv) ? 0 : -1;
}
