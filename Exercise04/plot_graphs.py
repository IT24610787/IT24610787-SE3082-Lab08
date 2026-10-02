#!/usr/bin/env python3
"""Exercise 4 - Time vs number of processors and speedup graphs for Exercises 2 and 3.

Reads  ../Exercise02/output.txt  and  ../Exercise03/output.txt  (the lines printed by run.sh),
and writes PNG graphs plus results.csv into this folder.

Speedup(p) = T(1) / T(p)        Efficiency(p) = Speedup(p) / p
"""
import csv
import re
from pathlib import Path

import matplotlib
matplotlib.use("Agg")           # no window needed, just save files
import matplotlib.pyplot as plt

HERE = Path(__file__).resolve().parent
EXERCISES = {
    "ex2": ("Exercise 2: Parallel sum (1 to 10,000,000)", HERE.parent / "Exercise02" / "output.txt"),
    "ex3": ("Exercise 3: Monte Carlo Pi (10,000,000 samples)", HERE.parent / "Exercise03" / "output.txt"),
}
LINE = re.compile(r"np=(\d+).*?avg_time=([0-9.]+)")


def read_times(path):
    """Return {np: time_in_seconds}. If a process count appears twice, the last line wins."""
    times = {}
    for line in path.read_text().splitlines():
        m = LINE.search(line)
        if m:
            times[int(m.group(1))] = float(m.group(2))
    if 1 not in times:
        raise SystemExit(f"{path}: no np=1 line found, cannot compute speedup")
    return dict(sorted(times.items()))


def make_plots(key, title, times):
    procs = list(times)
    t = [times[p] for p in procs]
    speedup = [times[1] / times[p] for p in procs]

    # 1) Time vs number of processors
    plt.figure(figsize=(6, 4))
    plt.plot(procs, t, "o-", color="tab:blue")
    for p, v in zip(procs, t):
        plt.annotate(f"{v:.4f}", (p, v), textcoords="offset points", xytext=(0, 8), ha="center", fontsize=8)
    plt.xticks(procs)
    plt.xlabel("Number of processors")
    plt.ylabel("Execution time (s)")
    plt.title(f"{title}\nTime vs number of processors", fontsize=10)
    plt.grid(True, alpha=0.3)
    plt.tight_layout()
    plt.savefig(HERE / f"{key}_time.png", dpi=150)
    plt.close()

    # 2) Speedup, with the ideal (linear) speedup for comparison
    plt.figure(figsize=(6, 4))
    plt.plot(procs, speedup, "o-", color="tab:green", label="Measured speedup")
    plt.plot(procs, procs, "--", color="gray", label="Ideal (linear) speedup")
    for p, s in zip(procs, speedup):
        plt.annotate(f"{s:.2f}", (p, s), textcoords="offset points", xytext=(0, -14), ha="center", fontsize=8)
    plt.xticks(procs)
    plt.ylim(0, max(procs) * 1.05)
    plt.xlabel("Number of processors")
    plt.ylabel("Speedup  T(1) / T(p)")
    plt.title(f"{title}\nSpeedup", fontsize=10)
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.tight_layout()
    plt.savefig(HERE / f"{key}_speedup.png", dpi=150)
    plt.close()

    return [(p, times[p], times[1] / times[p], times[1] / times[p] / p) for p in procs]


def main():
    rows = []
    for key, (title, path) in EXERCISES.items():
        if not path.exists():
            raise SystemExit(f"Missing {path} - run that exercise's run.sh with '| tee output.txt' first")
        for p, t, s, e in make_plots(key, title, read_times(path)):
            rows.append([key, p, f"{t:.6f}", f"{s:.3f}", f"{e:.3f}"])

    with open(HERE / "results.csv", "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["exercise", "processors", "time_s", "speedup", "efficiency"])
        w.writerows(rows)
    print("Wrote ex2_time.png, ex2_speedup.png, ex3_time.png, ex3_speedup.png and results.csv")


if __name__ == "__main__":
    main()