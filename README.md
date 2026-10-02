# SE4060 Parallel Computing - MPI Lab (Lab Sheet 6)

MPI programs written in C/C++. Each exercise has its own folder with the source code, a `run.sh` script and an `output.txt` with the results.

## Environment

- MacBook Pro (macOS), MPICH installed with Homebrew (`brew install mpich`)
- Compile with `mpicc` (C) or `mpicxx` (C++), run with `mpirun -np <N>`

## How to run

```bash
cd ExerciseXX
chmod +x run.sh
./run.sh 2>&1 | tee output.txt
```

## Exercises

| Folder | Exercise | Description |
|---|---|---|
| `Exercise01` | 0 and 1 | Lecture activities: `HelloMPI.c`, `message1.cc`, `message2.cc` |
| `Exercise02` | 2 | Parallel sum of 1 to 10,000,000 using `MPI_Reduce` |
| `Exercise03` | 3 | Monte Carlo estimate of Pi, 10,000,000 samples, workers use `MPI_Send` and rank 0 uses `MPI_Recv` |
| `Exercise04` | 4 | Time vs processors and speedup graphs for Exercises 2 and 3 (`plot_graphs.py`) |
| `Exercise05` | 5 | Source/destination mismatch (`mismatch.cc`) and `message2` rewritten with `MPI_Bsend` (`bsend.cc`) |
| `Exercise06` | 6 | Exercise 3 with `MPI_ANY_SOURCE` on the receive (`pi_any_source.c`), compared with Exercise 3 |
| `Exercise07` | 7 | Exercise 6 with `MPI_Bsend` (`pi_bsend.c`), compared with Exercise 6 |

## Notes

- Exercise 2 is compiled with `-O0`, because with optimization the compiler can replace the summing loop with a formula and the timings become meaningless.
- Exercise 4 reads `Exercise02/output.txt` and `Exercise03/output.txt`. Run those first, then `python3 plot_graphs.py` (needs `matplotlib`).
- Exercise 4 in the lab sheet says "Exercise 1 and Exercise 2"; the graphs are made for Exercises 2 and 3, the two programs that can be timed.
- Exercise 6 and 7 can be compared with `python3 compare.py` inside their folders.