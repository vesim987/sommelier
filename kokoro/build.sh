#!/bin/bash

set -ex

# Move docker on kokoro to use the scratch /tmpfs space.
setup_docker() {
    sudo stop docker

    sudo mv /var/lib/docker /tmpfs/
    sudo ln -s /tmpfs/docker /var/lib/docker

    sudo start docker
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

    setup_docker

    local image_name=xwl-build-stretch

    docker build "${src_root}/kokoro" -t "${image_name}"

    docker run --volume "${src_root}:/src:rw" \
               --user="$(id -u)" \
               "${image_name}"

    # Copy resulting debs to results directory.
    cp *.deb "${result_dir}"
}

main "$@"
