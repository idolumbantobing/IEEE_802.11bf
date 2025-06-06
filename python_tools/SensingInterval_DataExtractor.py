import os
import matplotlib.pyplot as plt
import numpy as np

def extract_metric(file_path, metric_name):
    with open(file_path, 'r') as file:
        for line in file:
            if metric_name in line:
                parts = line.split(':')
                if len(parts) > 1:
                    metric_value_str = parts[1].strip().split()[0]  # Extract the numerical value
                    try:
                        metric_value = float(metric_value_str)
                        return metric_value if not np.isnan(metric_value) else None
                    except ValueError:
                        return None
    return None

def calculate_average_metric_in_folder(folder_path, metric_name):
    metric_values = []
    for filename in os.listdir(folder_path):
        file_path = os.path.join(folder_path, filename)
        if os.path.isfile(file_path):
            metric_value = extract_metric(file_path, metric_name)
            if metric_value is not None:
                metric_values.append(metric_value)
    return metric_values  # Return the list of metric values

def scale_latency(latency_values):
    if not latency_values:
        return [], 'Latency (s)'  # Handle empty list case

    max_latency = max(latency_values)
    if max_latency < 1e-3:
        # Convert to microseconds
        return [latency * 1e6 for latency in latency_values], 'Latency (µs)'
    elif max_latency < 1:
        # Convert to milliseconds
        return [latency * 1e3 for latency in latency_values], 'Latency (ms)'
    else:
        # Keep in seconds
        return latency_values, 'Latency (s)'

def remove_outliers(data):
    if not data:
        return data  # Handle empty list case

    Q1 = np.percentile(data, 5)
    Q3 = np.percentile(data, 95)
    IQR = Q3 - Q1
    lower_bound = Q1 - 1.5 * IQR
    upper_bound = Q3 + 1.5 * IQR
    return [x for x in data if lower_bound <= x <= upper_bound]

def calculate_sensing_metrics(folder_path):
    successful_sensing = []
    unsuccessful_sensing = []
    for filename in os.listdir(folder_path):
        file_path = os.path.join(folder_path, filename)
        if os.path.isfile(file_path):
            successful = extract_metric(file_path, "successful sensing")
            unsuccessful = extract_metric(file_path, "unsuccessfull sensing")
            if successful is not None:
                successful_sensing.append(successful)
            if unsuccessful is not None:
                unsuccessful_sensing.append(unsuccessful)
    return successful_sensing, unsuccessful_sensing

# Define base folder path and metrics
current_directory = os.path.dirname(os.path.abspath(__file__))
base_folder_path = current_directory

# Sensing intervals (or rates) - Include values from 10 to 50 in steps of 10, and 11-19
rate_values = [str(i) for i in range(10, 51, 10)] + [str(i) for i in range(11, 20)] + [str(i) for i in range(1, 10)]
rate_values = sorted(rate_values, key=int)  # Sort numerically

metrics = {
    "latency": "average latency",
}

# Prepare lists to store average latencies without outliers 
average_latencies_no_outliers = []

# Read data from folders corresponding to each rate value 
for rate in rate_values:
    folder_path = os.path.join(base_folder_path, f"rate={rate}")
    
    latency_values = calculate_average_metric_in_folder(folder_path, metrics["latency"])
    
    # Remove outliers before calculating the average 
    cleaned_data_no_outliers = remove_outliers(latency_values)
    
    # Calculate average without outliers 
    avg_latency_no_outlier_final = sum(cleaned_data_no_outliers) / len(cleaned_data_no_outliers) if cleaned_data_no_outliers else None
    
    average_latencies_no_outliers.append(avg_latency_no_outlier_final)

# Scale latencies 
scaled_average_latencies_no_outliers, y_label = scale_latency(average_latencies_no_outliers)

# Convert rate_values to integers for proper x-axis scaling
rate_values_int = [int(rate) for rate in rate_values]

# Generate a Python-compatible string representation of the data
data_string = "average_latency_data = {\n"
for rate, latency in zip(rate_values_int, scaled_average_latencies_no_outliers):
    data_string += f"    {rate}: {latency},\n"
data_string += "}"

# Save the Python-compatible string to a .txt file
output_file_path = os.path.join(current_directory, 'average_latency_data.txt')
with open(output_file_path, 'w') as file:
    file.write(data_string)

print(f'Saved average latency data to: {output_file_path}')

# Plotting Average Latency using Line Plot 
plt.figure(figsize=(12, 6))
plt.plot(rate_values_int,
         scaled_average_latencies_no_outliers,
         marker='o',
         linestyle='-',
         color='g',
         label='Average Latency Without Outliers')

plt.xlabel('Sensing Interval (ms)', fontsize=16)
plt.ylabel(y_label, fontsize=16)
plt.title('Average Latency vs Sensing Interval', fontsize=18)

# Customize x-axis ticks and labels
# Show labels for 10, 20, 30, ..., 100, and hide labels for 11-19
x_ticks = rate_values_int  # All ticks
x_labels = [str(x) if x % 10 == 0 or x == 100 else '' for x in x_ticks]  # Hide labels for 11-19

plt.xticks(ticks=x_ticks, labels=x_labels, rotation=45)  # Rotate labels for better readability
plt.grid(axis='y', linestyle='--', alpha=0.7)
plt.legend(fontsize=14)

plt.tight_layout()
avg_latency_plot_filename_lineplot = 'average_latency_line_plot.png'
plt.savefig(avg_latency_plot_filename_lineplot, dpi=1200)

print(f'Saved plot: {avg_latency_plot_filename_lineplot}')  

# Define metrics for sensing
sensing_metrics = {
    "successful": "successful sensing",
    "unsuccessful": "unsuccessfull sensing",
}

