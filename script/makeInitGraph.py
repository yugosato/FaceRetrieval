import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns

def drawGraph():
    sns.set_style("darkgrid")
    plt.figure(figsize=(16,3))
    plt.ylim([-0.02, 1.02])
    plt.xlim([0, 20])
    plt.yticks(np.arange(0.0, 1.1, 0.2))
    plt.xticks(np.arange(0, 21, 1))
    plt.fill_between([0.0], 0.0, 0.0, color="lightcoral", alpha=0.5, label="variance")
    plt.plot([0.0], [0.0], color="red", linewidth=1.5, label="reliability")
    plt.xlabel("Iteration", fontsize=14)
    plt.ylabel("Reliability", fontsize=14)
    plt.tick_params(labelsize=12)
    plt.tight_layout(pad=1)
    plt.legend(loc="lower right", fontsize=14)
    plt.savefig("/home/yugo/workspace/Interface/bin/data/items/init_graph.png")

drawGraph()