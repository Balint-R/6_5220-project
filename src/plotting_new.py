import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# -------------------------------------------------
# Load data and define short algorithm names
# -------------------------------------------------
GEN_NAME = "cyclic"
df = pd.read_csv("../results/" + GEN_NAME + ".csv").copy()

name_map = {
    "Brute force": "Brute Force",
    "Brute force fast": "AVX Optimized Brute Force",
    "Projection to 2/eps alphabet": "KP Projection",
    "Sqrt": "Sqrt",
    "Heuristic 1: TP score": "Heuristic TP",
    "Heuristic 1: TP score with magic short circuit": "Heuristic TP w/ Magic Short Circuit",
    "Projection to 2/eps alphabet with short circuit": "KP w/ Short Circuit",
    "Projection to 2/eps alphabet with magic short circuit": "KP w/ Magic Short Circuit",
}

df["short_name"] = df["algo_name"].map(name_map)

# approx_algos = ["KP Projection", "Heuristic 1", "Heuristic 2", "Heuristic 3"]
approx_algos = ["Heuristic TP", "Heuristic TP w/ Magic Short Circuit", "KP Projection", "KP w/ Short Circuit", "KP w/ Magic Short Circuit"]
exact_algos  = ["Brute Force", "Sqrt"]

# approx_algos = ["KP w/ Short Circuit", "KP w/ Magic Short Circuit"]
# exact_algos  = []

# helpful: m/n
df["m_frac"] = df["m"] / df["n"]

# Baseline settings for the slices
baseline_n      = 1000000
baseline_sigma  = 100
baseline_eps    = 0.2
baseline_m_frac = 0.2 if GEN_NAME == "uniform" else 0.02


# -------------------------------------------------
# Helper to plot group on a given axis
# -------------------------------------------------
def plot_group_on_ax(ax, data, algos, x_col, metric,
                     x_label, title, log_x=False, log_y=False, xticks=None, xticklabels=None):

    for algo in algos:
        cur = data[data["short_name"] == algo].sort_values(x_col)
        # print(cur)
        if cur.empty:
            continue
        ax.plot(cur[x_col], cur[metric], marker="o", label=algo)

    if log_x:
        ax.set_xscale("log")
    if log_y:
        ax.set_yscale("log")

    if xticks is not None:
        ax.set_xticks(xticks)
        ax.set_xticklabels(xticklabels)

    ax.set_xlabel(x_label)

    if metric == "avg_time":
        ax.set_ylabel("Average runtime (s)")
    elif metric == "median_time":
        ax.set_ylabel("Median runtime (s)")
    elif metric == "avg_ratio":
        ax.set_ylabel("Average approximation ratio")
    else:
        ax.set_ylabel(metric)

    # ax.set_ylabel(metric)
    ax.set_title(title)
    ax.margins(y=0.25)
    ax.grid(True, linestyle="--", alpha=0.5)



# Precompute slices used in several figures
sub_m = df[(df["n"] == baseline_n) &
           (df["sigma"] == baseline_sigma) &
           (np.isclose(df["eps"], baseline_eps))]
# print(sub_m.head())

sub_n = df[(np.isclose(df["m_frac"], baseline_m_frac)) &
           (df["sigma"] == baseline_sigma) &
           (np.isclose(df["eps"], baseline_eps))]
# print(sub_n.head())

sub_sigma = df[(df["n"] == baseline_n) &
               (np.isclose(df["m_frac"], baseline_m_frac)) &
               (np.isclose(df["eps"], baseline_eps))]

sub_eps = df[(df["n"] == baseline_n) &
             (np.isclose(df["m_frac"], baseline_m_frac)) &
             (df["sigma"] == baseline_sigma)]

RIGHT_MARGIN = 0.85
LEGEND_LOC = "upper right"

# =====================================================
# FIGURE 1 — Approx algos: AVG TIME panels
# =====================================================
fig1, axes1 = plt.subplots(2, 2, figsize=(11, 8))
# custom_ticks = [5e4, 1e5, 5e5]
# custom_labels = [rf"$5\cdot10^{4}$", rf"$10^{5}$", rf"$5\cdot10^{5}$"]
plot_group_on_ax(
    axes1[0, 0], sub_m, exact_algos + approx_algos,
    x_col="m",
    metric="median_time",
    x_label="m",
    title=f"Runtime vs m (n={baseline_n}, Sigma={baseline_sigma}, ε={baseline_eps})",
    log_x=True, log_y=True,
    # xticks=custom_ticks,
    # xticklabels=custom_labels
)

plot_group_on_ax(
    axes1[0, 1], sub_n, exact_algos + approx_algos,
    x_col="n",
    metric="median_time",
    x_label="n",
    title=f"Runtime vs n (m={baseline_m_frac}n, Sigma={baseline_sigma}, ε={baseline_eps})",
    log_x=True, log_y=True,
)

plot_group_on_ax(
    axes1[1, 0], sub_sigma, exact_algos + approx_algos,
    x_col="sigma",
    metric="median_time",
    x_label="Sigma",
    title=f"Runtime vs Sigma (n={baseline_n}, m={baseline_m_frac}n, ε={baseline_eps})",
    log_x=True, log_y=True,
)

plot_group_on_ax(
    axes1[1, 1], sub_eps, exact_algos + approx_algos,
    x_col="eps",
    metric="median_time",
    x_label="ε",
    title=f"Runtime vs ε (n={baseline_n}, m={baseline_m_frac}n, Sigma={baseline_sigma})",
    log_x=False, log_y=True,
)

handles, labels = axes1[0, 0].get_legend_handles_labels()
fig1.suptitle(f"Runtime on {GEN_NAME}", y=0.98)

fig1.tight_layout()

fig1.subplots_adjust(right=RIGHT_MARGIN)
fig1.legend(
    handles, labels,
    loc=LEGEND_LOC,
    ncol=1,
)

fig1.savefig("../figures/runtime_" + GEN_NAME + ".png", dpi=300, bbox_inches="tight")
plt.show()
