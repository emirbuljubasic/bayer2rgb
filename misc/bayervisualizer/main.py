import tkinter as tk
from tkinter import ttk
import numpy as np

def bayer_to_rgb888(matrix):
    """ Convert BGGR Bayer matrix to RGB888 format """
    height, width = matrix.shape
    rgb888_matrix = np.zeros((height, width, 3), dtype=np.uint8)

    for y in range(0, height, 2):
        for x in range(0, width, 2):
            B = matrix[y][x]
            G1 = matrix[y][x+1]
            G2 = matrix[y+1][x]
            R = matrix[y+1][x+1]

            rgb888_matrix[y][x] = [B, G1, R]
            rgb888_matrix[y][x+1] = [B, G1, R]
            rgb888_matrix[y+1][x] = [B, G2, R]
            rgb888_matrix[y+1][x+1] = [B, G2, R]

    return rgb888_matrix

def update_rgb_matrix():
    """ Update the RGB matrix display based on the input matrix """
    try:
        width = int(width_entry.get())
        height = int(height_entry.get())
        matrix = np.zeros((height, width), dtype=np.uint8)

        for i in range(height):
            for j in range(width):
                value = int(entries[i][j].get())
                matrix[i][j] = np.clip(value, 0, 255)

        rgb_matrix = bayer_to_rgb888(matrix)

        for i in range(height):
            for j in range(width):
                r, g, b = rgb_matrix[i][j]
                rgb888_entries[i][j].config(text=f"R:{r} G:{g} B:{b}")

    except ValueError:
        pass

# GUI setup
root = tk.Tk()
root.title("Bayer to RGB888 Converter")

frame = ttk.Frame(root, padding="10")
frame.grid(row=0, column=0, sticky=(tk.W, tk.E, tk.N, tk.S))

ttk.Label(frame, text="Width:").grid(row=0, column=0, sticky=tk.W)
width_entry = ttk.Entry(frame)
width_entry.grid(row=0, column=1)
width_entry.insert(0, "4")

ttk.Label(frame, text="Height:").grid(row=1, column=0, sticky=tk.W)
height_entry = ttk.Entry(frame)
height_entry.grid(row=1, column=1)
height_entry.insert(0, "4")

update_button = ttk.Button(frame, text="Update", command=update_rgb_matrix)
update_button.grid(row=2, column=0, columnspan=2)

entries = []
rgb888_entries = []

def create_matrix_entries():
    for widget in frame.grid_slaves():
        if widget not in [width_entry, height_entry, update_button]:
            widget.destroy()

    width = int(width_entry.get())
    height = int(height_entry.get())

    for i in range(height):
        row_entries = []
        for j in range(width):
            entry = ttk.Entry(frame, width=5)
            entry.grid(row=3+i, column=j, padx=5, pady=5)
            row_entries.append(entry)
        entries.append(row_entries)

    for i in range(height):
        row_entries = []
        for j in range(width):
            lbl = ttk.Label(frame, text="R:0 G:0 B:0", width=15)
            lbl.grid(row=3+height+i, column=j, padx=5, pady=5)
            row_entries.append(lbl)
        rgb888_entries.append(row_entries)

create_matrix_entries()

root.mainloop()

