#!/bin/bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ASCEND_CANN_PATH="${ASCEND_HOME_PATH:-${ASCEND_INSTALL_PATH:-}}"

if [[ -z "${ASCEND_CANN_PATH}" ]]; then
    echo "[ERROR] ASCEND_HOME_PATH or ASCEND_INSTALL_PATH is not set."
    echo "[ERROR] Please source CANN set_env.sh before running this test."
    exit 1
fi

if [[ ! -f "${ASCEND_CANN_PATH}/bin/setenv.bash" ]]; then
    echo "[ERROR] ${ASCEND_CANN_PATH}/bin/setenv.bash does not exist."
    exit 1
fi

source "${ASCEND_CANN_PATH}/bin/setenv.bash"

cd "${SCRIPT_DIR}"
rm -rf build
cmake -B build -DASCEND_CANN_PACKAGE_PATH="${ASCEND_CANN_PATH}"
cmake --build build -j
./build/main
