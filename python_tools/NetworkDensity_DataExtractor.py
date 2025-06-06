import os
import matplotlib.pyplot as plt
import numpy as np

def extract_metric(file_path, metric_name):
    with open(file_path, 'r') as file:
        for line in file:
            if metric_name in line:
                parts = line.split(':')
                if len(parts) > 1:
                    metric_value = parts[1].strip().split()[0]  # Extract the numerical value
                    return float(metric_value)
    return None

def calculate_average_metric_in_folder(folder_path, metric_name):
    metric_values = []
    results = {}
    
    for filename in os.listdir(folder_path):
        file_path = os.path.join(folder_path, filename)
        if os.path.isfile(file_path):
            metric_value = extract_metric(file_path, metric_name)
            
            # Ignore NaN values
            if metric_value is not None and not np.isnan(metric_value):
                results[filename] = metric_value
                metric_values.append(metric_value)

    # Filter out extreme values based on predefined thresholds
    if len(metric_values) > 0:
        lower_bound = np.percentile(metric_values, 5)
        upper_bound = np.percentile(metric_values, 95)
        
        filtered_metric_values = [value for value in metric_values if lower_bound <= value <= upper_bound]
        
        average_metric = sum(filtered_metric_values) / len(filtered_metric_values) if filtered_metric_values else None
        
        return average_metric, filtered_metric_values, results
    
    return None, [], results

# Define folder paths and metrics
current_directory = os.path.dirname(os.path.abspath(__file__))
print(current_directory)
base_folder_path = current_directory  # Change this if necessary
nSta_values = ["1", "2", "3", "4", "5", "6", "7", "8"]

metrics = {
    "latency": "average latency",
}

metric_data = {metric: {} for metric in metrics}
average_latency_values_ms = []  # Store average latency in ms

# Open the summary file for writing
summary_file_path = os.path.join(current_directory, 'Summary_all_Result.txt')
with open(summary_file_path, 'w') as summary_file:
    for nSta in nSta_values:
        folder_path = os.path.join(base_folder_path, f"nSta={nSta}")
        
        if not os.path.exists(folder_path):
            summary_file.write(f"Folder does not exist: {folder_path}\n")
            continue
        
        summary_file.write(f"\nProcessing folder: {folder_path}\n")
        
        for metric, metric_name in metrics.items():
            average_metric, metric_values, results = calculate_average_metric_in_folder(folder_path, metric_name)
            metric_data[metric][nSta] = metric_values
            
            if average_metric is not None:
                average_latency_ms = average_metric * 1000  # Convert to ms
                summary_file.write(f"Average {metric.capitalize()} from all files in folder: {average_latency_ms:.8f} ms\n")
                average_latency_values_ms.append(average_latency_ms)  # Store converted value.
            else:
                summary_file.write(f"No valid {metric} data found in the files in the folder.\n")

# Set desired font sizes here
label_fontsize = 24   # Font size for labels
axis_fontsize = 20    # Font size for axis ticks

# Plotting each violin plot in its own figure
for metric in metrics:
    plt.figure()
    
    data_to_plot_ms_combined = [[value * 1000 for value in metric_data[metric][nSta]] for nSta in nSta_values]

    # Create a violin plot instead of a box plot
    plt.violinplot(data_to_plot_ms_combined)

    plt.xlabel('Number of Station', fontsize=label_fontsize) 
    plt.ylabel('Latency (ms)', fontsize=label_fontsize)

    plt.xticks(ticks=np.arange(1,len(nSta_values)+1), labels=nSta_values)   # Set x-tick labels
    
    plt.xticks(fontsize=axis_fontsize)   # Adjust x-axis tick font size
    plt.yticks(fontsize=axis_fontsize)   # Adjust y-axis tick font size
    
    plt.tight_layout()
    
    plot_filename_violinplot = f'{metric.capitalize()}_Violin_Plot.png'
    plt.savefig(plot_filename_violinplot, dpi=1200)
    
# Plotting latency comparison integrated with a violin plot.
plt.figure(figsize=(8, 6))

data_to_plot_ms_combined_final = [[value * 1000 for value in metric_data["latency"][nSta]] for nSta in nSta_values]
plt.violinplot(data_to_plot_ms_combined_final , showmeans=True)

plt.xlabel('Number of Station', fontsize=label_fontsize)
plt.ylabel('Latency (ms)', fontsize=label_fontsize)

plt.xticks(ticks=np.arange(1,len(nSta_values)+1), labels=nSta_values)   # Set x-tick labels

plt.xticks(fontsize=axis_fontsize)   # Adjust x-axis tick font size
plt.yticks(fontsize=axis_fontsize)   # Adjust y-axis tick font size

plt.grid(True, linestyle='--', alpha=0.6)
plt.tight_layout()
plt.savefig('latency_violin_plot.png', dpi=1200)

# Plotting the average latency.
plt.figure(figsize=(8, 6))
plt.plot(range(1, len(nSta_values) + 1), average_latency_values_ms,
         marker='o', linestyle='--', color='g', label='Average Latency (ms)')
         
plt.xlabel('Number of Station', fontsize=label_fontsize)
plt.ylabel('Average Latency (ms)', fontsize=label_fontsize)

plt.grid(axis='y', linestyle='--', alpha=0.6)  
plt.xticks(fontsize=axis_fontsize)     # Adjust x-axis tick font size 
plt.yticks(fontsize=axis_fontsize)     # Adjust y-axis tick font size 

avg_latency_plot_filename_v2 = 'average_latency_plot_test.png'
plt.savefig(avg_latency_plot_filename_v2, dpi=1200)

print(f'Saved plot: {avg_latency_plot_filename_v2}')  

# Show plots (optional).
