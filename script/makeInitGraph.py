import numpy as np
import matplotlib.pyplot as plt

def drawGraph():

    plt.figure(figsize=(11,3))
    plt.fill_between([0.0], 0.0, 0.0, color="blue", alpha=0.1, label="variance")
    plt.plot([0.0], [0.0], color="b", alpha=0.5, label="reliability")
    plt.ylim([0.0, 1.0])
    plt.xlim([0, 10])
    plt.tight_layout(pad=2)
    plt.legend(loc="upper left")
    plt.xlabel("Iteration")
    plt.ylabel("Reliability")
    plt.yticks(np.arange(0.0, 1.1, 0.1))
    plt.xticks(np.arange(0, 11, 1))
    plt.show()

drawGraph()