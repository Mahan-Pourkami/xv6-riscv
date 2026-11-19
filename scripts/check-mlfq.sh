#!/usr/bin/env bash
set -euo pipefail
cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.."
command -v timeout qemu-system-riscv64 >/dev/null
# Requires RISC-V toolchain, Perl, bc and QEMU >= 7.2.
for scheduler in RR MLFQ; do
  for cpus in 1 2; do
    echo "=== smoke: ${scheduler} CPUS=${cpus} ==="
    make clean >/dev/null
    log=$(mktemp)
    make kernel/kernel fs.img "SCHEDULER=${scheduler}" "CPUS=${cpus}" >"$log" 2>&1 || {
      cat "$log"; rm -f "$log"; exit 1;
    }
    # The first terminal commands are sent after building and booting.
    if [[ "$scheduler" == MLFQ ]]; then
      { sleep 4; printf 'mlfqtest 8\n'; sleep 10; printf 'echo MLFQ_SMOKE_OK\n'; sleep 2; printf '\001x'; } | \
        timeout 120 make qemu "SCHEDULER=${scheduler}" "CPUS=${cpus}" >>"$log" 2>&1 || {
          cat "$log"; rm -f "$log"; exit 1;
        }
      grep -q 'mlfqtest: PASS children finished' "$log" || {
        cat "$log"; rm -f "$log"; echo 'MLFQ workload failed'; exit 1;
      }
    else
      { sleep 4; printf 'echo MLFQ_SMOKE_OK\n'; sleep 2; printf '\001x'; } | \
        timeout 120 make qemu "SCHEDULER=${scheduler}" "CPUS=${cpus}" >>"$log" 2>&1 || {
          cat "$log"; rm -f "$log"; exit 1;
        }
    fi
    if ! tr -d '\r' <"$log" | grep -qx MLFQ_SMOKE_OK ||
       grep -q 'panic:' "$log" ||
       { [[ "$cpus" == 2 ]] && ! grep -q 'hart 1 starting' "$log"; }; then
      cat "$log"; rm -f "$log"; echo 'Missing smoke sentinel'; exit 1
    fi
    rm -f "$log"
  done
done
printf 'PASS: RR and MLFQ smoke on CPUS=1 and CPUS=2.\n'
