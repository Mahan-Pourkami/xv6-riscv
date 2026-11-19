#!/usr/bin/env python3
"""Lightweight source-contract checks. QEMU functional tests are also required."""
from pathlib import Path
import sys

root = Path(__file__).resolve().parents[1]
proc = (root / 'kernel/proc.c').read_text()
trap = (root / 'kernel/trap.c').read_text()
header = (root / 'kernel/proc.h').read_text()
mk = (root / 'Makefile').read_text()
assert '#define MLFQ_RR_QUANTUM 5' in header
assert '#define MLFQ_AGING_TICKS 800' in header
assert 'p->quantum_ticks++' in trap
assert 'w_stimecmp(r_time() + 1000000)' in trap
assert 'mlfq_aging_tick();' in trap
assert 'p->state == RUNNABLE' in proc
assert 'p->queue_enter_tick' in proc
assert 'choices[i].confidence' in proc
assert 'SCHEDULER ?= RR' in mk
assert '$U/_mlfqtest' in mk
assert 'timer_should_yield(p)' in trap
print('PASS: MLFQ static source contracts (not a substitute for functional tests)')
