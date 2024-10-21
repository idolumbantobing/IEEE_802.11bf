import tkinter as tk
from tkinter import messagebox

class DraggableNode:
    def __init__(self, canvas, x, y, label, node_type, app):
        self.canvas = canvas
        self.node_type = node_type
        self.app = app
        color = "blue" if node_type == "AP" else "red"
        self.id = canvas.create_oval(x-5, y-5, x+5, y+5, fill=color, tags="node")
        self.text_id = canvas.create_text(x, y-10, text=label)
        self.label = label
        self._drag_data = {"x": 0, "y": 0}
        
        canvas.tag_bind(self.id, "<ButtonPress-1>", self.on_start)
        canvas.tag_bind(self.id, "<B1-Motion>", self.on_drag)
        canvas.tag_bind(self.id, "<ButtonRelease-1>", self.on_drop)

    def on_start(self, event):
        self._drag_data["x"] = event.x
        self._drag_data["y"] = event.y

    def on_drag(self, event):
        dx = event.x - self._drag_data["x"]
        dy = event.y - self._drag_data["y"]
        self.canvas.move(self.id, dx, dy)
        self.canvas.move(self.text_id, dx, dy)
        self._drag_data["x"] = event.x
        self._drag_data["y"] = event.y
        self.app.update_connections()

    def on_drop(self, event):
        pass

    def get_position(self):
        x1, y1, x2, y2 = self.canvas.coords(self.id)
        x = (x1 + x2) / 2
        y = (y1 + y2) / 2
        return round(x), round(y)

class CoordinateApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Coordinate System")

        self.canvas_width = 800
        self.canvas_height = 600
        self.center_x = self.canvas_width // 2
        self.center_y = self.canvas_height // 2
        
        self.canvas = tk.Canvas(root, width=self.canvas_width, height=self.canvas_height, bg="white")
        self.canvas.grid(row=0, column=0, columnspan=4)
        
        self.draw_axes()

        self.nodes = []
        self.node_counter = 0
        self.selected_node = None
        self.connections = []

        self.add_ap_button = tk.Button(root, text="Add Access Point", command=self.add_ap)
        self.add_ap_button.grid(row=1, column=0)

        self.add_sta_button = tk.Button(root, text="Add Station", command=self.add_sta)
        self.add_sta_button.grid(row=1, column=1)

        self.calculate_button = tk.Button(root, text="Calculate Positions", command=self.calculate_positions)
        self.calculate_button.grid(row=1, column=2)

        self.quit_button = tk.Button(root, text="Quit", command=root.quit)
        self.quit_button.grid(row=1, column=3)

    def draw_axes(self):
        # Draw x-axis
        self.canvas.create_line(0, self.center_y, self.canvas_width, self.center_y, fill="black")
        # Draw y-axis
        self.canvas.create_line(self.center_x, 0, self.center_x, self.canvas_height, fill="black")
        # Draw labels for axes
        self.canvas.create_text(self.center_x + 10, self.center_y - 10, text="(0, 0)", anchor="nw")

    def add_ap(self):
        self.add_node("AP")

    def add_sta(self):
        self.add_node("STA")

    def add_node(self, node_type):
        label = f"{node_type}{self.node_counter}"
        new_node = DraggableNode(self.canvas, self.center_x, self.center_y, label, node_type, self)
        self.nodes.append(new_node)
        self.node_counter += 1
        self.canvas.tag_bind(new_node.id, "<ButtonPress-3>", lambda event, node=new_node: self.select_node(event, node))

    def select_node(self, event, node):
        if self.selected_node is None:
            self.selected_node = node
        else:
            if self.selected_node.node_type != node.node_type:
                self.create_connection(self.selected_node, node)
                self.selected_node = None
            else:
                self.selected_node = node

    def create_connection(self, node1, node2):
        x1, y1 = node1.get_position()
        x2, y2 = node2.get_position()
        line = self.canvas.create_line(x1, y1, x2, y2, dash=(4, 2))
        self.connections.append((node1, node2, line))

    def update_connections(self):
        for node1, node2, line in self.connections:
            x1, y1 = node1.get_position()
            x2, y2 = node2.get_position()
            self.canvas.coords(line, x1, y1, x2, y2)

    def calculate_positions(self):
        positions = {
            node.label: (
                (node.get_position()[0] - self.center_x) / 100,
                (self.center_y - node.get_position()[1]) / 100
            )
            for node in self.nodes
        }
        with open("position.txt", "w") as file:
            file.write("std::vector<std::pair<std::string, std::pair<double, double>>> positions = {\n")
            for label, pos in positions.items():
                file.write(f'    {{"{label}", {{{pos[0]}, {pos[1]}}}}},\n')
            file.write("};\n")

if __name__ == "__main__":
    root = tk.Tk()
    app = CoordinateApp(root)
    root.mainloop()

