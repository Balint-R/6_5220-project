import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# -------------------------------------------------
# Load data and define short algorithm names
# -------------------------------------------------
df = pd.read_csv("../results/results_all_v1_no_outliers.csv").copy()

name_map = {
    "Projection to Binary Alphabet": "KP Projection",
    "Heuristic 1: sum(bucket_mass)^2": "Heuristic 1",
    "Heuristic 2: sum(max freq over windows)^2": "Heuristic 2",
    "Heuristic 3: sum(weighted_bucket_mass)^2": "Heuristic 3",
    "Brute Force": "Brute Force",
    "Sqrt": "Sqrt",
}

df["short_name"] = df["algo_name"].map(name_map)

approx_algos = ["KP Projection", "Heuristic 1", "Heuristic 2", "Heuristic 3"]
exact_algos  = ["Brute Force", "Sqrt"]

# helpful: m/n
df["m_frac"] = df["m"] / df["n"]

# Baseline settings for the slices
baseline_n = 1_000_000
baseline_sigma = 1000
baseline_eps = 0.1
baseline_m_frac = 0.2

# -------------------------------------------------
# Helper to plot group on a given axis
# -------------------------------------------------
def plot_group_on_ax(ax, data, algos, x_col, metric,
                     x_label, title, log_x=False, log_y=False, xticks=None, xticklabels=None):

    for algo in algos:
        cur = data[data["short_name"] == algo].sort_values(x_col)
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

sub_n = df[(np.isclose(df["m_frac"], baseline_m_frac)) &
           (df["sigma"] == baseline_sigma) &
           (np.isclose(df["eps"], baseline_eps))]

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
custom_ticks = [5e4, 1e5, 5e5]
custom_labels = [rf"$5\cdot10^{4}$", rf"$10^{5}$", rf"$5\cdot10^{5}$"]
plot_group_on_ax(
    axes1[0, 0], sub_m, approx_algos,
    x_col="m",
    metric="avg_time",
    x_label="m",
    title=f"Runtime vs m (n={baseline_n}, Sigma={baseline_sigma}, ε={baseline_eps})",
    log_x=True, log_y=True,
    xticks=custom_ticks,
    xticklabels=custom_labels
)

plot_group_on_ax(
    axes1[0, 1], sub_n, approx_algos,
    x_col="n",
    metric="avg_time",
    x_label="n",
    title=f"Runtime vs n (m={baseline_m_frac}n, Sigma={baseline_sigma}, ε={baseline_eps})",
    log_x=True, log_y=True,
)

plot_group_on_ax(
    axes1[1, 0], sub_sigma, approx_algos,
    x_col="sigma",
    metric="avg_time",
    x_label="Sigma",
    title=f"Runtime vs Sigma (n={baseline_n}, m={baseline_m_frac}n, ε={baseline_eps})",
    log_x=True, log_y=True,
)

plot_group_on_ax(
    axes1[1, 1], sub_eps, approx_algos,
    x_col="eps",
    metric="avg_time",
    x_label="ε",
    title=f"Runtime vs ε (n={baseline_n}, m={baseline_m_frac}n, Sigma={baseline_sigma})",
    log_x=False, log_y=True,
)

handles, labels = axes1[0, 0].get_legend_handles_labels()
fig1.suptitle("Approximate Algorithms: Runtime", y=0.98)

fig1.tight_layout()

fig1.subplots_adjust(right=RIGHT_MARGIN)
fig1.legend(
    handles, labels,
    loc=LEGEND_LOC,
    ncol=1,
)

fig1.savefig("../figures/approx_runtime.png", dpi=300, bbox_inches="tight")
# plt.show()



# =====================================================
# FIGURE 2 — Approx algos: AVG RATIO panels
# =====================================================
fig2, axes2 = plt.subplots(2, 2, figsize=(11, 8))