# Store data for each metric
sensing_data = {metric: {} for metric in sensing_metrics}

# Open a summary file for writing
with open('Successful_Sensing_Summary.txt', 'w') as summary_file:
    summary_file.write("#" * 75 + "\n")
    summary_file.write("Summary of Sensing Analysis\n")
    summary_file.write("#" * 75 + "\n\n")

    # Iterate over each rate value and collect data
    for rate in rate_values:
        folder_path = os.path.join(base_folder_path, f"rate={rate}")
        summary_file.write("*" * 75 + "\n")
        summary_file.write(f"Data for rate={rate}:\n")
        summary_file.write("*" * 75 + "\n")
        
        successful_sensing, unsuccessful_sensing = calculate_sensing_metrics(folder_path)
        
        sensing_data["successful"][rate] = successful_sensing
        sensing_data["unsuccessful"][rate] = unsuccessful_sensing
        
        summary_file.write(f"Successful Sensing Data: {successful_sensing}\n")
        summary_file.write(f"Unsuccessful Sensing Data: {unsuccessful_sensing}\n")
        summary_file.write("-" * 75 + "\n")
        
        summary_file.write("\n")

    # Calculate average values for sensing metrics and percentage failure
    average_successful = []
    average_unsuccessful = []
    percentage_failures = []

    summary_file.write("*" * 75 + "\n")
    summary_file.write("Averages and Failure Percentages:\n")
    summary_file.write("*" * 75 + "\n")

    for rate in rate_values:
        successful = sensing_data["successful"].get(rate, [])
        unsuccessful = sensing_data["unsuccessful"].get(rate, [])

        if successful:
            avg_successful = sum(successful) / len(successful)
            average_successful.append(avg_successful)
        else:
            avg_successful = 0
            average_successful.append(None)

        if unsuccessful:
            avg_unsuccessful = sum(unsuccessful) / len(unsuccessful)
            average_unsuccessful.append(avg_unsuccessful)
        else:
            avg_unsuccessful = 0
            average_unsuccessful.append(None)

        if avg_successful > 0:
            percentage_failure = (avg_unsuccessful / (avg_unsuccessful + avg_successful)) * 100
            percentage_failures.append(percentage_failure)
        else:
            percentage_failures.append(None)

        summary_file.write(f"rate={rate}:\n")
        summary_file.write(f"  Average Successful Sensing: {avg_successful:.8f}\n")
        summary_file.write(f"  Average Unsuccessful Sensing: {avg_unsuccessful:.8f}\n")
        summary_file.write(f"  Failure Percentage: {percentage_failure:.2f}%\n")
        summary_file.write("\n")

# Plotting
FONT_SIZE = 20
plt.figure(figsize=(18, 12))

# Prepare data for violin plots
successful_data = []
unsuccessful_data = []

for rate in rate_values:
    successful = sensing_data["successful"].get(rate, [])
    unsuccessful = sensing_data["unsuccessful"].get(rate, [])
    successful_data.append(successful if successful else [0])
    unsuccessful_data.append(unsuccessful if unsuccessful else [0])

# Plotting with violin plots
plt.subplot(3, 1, 1)
plt.violinplot(successful_data, showmeans=True, showmedians=True)
plt.title('Successful Sensing Distribution', fontsize=FONT_SIZE)
plt.xlabel('Sensing Interval (ms)', fontsize=FONT_SIZE)
plt.ylabel('Successful Sensing', fontsize=FONT_SIZE)
plt.xticks(ticks=range(1, len(rate_values) + 1), labels=rate_values, fontsize=FONT_SIZE)
plt.yticks(fontsize=FONT_SIZE)
plt.grid(axis='y', linestyle='--', alpha=0.7)

plt.subplot(3, 1, 2)
plt.violinplot(unsuccessful_data, showmeans=True, showmedians=True)
plt.title('Unsuccessful Sensing Distribution', fontsize=FONT_SIZE)
plt.xlabel('Sensing Interval (ms)', fontsize=FONT_SIZE)
plt.ylabel('Unsuccessful Sensing', fontsize=FONT_SIZE)
plt.xticks(ticks=range(1, len(rate_values) + 1), labels=rate_values, fontsize=FONT_SIZE)
plt.yticks(fontsize=FONT_SIZE)
plt.grid(axis='y', linestyle='--', alpha=0.7)

plt.subplot(3, 1, 3)
plt.plot(rate_values_int, percentage_failures, marker='o', color='blue')
plt.title('Average Failure Percentage', fontsize=FONT_SIZE)
plt.xlabel('Sensing Interval (ms)', fontsize=FONT_SIZE)
plt.ylabel('Failure Percentage (%)', fontsize=FONT_SIZE)
plt.xticks(fontsize=FONT_SIZE)
plt.yticks(fontsize=FONT_SIZE)
plt.grid(axis='y', linestyle='--', alpha=0.7)

plt.tight_layout()
plt.savefig('Successful_Sensing_KPI.png', dpi=1200)

FONT_SIZE = 20
plt.figure(figsize=(18, 12))
plt.plot(rate_values_int, percentage_failures, marker='o', color='blue')
plt.title('Average Failure Percentage', fontsize=FONT_SIZE)
plt.xlabel('Sensing Interval (ms)', fontsize=FONT_SIZE)
plt.ylabel('Failure Percentage (%)', fontsize=FONT_SIZE)
plt.xticks(fontsize=FONT_SIZE)
plt.yticks(fontsize=FONT_SIZE)
plt.grid(axis='y', linestyle='--', alpha=0.7)
plt.tight_layout()
plt.savefig('Unuccessful_Sensing_percentage.png', dpi=1200)

# Show plots (optional).
# plt.show()
