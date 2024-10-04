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
(bf)AP: 6, 0
(bf)STA: 8, 0
(bf)STA: 7.53209, 1.28558
(bf)STA: 6.3473, 1.96962
(bf)STA: 5, 1.73205
(bf)STA: 4.12061, 0.68404
(bf)STA: 4.12061, -0.68404
(bf)STA: 5, -1.73205
(bf)STA: 6.3473, -1.96962
(bf)STA: 7.53209, -1.28558
(bf)AP: 3.74094, 4.69099
(bf)STA: 5.74094, 4.69099
(bf)STA: 5.27303, 5.97656
(bf)STA: 4.08824, 6.6606
(bf)STA: 2.74094, 6.42304
(bf)STA: 1.86155, 5.37503
(bf)STA: 1.86155, 4.00695
(bf)STA: 2.74094, 2.95894
(bf)STA: 4.08824, 2.72137
(bf)STA: 5.27303, 3.40541
(bf)AP: -1.33513, 5.84957
(bf)STA: 0.664874, 5.84957
(bf)STA: 0.196963, 7.13514
(bf)STA: -0.987829, 7.81918
(bf)STA: -2.33513, 7.58162
(bf)STA: -3.21451, 6.53361
(bf)STA: -3.21451, 5.16553
(bf)STA: -2.33513, 4.11752
(bf)STA: -0.987829, 3.87995
(bf)STA: 0.196963, 4.56399
(bf)AP: -5.40581, 2.6033
(bf)STA: -3.40581, 2.6033
(bf)STA: -3.87372, 3.88888
(bf)STA: -5.05852, 4.57292
(bf)STA: -6.40581, 4.33535
(bf)STA: -7.2852, 3.28734
(bf)STA: -7.2852, 1.91926
(bf)STA: -6.40581, 0.871252
(bf)STA: -5.05852, 0.633687
(bf)STA: -3.87372, 1.31773
(ax)AP: -5.40581, -2.6033
(ax)STA: -3.40581, -2.6033
(ax)AP: -1.33513, -5.84957
(ax)STA: 0.664874, -5.84957
(ax)AP: 3.74094, -4.69099
(ax)STA: 5.74094, -4.69099
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

