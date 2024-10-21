
import re

import tkinter as tk
from tkinter import ttk
import tkinter.messagebox as messagebox
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import numpy as np
import subprocess

def extract_float_from_time(time_string):
    # Define a regular expression pattern to match the float part of the time string
    pattern = r"[-+]?\d*\.\d+|\d+"
    # Use re.findall to find all matches of the pattern in the time string
    matches = re.findall(pattern, time_string)
    # Convert the first match to a float (assuming there's only one float in the string)
    if matches:
        return float(matches[0])
    else:
        return None


def program_calculator():
    def calculate():
        try:
            num1 = float(extract_float_from_time(entry1.get()))
            num2 = float(extract_float_from_time(entry2.get()))
            result = round((num1 - num2)*1e6, 1)
            output_label.config(text="Result: " + str(result) + " microseconds")
        except ValueError:
            output_label.config(text="Please enter valid numbers.")

    # Create the main window
    root = tk.Tk()
    root.title("Simple Calculator")

    # Create input fields
    entry1 = tk.Entry(root)
    entry1.pack(pady=5)
    entry2 = tk.Entry(root)
    entry2.pack(pady=5)

    # # Example usage:
    # time_string = "+1.04s"
    # float_value = extract_float_from_time(time_string)
    # print("Extracted float value:", float_value)

    # Create button to perform calculation
    calculate_button = tk.Button(root, text="Calculate", command=calculate)
    calculate_button.pack(pady=5)

    # Create label to display output
    output_label = tk.Label(root, text="")
    output_label.pack(pady=5)

    # Start the GUI event loop
    root.mainloop()

# def graph_maker():
#     # Sample data
#     categories = ['4', '6', '8', '10']
#     show_values2 = False
#     # values with MU OFDMA
#     Average1 = 0.0010746
#     Average2 = 0.0018498
#     Average3 = 0.0022239
#     Average4 = 0.0027125
#     values = [Average1, Average2, Average3, Average4]
#     values_microsec = [x * 1e3 for x in values]
#     values_microsec = [round(num, 2) for num in values_microsec]

#     # values without MU OFDMA
#     values2 = [0.4096, 0.6144, 0.8192, 1.024]
#     values_microsec2 = [x * 1e3 for x in values2]
#     values_microsec2 = [round(num, 2) for num in values_microsec2]

#     # Width of each bar
#     bar_width = 0.35

#     # Set position of bars on X axis
#     r1 = np.arange(len(categories))
#     r2 = [x + bar_width for x in r1]

#     # Plotting the bar chart
#     if(show_values2):  
#         bars1 = plt.bar(r1, values_microsec, color='skyblue', width=bar_width, label='With MU OFDMA')
#         bars2 = plt.bar(r2, values_microsec2, color='orange', width=bar_width, label='Without MU OFDMA')
#     else:
#         bars1 = plt.bar(categories, values_microsec, color='skyblue', width=bar_width, label='With MU OFDMA')

#     # Adding labels and title
#     plt.xlabel('Number of Stations')
#     plt.ylabel('Times in Milliseconds') 
#     plt.title('Time taken until next polling session')
#     if(show_values2): plt.xticks([r + bar_width / 2 for r in range(len(categories))], categories)
#     # Rotating x-axis labels if needed
#     plt.xticks(rotation=0)  # Adjust angle as needed

#     for bar, values_microsec in zip(bars1, values_microsec):
#         plt.text(bar.get_x() + bar.get_width() / 2, bar.get_height(), str(values_microsec),
#                 ha='center', va='bottom')
#     if(show_values2): 
#         for bar, values_microsec2 in zip(bars2, values_microsec2):
#             plt.text(bar.get_x() + bar.get_width() / 2, bar.get_height(), str(values_microsec2),
#                 ha='center', va='bottom')

#     # Displaying the plot
#     plt.legend()
#     plt.grid(axis='y')  # Optionally add gridlines
#     plt.tight_layout()  # Adjust layout to prevent clipping of labels
#     plt.show()

