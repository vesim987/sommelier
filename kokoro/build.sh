#!/bin/bash

set -ex

install_deps() {
    sudo apt-get install debhelper \
                         libsystemd-dev \
                         libwayland-dev \
                         libwayland-bin \
                         libxcb-composite0-dev \
                         pkg-config
}

main() {
    if [ -z "${KOKORO_ARTIFACTS_DIR}" ]; then
        echo "This script must be run in kokoro"
        exit 1
    fi

    local src_root="${KOKORO_ARTIFACTS_DIR}"/git/xwl
    local result_dir="${src_root}"/results
    mkdir -p "${result_dir}"

    cd "${src_root}"

    install_deps

    # Build all targets.
    make deb

    # Copy resulting debs to results directory.
    cp *.deb "${result_dir}"
}

main "$@"
