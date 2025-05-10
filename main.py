from tkinter import *
import time
from PIL import Image, ImageTk

window = Tk()
window.title("N-BODY SIMULATOR")

# configuration for the main window
window_width = 1000
window_height = 1000
window.config(width=window_width, height=window_height)

# window centering
screen_width = window.winfo_screenwidth()
screen_height = window.winfo_screenheight()
x = int((screen_width / 2) - (window_width / 2))
y = int((screen_height / 2) - (window_height / 2))
window.geometry(f"{window_width}x{window_height}+{x}+{y}")

main_canvas = Canvas(window, width=window_width, height=window_height, bg="white", highlightthickness=0)
main_canvas.pack(fill=BOTH, expand=YES)

# origin point
ABS_ZERO_X = window_width // 2
ABS_ZERO_Y = window_height // 2

# Colors for trajectories (you can add more if needed)
TRAJECTORY_COLORS = ["red", "green", "blue", "yellow", "cyan", "magenta"]

# moving animation for the bodies
def move_image_smoothly(image_id, x_start, y_start, x_end, y_end, duration=0.00001):
    steps = 20
    for step in range(steps + 1):
        progress = step / steps
        current_x = x_start + (x_end - x_start) * progress
        current_y = y_start + (y_end - y_start) * progress
        main_canvas.coords(image_id, current_x, current_y)
        window.update()
        time.sleep(duration / steps)

# function made to get the information from the files filled by the C source code
def get_file_info():
    xbody = []
    ybody = []
    
    with open("fileInfo.txt", "r") as file:
        file_number = int(file.readline().strip()) # get the number of bodies; 
        file_names = [file.readline().strip() for _ in range(file_number)] # each file contains (x,y) peers 
    
    # reading data from each file and store x and y into two separate lists
    for file_name in file_names:
        body_x = []
        body_y = []
        with open(file_name, "r") as f:
            for line in f:
                x, y = map(float, line.strip().split()) # data was read as string -> conversion to float individuals
                body_x.append(x)
                body_y.append(y)
        xbody.append(body_x)
        ybody.append(body_y)
    
    return file_number, xbody, ybody

file_number, xbody, ybody = get_file_info()

DOT = "dot.png"
    
original_image = Image.open(DOT)

#image redimensioning 
width = int(original_image.width * 0.1)
height = int(original_image.height * 0.1)
resized_image = original_image.resize((width, height))
tk_image = ImageTk.PhotoImage(resized_image)

# list of objects displayed on the canvas
image_objects = []
trajectory_lines = []  # To store trajectory line IDs

# objects' initialization
for i in range(file_number):
    if tk_image:
        img_obj = main_canvas.create_image(ABS_ZERO_X + xbody[i][0], ABS_ZERO_Y + ybody[i][0], image=tk_image)
        image_objects.append(img_obj)
        # create an empty trajectory line for each body
        traj_line = main_canvas.create_line(ABS_ZERO_X + xbody[i][0], ABS_ZERO_Y + ybody[i][0],
                                           ABS_ZERO_X + xbody[i][0], ABS_ZERO_Y + ybody[i][0],
                                           fill=TRAJECTORY_COLORS[i % len(TRAJECTORY_COLORS)], width=1)
        trajectory_lines.append(traj_line)

def animate():
    if xbody and ybody and len(xbody[0]) > 1:
        steps = len(xbody[0]) # define the number of steps based on the data stored
        
        for step in range(steps - 1):
            # at each step, we are updating each body's position
            for i in range(file_number):
                current_x = ABS_ZERO_X + xbody[i][step]
                current_y = ABS_ZERO_Y + ybody[i][step]
                
                next_x = ABS_ZERO_X + xbody[i][step + 1]
                next_y = ABS_ZERO_Y + ybody[i][step + 1]
                
                # Get current coordinates of the trajectory line
                current_line_coords = main_canvas.coords(trajectory_lines[i])
                
                # Update the trajectory line to include the new point
                main_canvas.coords(trajectory_lines[i], *current_line_coords, next_x, next_y)
                
                # apply changes
                move_image_smoothly(image_objects[i], current_x, current_y, next_x, next_y, duration=0.0001)

start_button = Button(window, text="Start Simulare", command=animate)
start_button.place(x=10, y=10)

# saving image to avoid it being collected by the garbage collector
window.image_reference = tk_image

window.mainloop()