plot_group_on_ax(
    axes2[0, 0], sub_m, approx_algos,
    x_col="m",
    metric="avg_ratio",
    x_label="m",
    title=f"Approx ratio vs m (n={baseline_n}, Sigma={baseline_sigma}, ε={baseline_eps})",
    log_x=True, log_y=False,
    xticks=custom_ticks,
    xticklabels=custom_labels
)

plot_group_on_ax(
    axes2[0, 1], sub_n, approx_algos,
    x_col="n",
    metric="avg_ratio",
    x_label="n",
    title=f"Approx ratio vs n (m={baseline_m_frac}n, Sigma={baseline_sigma}, ε={baseline_eps})",
    log_x=True, log_y=False,
)

plot_group_on_ax(
    axes2[1, 0], sub_sigma, approx_algos,
    x_col="sigma",
    metric="avg_ratio",
    x_label="Sigma",
    title=f"Approx ratio vs Sigma (n={baseline_n}, m={baseline_m_frac}n, ε={baseline_eps})",
    log_x=True, log_y=False,
)

plot_group_on_ax(
    axes2[1, 1], sub_eps, approx_algos,
    x_col="eps",
    metric="avg_ratio",
    x_label="ε",
    title=f"Approx ratio vs ε (n={baseline_n}, m={baseline_m_frac}n, Sigma={baseline_sigma})",
    log_x=False, log_y=False,
)

handles2, labels2 = axes2[0, 0].get_legend_handles_labels()
fig2.suptitle("Approximate Algorithms: Approximation Ratio", y=0.98)
fig2.tight_layout()

fig2.subplots_adjust(right=RIGHT_MARGIN)
fig2.legend(
    handles2, labels2,
    loc=LEGEND_LOC,
    ncol=1,
)

fig2.savefig("../figures/approx_ratio.png", dpi=300, bbox_inches="tight")
# plt.show()


# =====================================================
# FIGURE 3 — Exact algos: AVG TIME panels (Brute + Sqrt)
# =====================================================
fig3, axes3 = plt.subplots(2, 2, figsize=(11, 8))

plot_group_on_ax(
    axes3[0, 0], sub_m, exact_algos,
    x_col="m",
    metric="avg_time",
    x_label="m",
    title=f"Exact runtime vs m (n={baseline_n}, Sigma={baseline_sigma}, ε={baseline_eps})",
    log_x=True, log_y=True,
    xticks=custom_ticks,
    xticklabels=custom_labels
)

plot_group_on_ax(
    axes3[0, 1], sub_n, exact_algos,
    x_col="n",
    metric="avg_time",
    x_label="n",
    title=f"Exact runtime vs n (m={baseline_m_frac}n, Sigma={baseline_sigma}, ε={baseline_eps})",
    log_x=True, log_y=True,
)

plot_group_on_ax(
    axes3[1, 0], sub_sigma, exact_algos,
    x_col="sigma",
    metric="avg_time",
    x_label="Sigma",
    title=f"Exact runtime vs Sigma (n={baseline_n}, m={baseline_m_frac}n, ε={baseline_eps})",
    log_x=True, log_y=True,
)

plot_group_on_ax(
    axes3[1, 1], sub_eps, exact_algos,
    x_col="eps",
    metric="avg_time",
    x_label="ε",
    title=f"Exact runtime vs ε (n={baseline_n}, m={baseline_m_frac}n, Sigma={baseline_sigma})",
    log_x=False, log_y=True,
)

handles3, labels3 = axes3[0, 0].get_legend_handles_labels()
fig3.suptitle("Exact Algorithms (Brute Force & Sqrt): Runtime", y=0.98)
fig3.tight_layout()

fig3.subplots_adjust(right=RIGHT_MARGIN)
fig3.legend(
    handles3, labels3,
    loc=LEGEND_LOC,
    ncol=1,
)

fig3.savefig("../figures/exact_runtime.png", dpi=300, bbox_inches="tight")
plt.show()