def plot_graph(values_microsec, values_microsec2, values_microsec3, categories, labels, show_values2, show_values3):
    # Width of each bar
    bar_width = 0.25

    # Set position of bars on X axis
    r1 = np.arange(len(categories))
    r2 = [x + bar_width for x in r1]
    r3 = [x + bar_width for x in r2]
        
    plt.figure(figsize=(8, 6))  # Adjust figure size if needed
    
    values_microsec = [x * 1e3 for x in values_microsec]
    values_microsec2 = [x * 1e3 for x in values_microsec2]
    values_microsec3 = [x * 1e3 for x in values_microsec3]

    bars1 = plt.bar(r1, values_microsec, color='skyblue', width=bar_width, label=labels[0])
    
    if show_values2:
        bars2 = plt.bar(r2, values_microsec2, color='orange', width=bar_width, label=labels[1])
    if show_values3:
        bars3 = plt.bar(r3, values_microsec3, color='green', width=bar_width, label=labels[2])

    # Adding labels and title
    plt.xlabel('Number of Stations')
    plt.ylabel('Times in Milliseconds') 
    plt.title('Time taken until next polling session')
    
    plt.xticks([r + bar_width for r in range(len(categories))], categories)
    # Rotating x-axis labels if needed
    plt.xticks(rotation=0)  # Adjust angle as needed

    for bar, value in zip(bars1, values_microsec):
        plt.text(bar.get_x() + bar.get_width() / 2, bar.get_height(), str(value),
                 ha='center', va='bottom')
    
    if show_values2:
        for bar, value in zip(bars2, values_microsec2):
            plt.text(bar.get_x() + bar.get_width() / 2, bar.get_height(), str(value),
                 ha='center', va='bottom')
    if show_values3:
        for bar, value in zip(bars3, values_microsec3):
            plt.text(bar.get_x() + bar.get_width() / 2, bar.get_height(), str(value),
                 ha='center', va='bottom')

    # Displaying the plot
    plt.legend()
    plt.grid(axis='y')  # Optionally add gridlines
    plt.tight_layout()  # Adjust layout to prevent clipping of labels
    plt.show()

def open_graph_maker():
    def plot():
        values_microsec = [float(val) for val in values_microsec_entry.get().split(",")]
        values_microsec2 = [float(val) for val in values_microsec2_entry.get().split(",")]
        values_microsec3 = [float(val) for val in values_microsec3_entry.get().split(",")]
        categories = categories_entry.get().split(",")
        labels = [label1_entry.get(), label2_entry.get(), label3_entry.get()]
        show_values2 = show_values2_var.get()
        show_values3 = show_values3_var.get()

        plot_graph(values_microsec, values_microsec2, values_microsec3, categories, labels, show_values2, show_values3)

    graph_maker_window = tk.Toplevel()
    graph_maker_window.title("Graph Maker")

    # Create input fields for values_microsec
    values_microsec_label = tk.Label(graph_maker_window, text="Values 1 (comma-separated):")
    values_microsec_label.grid(row=0, column=0, padx=5, pady=5)
    values_microsec_entry = ttk.Entry(graph_maker_window)
    values_microsec_entry.grid(row=0, column=1, padx=5, pady=5)

    # Create input fields for values_microsec2
    values_microsec2_label = tk.Label(graph_maker_window, text="Values 2 (comma-separated):")
    values_microsec2_label.grid(row=1, column=0, padx=5, pady=5)
    values_microsec2_entry = ttk.Entry(graph_maker_window)
    values_microsec2_entry.grid(row=1, column=1, padx=5, pady=5)

    # Create input fields for values_microsec3
    values_microsec3_label = tk.Label(graph_maker_window, text="Values 3 (comma-separated):")
    values_microsec3_label.grid(row=2, column=0, padx=5, pady=5)
    values_microsec3_entry = ttk.Entry(graph_maker_window)
    values_microsec3_entry.grid(row=2, column=1, padx=5, pady=5)

    # Create input field for categories
    categories_label = tk.Label(graph_maker_window, text="Categories (comma-separated):")
    categories_label.grid(row=3, column=0, padx=5, pady=5)
    categories_entry = ttk.Entry(graph_maker_window)
    categories_entry.grid(row=3, column=1, padx=5, pady=5)

    # Create input fields for labels
    label1_label = tk.Label(graph_maker_window, text="Label for values 1:")
    label1_label.grid(row=4, column=0, padx=5, pady=5)
    label1_entry = ttk.Entry(graph_maker_window)
    label1_entry.grid(row=4, column=1, padx=5, pady=5)

    label2_label = tk.Label(graph_maker_window, text="Label for values 2:")
    label2_label.grid(row=5, column=0, padx=5, pady=5)
    label2_entry = ttk.Entry(graph_maker_window)
    label2_entry.grid(row=5, column=1, padx=5, pady=5)

    label3_label = tk.Label(graph_maker_window, text="Label for values 3:")
    label3_label.grid(row=6, column=0, padx=5, pady=5)
    label3_entry = ttk.Entry(graph_maker_window)
    label3_entry.grid(row=6, column=1, padx=5, pady=5)

    # Create checkbox for showing values2
    show_values2_var = tk.BooleanVar()
    show_values2_check = ttk.Checkbutton(graph_maker_window, text="Show values 2", variable=show_values2_var)
    show_values2_check.grid(row=7, columnspan=2, padx=5, pady=5)

    # Create checkbox for showing values3
    show_values3_var = tk.BooleanVar()
    show_values3_check = ttk.Checkbutton(graph_maker_window, text="Show values 3", variable=show_values3_var)
    show_values3_check.grid(row=8, columnspan=2, padx=5, pady=5)

    # Create "Plot" button
    plot_button = ttk.Button(graph_maker_window, text="Plot", command=plot)
    plot_button.grid(row=9, columnspan=2, padx=5, pady=10)
    
