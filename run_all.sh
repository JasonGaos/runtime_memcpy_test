#!/bin/bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

tests=(
    "1_h2d_sync_full_buffer"
    "2_h2d_async_full_buffer"
    "3_d2h_sync_full_buffer"
    "4_d2h_async_full_buffer"
)

for test_dir in "${tests[@]}"; do
    echo "[INFO] Running ${test_dir}"
    bash "${SCRIPT_DIR}/${test_dir}/run.sh"
done

echo "[SUCCESS] All full-buffer memory copy tests passed"
