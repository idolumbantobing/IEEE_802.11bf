
# importing the required module
import matplotlib.pyplot as plt
import numpy as np
 
# # x axis values
# x = [0,6,8,10]
# # corresponding y axis values
# y = [0, 0.000411, 0.001943, 0.002655]
 
# # plotting the points 
# plt.plot(x, y)
 
# # naming the x axis
# plt.xlabel('x - axis')
# # naming the y axis
# plt.ylabel('y - axis')
 
# # giving a title to my graph
# plt.title('My first graph!')
 
# # function to show the plot
# plt.show()

# Sample data
categories = ['1m', '10m', '20m', '30m', '40m', '50m']
show_values2 = True
# values with MU OFDMA
values = [0.00514011, 0.00514074, 0.00514127, 0.00514202, 0.00514250, 0.00514316]
values2 = [0.00771245, 0.00771341, 0.00771420, 0.00771549, 0.00771625, 0.00771703]
values3 = [0.01028454, 0.01028594, 0.01028702, 0.01028781, 0.01028961, 0.01029123]
values4 = [0.01286282, 0.01286476, 0.01286617, 0.01286765, 0.01286862, 0.01287048]

values_microsec = values
# values_microsec = [x * 1e3 for x in values]
# values_microsec = [round(num, 2) for num in values_microsec]

# values without MU OFDMA
# values2 = [0.4096, 0.6144, 0.8192, 1.024]
# values_microsec2 = values2
# values_microsec2 = [x * 1e3 for x in values2]
# values_microsec2 = [round(num, 2) for num in values_microsec2]

# Width of each bar
bar_width = 0.25
spacing = 0.2  # Spacing between groups of bars

# Set position of bars on X axis
r1 = np.arange(len(categories)) * (4 * bar_width + spacing)
r2 = [x + bar_width for x in r1]
r3 = [x + bar_width for x in r2]
r4 = [x + bar_width for x in r3]

# Plotting the bar chart
if(show_values2):  
    bars1 = plt.bar(r1, values, color='skyblue', width=bar_width, label='average latency from 25 seeds, MU-OFDMA only, nSta=4')
    bars2 = plt.bar(r2, values2, color='orange', width=bar_width, label='average latency from 25 seeds, MU-OFDMA only, nSta=6')
    bars3 = plt.bar(r3, values3, color='green', width=bar_width, label='average latency from 25 seeds, MU-OFDMA only, nSta=8')
    bars4 = plt.bar(r4, values4, color='purple', width=bar_width, label='average latency from 25 seeds, MU-OFDMA only, nSta=10')
else:
    bars1 = plt.bar(categories, values_microsec, color='skyblue', width=bar_width, label='average latency from 25 seeds, MU-OFDMA only, nSta=4')

# Adding labels and title
plt.xlabel('Distance')
plt.ylabel('Times in Seconds') 
plt.title('Time taken until next sensing participation')
if(show_values2): plt.xticks([r + 1.5 * bar_width for r in r1], categories)

# Rotating x-axis labels if needed
plt.xticks(rotation=0)  # Adjust angle as needed

for bar, values in zip(bars1, values):
    plt.text(bar.get_x() + bar.get_width() / 2, bar.get_height(), str(values),
             ha='center', va='bottom')
             
if(show_values2): 
    for bar, values2 in zip(bars2, values2):
        plt.text(bar.get_x() + bar.get_width() / 2, bar.get_height(), str(values2),
             ha='center', va='bottom')
    for bar, values3 in zip(bars3, values3):
        plt.text(bar.get_x() + bar.get_width() / 2, bar.get_height(), str(values3),
             ha='center', va='bottom')
    for bar, values4 in zip(bars4, values4):
        plt.text(bar.get_x() + bar.get_width() / 2, bar.get_height(), str(values4),
             ha='center', va='bottom')

# Displaying the plot
# plt.ylim(min(values)-1e-6, max(values)+1e-6)
plt.legend()
plt.grid(axis='y')  # Optionally add gridlines
plt.legend(loc='center left', bbox_to_anchor=(1, 0.5))

# Manually adjust subplot parameters to ensure legend is not cropped
plt.subplots_adjust(right=0.75)
plt.tight_layout(rect=[0, 0, 0.85, 1])  # Adjust layout to prevent clipping of labels
plt.show()
