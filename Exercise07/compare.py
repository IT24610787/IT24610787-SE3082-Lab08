#!/usr/bin/env python3
"""Compare two Monte Carlo Pi result files side by side.

Usage:  python3 compare.py [original_output.txt] [new_output.txt]
Default: Exercise 6 (MPI_Send) vs Exercise 7 (MPI_Bsend).
"""
import re
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
a_path = Path(sys.argv[1]) if len(sys.argv) > 1 else HERE.parent / "Exercise06" / "output.txt"
b_path = Path(sys.argv[2]) if len(sys.argv) > 2 else HERE / "output.txt"
LINE = re.compile(r"np=(\d+).*?hits=(\d+).*?pi=([0-9.]+).*?avg_time=([0-9.]+)")


def load(path):
    rows = {}
    for line in path.read_text().splitlines():
        m = LINE.search(line)
        if m:
            rows[int(m.group(1))] = (int(m.group(2)), float(m.group(3)), float(m.group(4)))
    return rows


a, b = load(a_path), load(b_path)
print(f"A = {a_path}\nB = {b_path}\n")
print(f"{'np':>3} | {'hits A':>9} {'hits B':>9} {'same?':>6} | {'pi A':>9} {'pi B':>9} | {'time A (s)':>10} {'time B (s)':>10} {'B/A':>6}")
for p in sorted(set(a) & set(b)):
    (ha, pa, ta), (hb, pb, tb) = a[p], b[p]
    print(f"{p:>3} | {ha:>9} {hb:>9} {'yes' if ha == hb else 'NO':>6} | {pa:>9.6f} {pb:>9.6f} | {ta:>10.6f} {tb:>10.6f} {tb / ta:>6.2f}")