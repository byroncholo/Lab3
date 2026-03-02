


import pandas as pd
import matplotlib.pyplot as plt

# Load your CSV (edit path if needed)
csv_path = "hanoi_results.csv"
df = pd.read_csv(csv_path)

# Clean column names
df.columns = [c.strip() for c in df.columns]

# Detect columns (supports either "mean" or "actual" time CSVs)
rec_time_col = "rec_time_us" if "rec_time_us" in df.columns else "rec_mean_us"
it_time_col  = "it_time_us"  if "it_time_us"  in df.columns else "it_mean_us"
expected_col = "expected_moves" if "expected_moves" in df.columns else "expected"

# -------------------------
# Figure 1: Runtime vs n
# -------------------------
plt.figure()
plt.plot(df["n"], df[rec_time_col], marker="o")
plt.plot(df["n"], df[it_time_col], marker="o")
plt.xlabel("n (number of disks)")
plt.ylabel("Time (microseconds)")
plt.title("Runtime vs n")
plt.legend(["Recursive", "Iterative"])
plt.grid(True)
plt.tight_layout()
plt.show()

# -------------------------
# Figure 2: Runtime vs expected moves (2^n - 1)
# -------------------------
plt.figure()
plt.plot(df[expected_col], df[rec_time_col], marker="o")
plt.plot(df[expected_col], df[it_time_col], marker="o")
plt.xlabel("Expected moves (2^n - 1)")
plt.ylabel("Time (microseconds)")
plt.title("Runtime vs Expected Moves")
plt.legend(["Recursive", "Iterative"])
plt.grid(True)
plt.tight_layout()
plt.show()

# -------------------------
# Optional: Save figures as PNG
# -------------------------
# Uncomment if you want saved images
# plt.figure()
# plt.plot(df["n"], df[rec_time_col], marker="o")
# plt.plot(df["n"], df[it_time_col], marker="o")
# plt.xlabel("n (number of disks)")
# plt.ylabel("Time (microseconds)")
# plt.title("Towers of Hanoi: Runtime vs n")
# plt.legend(["Recursive", "Iterative"])
# plt.grid(True)
# plt.tight_layout()
# plt.savefig("runtime_vs_n.png", dpi=200)
# plt.close()
#
# plt.figure()
# plt.plot(df[expected_col], df[rec_time_col], marker="o")
# plt.plot(df[expected_col], df[it_time_col], marker="o")
# plt.xlabel("Expected moves (2^n - 1)")
# plt.ylabel("Time (microseconds)")
# plt.title("Towers of Hanoi: Runtime vs Expected Moves")
# plt.legend(["Recursive", "Iterative"])
# plt.grid(True)
# plt.tight_layout()
# plt.savefig("runtime_vs_expected_moves.png", dpi=200)
# plt.close()
