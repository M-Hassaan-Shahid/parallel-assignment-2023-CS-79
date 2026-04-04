import csv
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

# ── Load CSV ──────────────────────────────────────────────────
data = {}
with open('timing_data.csv') as f:
    reader = csv.DictReader(f)
    for row in reader:
        n = int(row['ProblemSize'])
        p = int(row['Processes'])
        if n not in data:
            data[n] = {'p': [], 'time': [], 'speedup': [], 'seq_time': float(row['SequentialTime'])}
        data[n]['p'].append(p)
        data[n]['time'].append(float(row['ParallelTime']))
        data[n]['speedup'].append(float(row['Speedup']))

sizes  = sorted(data.keys())
colors = ['#1f77b4', '#ff7f0e', '#2ca02c']

# ── Plot 1: Speedup vs Processes ─────────────────────────────
fig, ax = plt.subplots(figsize=(8, 5))

for i, n in enumerate(sizes):
    ax.plot(data[n]['p'], data[n]['speedup'],
            marker='o', label=f'N={n:,}', color=colors[i], linewidth=2)

# Ideal linear and quadratic speedup lines
ps = [1, 2, 4, 8]
ax.plot(ps, ps,        'k--', linewidth=1, label='Ideal linear (P)')
ax.plot(ps, [p**2 for p in ps], 'k:', linewidth=1, label='Ideal quadratic (P²)')

ax.set_xlabel('Number of Threads (P)', fontsize=12)
ax.set_ylabel('Speedup', fontsize=12)
ax.set_title('Speedup vs Number of Threads\n(Sorted Linked List — Pthreads)', fontsize=13)
ax.set_xticks(ps)
ax.legend(fontsize=10)
ax.grid(True, linestyle='--', alpha=0.5)
plt.tight_layout()
plt.savefig('speedup_plot.png', dpi=150)
plt.close()
print("Saved speedup_plot.png")

# ── Plot 2: Time vs Problem Size (sequential vs parallel) ─────
fig, ax = plt.subplots(figsize=(8, 5))

seq_times = [data[n]['seq_time'] for n in sizes]
ax.plot(sizes, seq_times, marker='s', color='red',
        linewidth=2, label='Sequential', linestyle='--')

for p_target, marker in [(2, 'o'), (4, '^'), (8, 'D')]:
    times_at_p = []
    for n in sizes:
        idx = data[n]['p'].index(p_target)
        times_at_p.append(data[n]['time'][idx])
    ax.plot(sizes, times_at_p, marker=marker, linewidth=2, label=f'Parallel P={p_target}')

ax.set_xlabel('Problem Size (N)', fontsize=12)
ax.set_ylabel('Execution Time (seconds)', fontsize=12)
ax.set_title('Execution Time vs Problem Size\n(Sequential vs Parallel)', fontsize=13)
ax.legend(fontsize=10)
ax.grid(True, linestyle='--', alpha=0.5)
plt.tight_layout()
plt.savefig('scaling_plot.png', dpi=150)
plt.close()
print("Saved scaling_plot.png")
