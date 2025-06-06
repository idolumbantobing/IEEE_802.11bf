
import os
import matplotlib.pyplot as plt

def extract_failure_percentages(summary_file):
    failure_percentages = {}
    current_rate = None

    with open(summary_file, 'r') as file:
        for line in file:
            line = line.strip()
            if line.startswith("rate="):
                current_rate = int(line.split('=')[1].replace(':', ''))
            elif "Failure Percentage" in line and current_rate is not None:
                percentage = float(line.split(":")[1].strip().replace('%', ''))
                failure_percentages[current_rate] = percentage
    return failure_percentages

# List of AP folders
ap_folders = ['AP=1', 'AP=3', 'AP=5']
failure_data = {}

for folder in ap_folders:
    summary_path = os.path.join(folder, 'Successful_Sensing_Summary.txt')
    if os.path.exists(summary_path):
        failure_data[folder] = extract_failure_percentages(summary_path)
    else:
        print(f"Warning: {summary_path} not found.")


# Plotting
plt.figure(figsize=(12, 6))
markers = {'AP=1': 'g-^', 'AP=3': 'b-s', 'AP=5': 'r-o'}
labels = {'AP=1': 'scenario of 1 AP with 8 STAs', 'AP=3': 'scenario of 3 APs with 8 STAs each', 'AP=5': 'scenario of 5 APs with 8 STAs each'}

label_fontsize = 24   # Font size for labels
axis_fontsize = 20    # Font size for axis ticks

for label, data in failure_data.items():
    x = sorted(data.keys())
    y = [data[k] for k in x]
    plt.plot(x, y, markers[label], label=labels[label])

plt.xlabel('Sensing Interval (ms)', fontsize=label_fontsize)
plt.ylabel('Failed Procedure Percentage (%)', fontsize=label_fontsize)
plt.title('Failure Percentage vs Sensing Interval', fontsize=label_fontsize)

x_ticks = x  # All ticks
x_labels = [str(x) if x % 10 == 0 or x == 100 else '' for x in x_ticks]  # Hide labels for 11-19
plt.xticks(ticks=x_ticks, labels=x_labels, fontsize=axis_fontsize)  # Set x-tick labels
plt.yticks(fontsize=axis_fontsize)

plt.grid(True, linestyle='--', alpha=0.7)
plt.legend()
plt.tight_layout()
plt.savefig('comparison_graph.png', dpi=500)
plt.show()


