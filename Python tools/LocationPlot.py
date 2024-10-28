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
(ax)AP: -7.94504, 1.11164
(ax)STA: 5.94544, -5.04349
(ax)AP: -0.704537, -5.35878
(ax)STA: -8.77481, 0.71612
(ax)AP: -7.34971, -5.64889
(ax)STA: 7.74876, 7.52364
(ax)AP: 5.00736, -6.29654
(ax)STA: -1.15526, 4.9765
(ax)AP: 6.46649, -5.30678
(ax)STA: 6.60796, 1.02645
(ax)AP: -9.72604, -1.68821
(ax)STA: -4.19685, -8.46543
(ax)AP: -2.64109, 3.30042
(ax)STA: 9.92716, -8.7533
(ax)AP: 2.05698, -9.1247
(ax)STA: 7.61466, 2.71012
(ax)AP: -4.84345, 1.52698
(ax)STA: -5.62134, 8.39847
(ax)AP: 9.16628, 3.25867
(ax)STA: -8.27737, -8.40723
(ax)AP: -7.70785, -1.18609
(ax)STA: -6.22343, -7.2625
(ax)AP: -5.52287, -9.82947
(ax)STA: -8.46191, -0.341812
(ax)AP: 4.20793, 0.125113
(ax)STA: -2.49045, -9.65086
(ax)AP: -6.4594, -0.177962
(ax)STA: 5.57873, -2.26071
(ax)AP: 2.32441, -1.1076
(ax)STA: 0.206978, -3.23598
(ax)AP: -1.4984, 9.89675
(ax)STA: -4.89371, -9.14261
(ax)AP: -6.91114, -8.30342
(ax)STA: 7.09724, 4.86161
(ax)AP: 3.57108, -2.00149
(ax)STA: -7.24675, 8.42995
(ax)AP: 3.73872, 7.78641
(ax)STA: -3.37501, -9.13909
(ax)AP: 1.98513, -8.52804
(ax)STA: -4.34369, -3.50428
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

