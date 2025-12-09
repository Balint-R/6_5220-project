import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

df = pd.read_csv("../build/results.csv").copy()

name_map = {
    "Projection to Binary Alphabet": "Projection",
    "Heuristic 1: sum(bucket_mass)^2": "Heuristic 1",
    "Heuristic 2: sum(max freq over windows)^2": "Heuristic 2",
    "Heuristic 3: sum(weighted_bucket_mass)^2": "Heuristic 3",
    "Brute Force": "Brute Force",
    "Sqrt": "Sqrt",
}

df["short_name"] = df["algo_name"].map(name_map)

approx_algos = ["Projection", "Heuristic 1", "Heuristic 2", "Heuristic 3"]
exact_algos  = ["Brute Force", "Sqrt"]

# approx_algos = [
#     "Projection to Binary Alphabet",
#     "Heuristic 1: sum(bucket_mass)^2",
#     "Heuristic 2: sum(max freq over windows)^2",
#     "Heuristic 3: sum(weighted_bucket_mass)^2",
# ]

# helpful: m/n
df["m_frac"] = df["m"] / df["n"]

baseline_n = 1_000_000
baseline_sigma = 1000
baseline_eps = 0.1
baseline_m_frac = 0.1

# ---------------------------------------------
# Helper to plot a group on a given axis
# ---------------------------------------------
def plot_group_on_ax(ax, data, algos, x_col, metric,
                     x_label, title, log_x=False, log_y=False):

    for algo in algos:
        cur = data[data["algo_name"] == algo].sort_values(x_col)
        if cur.empty:
            continue
        ax.plot(cur[x_col], cur[metric], marker="o", label=algo)

    # log scaling
    if log_x:
        ax.set_xscale("log")
    if log_y:
        ax.set_yscale("log")

    # labels / titles
    ax.set_xlabel(x_label)
    ax.set_ylabel(metric)
    ax.set_title(title)

    ax.margins(y=0.25)
    ax.grid(True, linestyle="--", alpha=0.5)

def plot_group_on_ax(ax, data, algos, x_col, metric,
                     x_label, title, log_x=False, log_y=False):

    for algo in algos:
        cur = data[data["short_name"] == algo].sort_values(x_col)
        if cur.empty:
            continue
        ax.plot(cur[x_col], cur[metric], marker="o", label=algo)

    if log_x:
        ax.set_xscale("log")
    if log_y:
        ax.set_yscale("log")

    ax.set_xlabel(x_label)
    ax.set_ylabel(metric)
    ax.set_title(title)
    ax.margins(y=0.25)
    ax.grid(True, linestyle="--", alpha=0.5)



# =====================================================
# FIGURE 1 — AVG TIME (approx algorithms)
# =====================================================
fig1, axes1 = plt.subplots(2, 2, figsize=(11, 8))

sub_m = df[(df["n"] == baseline_n) &
           (df["sigma"] == baseline_sigma) &
           (np.isclose(df["eps"], baseline_eps))]

sub_n = df[(np.isclose(df["m_frac"], baseline_m_frac)) &
           (df["sigma"] == baseline_sigma) &
           (np.isclose(df["eps"], baseline_eps))]

sub_sigma = df[(df["n"] == baseline_n) &
               (np.isclose(df["m_frac"], baseline_m_frac)) &
               (np.isclose(df["eps"], baseline_eps))]

sub_eps = df[(df["n"] == baseline_n) &
             (np.isclose(df["m_frac"], baseline_m_frac)) &
             (df["sigma"] == baseline_sigma)]

plot_group_on_ax(
    axes1[0, 0], sub_m, approx_algos,
    x_col="m",
    metric="avg_time",
    x_label="m",
    title=f"Runtime vs m (n={baseline_n}, σ={baseline_sigma}, ε={baseline_eps})",
    log_x=True, log_y=True,
)

plot_group_on_ax(
    axes1[0, 1], sub_n, approx_algos,
    x_col="n",
    metric="avg_time",
    x_label="n",
    title=f"Runtime vs n (m={baseline_m_frac}·n)",
    log_x=True, log_y=True,
)

plot_group_on_ax(
    axes1[1, 0], sub_sigma, approx_algos,
    x_col="sigma",
    metric="avg_time",
    x_label="σ",
    title=f"Runtime vs σ",
    log_x=True, log_y=True,
)

plot_group_on_ax(
    axes1[1, 1], sub_eps, approx_algos,
    x_col="eps",
    metric="avg_time",
    x_label="ε",
    title=f"Runtime vs ε",
    log_x=False, log_y=True,
)

handles, labels = axes1[0,0].get_legend_handles_labels()

fig1.legend(handles, labels, loc="upper center", ncol=len(approx_algos))
fig1.suptitle("Approximate Algorithms: Runtime Panels", y=1.03)
fig1.tight_layout()
plt.show()


# =====================================================
# FIGURE 2 — AVG RATIO (approx algorithms)
# =====================================================
fig2, axes2 = plt.subplots(2, 2, figsize=(11, 8))

plot_group_on_ax(
    axes2[0, 0], sub_m, approx_algos,
    x_col="m", metric="avg_ratio",
    x_label="m",
    title="Approx ratio vs m",
    log_x=True, log_y=False
)
plot_group_on_ax(
    axes2[0, 1], sub_n, approx_algos,
    x_col="n", metric="avg_ratio",
    x_label="n",
    title="Approx ratio vs n",
    log_x=True, log_y=False
)
plot_group_on_ax(
    axes2[1, 0], sub_sigma, approx_algos,
    x_col="sigma", metric="avg_ratio",
    x_label="σ",
    title="Approx ratio vs σ",
    log_x=True, log_y=False
)
plot_group_on_ax(
    axes2[1, 1], sub_eps, approx_algos,
    x_col="eps", metric="avg_ratio",
    x_label="ε",
    title="Approx ratio vs ε",
    log_x=False, log_y=False
)

handles2, labels2 = axes2[0,0].get_legend_handles_labels()

fig2.legend(handles2, labels2, loc="upper center", ncol=len(approx_algos))
fig2.suptitle("Approximate Algorithms: Approximation Ratio Panels", y=1.03)
fig2.tight_layout()
plt.show()
