import matplotlib.pyplot as plt

# Data parsing function
def parse_data(file_content):
    bf_ap = []
    bf_sta = []
    ax_ap = []
    ax_sta = []

    for line in file_content.strip().split('\n'):
        if line.startswith('(bf)AP'):
            coords = line.split(': ')[1].split(', ')
            bf_ap.append((float(coords[0]), float(coords[1])))
        elif line.startswith('(bf)STA'):
            coords = line.split(': ')[1].split(', ')
            bf_sta.append((float(coords[0]), float(coords[1])))
        elif line.startswith('(ax)AP'):
            coords = line.split(': ')[1].split(', ')
            ax_ap.append((float(coords[0]), float(coords[1])))
        elif line.startswith('(ax)STA'):
            coords = line.split(': ')[1].split(', ')
            ax_sta.append((float(coords[0]), float(coords[1])))

    return bf_ap, bf_sta, ax_ap, ax_sta

# Provided file content
file_content = """
(bf)AP: -2.30333, -3.05295
(bf)STA: 6.4936, 9.04886
(ax)AP: 3.87612, -6.7148
(ax)STA: 0.957651, 0.088982
(ax)AP: 6.13646, 4.17481
(ax)STA: 5.11816, 2.96141
(ax)AP: -9.05047, 8.15574
(ax)STA: 5.57306, 9.91426
(ax)AP: 3.1291, -7.45772
(ax)STA: 6.26776, 1.50375
(ax)AP: -3.53336, 5.19074
(ax)STA: 7.96869, 0.132395
(ax)AP: 1.95872, -8.78351
(ax)STA: 3.38657, 0.0190151
(ax)AP: 5.15071, 7.39871
(ax)STA: 1.59282, 0.91514
(ax)AP: 6.0624, -5.81592
(ax)STA: 4.97737, 0.10896
(ax)AP: -5.54582, -6.59833
(ax)STA: 9.58879, 8.99724
"""

# Parse the data
bf_ap, bf_sta, ax_ap, ax_sta = parse_data(file_content)

# Plotting
plt.figure(figsize=(10, 8))

# Plot bf AP and STA
if bf_ap:
    bf_ap_x, bf_ap_y = zip(*bf_ap)
    plt.scatter(bf_ap_x, bf_ap_y, color='blue', marker='o', label='bf AP')
if bf_sta:
    bf_sta_x, bf_sta_y = zip(*bf_sta)
    plt.scatter(bf_sta_x, bf_sta_y, color='blue', marker='x', label='bf STA')

# Plot ax AP and STA
if ax_ap:
    ax_ap_x, ax_ap_y = zip(*ax_ap)
    plt.scatter(ax_ap_x, ax_ap_y, color='red', marker='o', label='ax AP')
if ax_sta:
    ax_sta_x, ax_sta_y = zip(*ax_sta)
    plt.scatter(ax_sta_x, ax_sta_y, color='red', marker='x', label='ax STA')

plt.xlabel('X Coordinate')
plt.ylabel('Y Coordinate')
plt.title('AP and STA Locations')
plt.legend()
plt.grid(True)
plt.show()