def open_table_input():
    # Create the table input window
    table_window = tk.Tk()
    table_window.title("Table Input")

    # Create entry fields for each column
    entries = []

    for i in range(6):
        label = tk.Label(table_window, text=f"Column {i+1}:")
        label.grid(row=0, column=i, padx=5, pady=5)
        
        entry = tk.Entry(table_window)
        entry.grid(row=1, column=i, padx=5, pady=5)
        entries.append(entry)

    # Function to submit table row
    def submit_row():
        data = [extract_float_from_time(entry.get()) for entry in entries]
        
        # Calculate differences between adjacent data points
        differences = [data[i+1] - data[i] for i in range(len(data)-1)]
        average = sum(differences) / len(differences)
        # average = round((average)*1e6, 1)
        average_label.config(text="Average: {:.7f}".format(average))
        copy_button.config(state="normal")
        
    
    # Function to copy average value to clipboard
    def copy_average():
        root.clipboard_clear()
        root.clipboard_append(average_label.cget("text"))
        messagebox.showinfo("Info", "Average value copied to clipboard.")
        

    # Create "Submit" button
    submit_button = tk.Button(table_window, text="Submit", command=submit_row)
    submit_button.grid(row=2, columnspan=6, padx=5, pady=10)

    # Create label to display average
    average_label = tk.Label(table_window, text="")
    average_label.grid(row=3, columnspan=6, padx=5, pady=5)
    
    # Create button to copy average value to clipboard
    copy_button = tk.Button(table_window, text="Copy Average", command=copy_average, state="disabled")
    copy_button.grid(row=4, columnspan=6, padx=5, pady=5)
    
    table_window.mainloop()
    
def open_program():
    selected_program = program_var.get()
    if selected_program:
        if selected_program == "Time Calculator":
            program_calculator()
        elif selected_program == "Average Calculator":
            open_table_input()
        elif selected_program == "Graph Maker":
            open_graph_maker()
            
# Create the main window
root = tk.Tk()
root.title("Program Menu")

# Create program menu
programs = ["Time Calculator", "Average Calculator", "Graph Maker"]  # Add more programs as needed
program_var = tk.StringVar()
program_menu = ttk.Combobox(root, textvariable=program_var, values=programs)
program_menu.grid(row=0, column=0, padx=10, pady=10)

# Create "Open" button
open_button = tk.Button(root, text="Open", command=open_program)
open_button.grid(row=0, column=1, padx=10, pady=10)

# Start the GUI event loop
root.clipboard_clear()
root.clipboard_append("Something to the clipboard")
root.mainloop()
