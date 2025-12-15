import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# -------------------------------------------------
# Load data and define real world data
# -------------------------------------------------
DATA_CSVNAMES = ["MUC16_31m", "covid_31m", "titin_31m", "nebulin_31m"]
df1 = pd.read_csv("../results/" + DATA_CSVNAMES[0] + ".csv").copy()
df2 = pd.read_csv("../results/" + DATA_CSVNAMES[1] + ".csv").copy()
df3 = pd.read_csv("../results/" + DATA_CSVNAMES[2] + ".csv").copy()
df4 = pd.read_csv("../results/" + DATA_CSVNAMES[3] + ".csv").copy()
dfs = [df1, df2, df3, df4]

m_values = [14507, 1273, 34350, 8525]

SAVE_NAME = '_'.join(DATA_CSVNAMES)
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

for df in dfs:
    df["short_algorithm_name"] = df["algo_name"].map(name_map)

# approx_algos = ["KP Projection", "Heuristic 1", "Heuristic 2", "Heuristic 3"]
approx_algos = ["Heuristic TP", "Heuristic TP w/ Magic Short Circuit", "KP Projection", "KP w/ Short Circuit", "KP w/ Magic Short Circuit"]
exact_algos  = ["Brute Force", "Sqrt"]
all_algos = exact_algos + approx_algos

# Baseline settings for the slices
baseline_n      = 31168785
baseline_sigma  = 22
baseline_eps    = 0.2


# -------------------------------------------------
# Helper to plot group on a given axis
# -------------------------------------------------
def plot_group_on_ax(ax, data, algos, x_col, metric,
                     x_label, title, log_x=False, log_y=False, xticks=None, xticklabels=None):

    for algo in algos:
        cur = data[data["short_algorithm_name"] == algo].sort_values(x_col)
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

RIGHT_MARGIN = 0.8
BOTTOM_MARGIN = 0.12
LEGEND_LOC = "lower center"
TIME = "avg_time"
RATIO = "avg_ratio"

# =====================================================
# FIGURE 1 — Approx algos: AVG TIME panels
# =====================================================
fig1, axes1 = plt.subplots(2, 2, figsize=(11, 8))
ax_poses = [(0,0), (0,1), (1,0), (1,1)]
for df, pos, dataset_name, m in zip(dfs, ax_poses, DATA_CSVNAMES, m_values):
    plot_group_on_ax(
        axes1[pos], df, all_algos,
        x_col="eps",
        metric=TIME,
        x_label="eps",
        title=f"{dataset_name}".replace("_31m", "") + f" (n={baseline_n}, m={m})",
        log_x=False, log_y=True,
    )

handles, labels = axes1[0, 0].get_legend_handles_labels()
fig1.suptitle(f"Runtime vs ε on Real World Datasets", y=0.98)

fig1.tight_layout()

# fig1.subplots_adjust(right=RIGHT_MARGIN)
fig1.subplots_adjust(bottom=BOTTOM_MARGIN)

fig1.legend(
    handles, labels,
    loc=LEGEND_LOC,
    # ncol=1,
    ncol=4,
)

fig1.savefig("../figures/runtime_" + SAVE_NAME + ".png", dpi=300, bbox_inches="tight")
plt.show()
