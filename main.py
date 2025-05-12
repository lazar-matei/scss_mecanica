from tkinter import *
import time
from PIL import Image, ImageTk

window = Tk()
window.title("N-BODY SIMULATOR")

# configuration for the main window
window_width = 1920
window_height = 1080
window.config(width=window_width, height=window_height)

screen_width = window.winfo_screenwidth()
screen_height = window.winfo_screenheight()

# origin point
ABS_ZERO_X = screen_width // 2
ABS_ZERO_Y = screen_height // 2

# colors for trajectories
TRAJECTORY_COLORS = ["red", "green", "blue", "yellow", "cyan", "magenta"]

# global variables for state management
image_objects = []   # list of objects displayed on the canvas
trajectory_lines = []   # stores trajectory line IDs
coord_labels = []  # stores labels with (x,y) coordinates
file_number = 0   # how many bodies we have
xbody, ybody = [], []  # coordinates from simulation files
screenimages = []
simulation_running = False # flag used by reset button
step_counter = 0 
isTrajectoryOff = False # flag used by trajectory toggler


main_canvas = Canvas(window, width=window_width, height=window_height)
main_canvas.pack(fill = "both", expand= True)

# function made to get the information from the files filled by the C source code
def get_file_info(mainFile):
    xbody = []
    ybody = []
    
    with open(mainFile, "r") as file:
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
        xbody.append(body_x) # array of arrays
        ybody.append(body_y)
    
    return file_number, xbody, ybody

DOT = "assets/dot.png"
original_image = Image.open(DOT)

# image redimensioning 
width = int(original_image.width * 0.1) 
height = int(original_image.height * 0.1)

resized_image = original_image.resize((width, height))
tk_image = ImageTk.PhotoImage(resized_image)

PIL_bg = Image.open("assets/bg.png")
TK_bg = ImageTk.PhotoImage(PIL_bg)

# Create background image at center
bgID = main_canvas.create_image(0, 0, anchor = "nw", image=TK_bg)
screenimages.append(TK_bg) # save the image as reference 
main_canvas.tag_lower(bgID) #place behind the bodies

# UI elements
progress_label = Label(window, text="Pas: 0", fg="black", bg="white", font=("Arial", 10))
progress_label.place(x=120, y=10)
getValuesFromFile = False

# reset simulation state and UI
def reset_simulation(mainFile):
    global image_objects, trajectory_lines, coord_labels, xbody, ybody
    global file_number, step_counter, simulation_running, isTrajectoryOff

    # delete every trace except the background
    main_canvas.delete("all")
    
    bgID = main_canvas.create_image(0, 0, anchor = "nw", image=TK_bg)
    screenimages.append(TK_bg)
    main_canvas.tag_lower(bgID)

    image_objects.clear()
    trajectory_lines.clear()
    coord_labels.clear()
    simulation_running = False
    step_counter = 0

    xbody = [] 
    ybody = []
    file_number, xbody, ybody = get_file_info(mainFile)

    # objects' initialization
    for i in range(file_number):
        if tk_image:
            img_obj = main_canvas.create_image(ABS_ZERO_X + xbody[i][0], (ABS_ZERO_Y + ybody[i][0]), image=tk_image)
            image_objects.append(img_obj)
           
            # create an empty trajectory line for each body
            traj_line = main_canvas.create_line(ABS_ZERO_X + xbody[i][0], (ABS_ZERO_Y + ybody[i][0]),
                                                ABS_ZERO_X + xbody[i][0], (ABS_ZERO_Y + ybody[i][0]),
                                                fill=TRAJECTORY_COLORS[i % len(TRAJECTORY_COLORS)], width=1)
            trajectory_lines.append(traj_line)

            # create label for coordinates
            label = main_canvas.create_text(ABS_ZERO_X + xbody[i][0], (ABS_ZERO_Y + ybody[i][0] - 15),
                                            text=f"{xbody[i][0]:.1f}, {ybody[i][0] * (-1):.1f}", fill="white", font=("Arial", 9))
            coord_labels.append(label)
    progress_label.config(text="Pas: 0")
    isTrajectoryOff = False

# moving animation for the bodies
def animate(step=0):
    global simulation_running, currentArrayPosition, currentStep
    if not simulation_running or step >= len(xbody[0]) - 1:
        simulation_running = False
        return

    for i in range(file_number):
        current_x = ABS_ZERO_X + xbody[i][step]
        current_y = (ABS_ZERO_Y + ybody[i][step])
        
        next_x = ABS_ZERO_X + xbody[i][step + 1]
        next_y = ABS_ZERO_Y + ybody[i][step + 1]
        
        currentArrayPosition = i 
        currentStep = step
        
        # updated trajectory line, added new (x,y) pair to the list of coordinates current_line_coords
        current_line_coords = main_canvas.coords(trajectory_lines[i])
        main_canvas.coords(trajectory_lines[i], *current_line_coords, next_x, next_y)
        
        # move image
        main_canvas.coords(image_objects[i], next_x, next_y)

        # update coordinate label
        main_canvas.coords(coord_labels[i], next_x, next_y - 15)
        main_canvas.itemconfig(coord_labels[i], text=f"{xbody[i][step+1]:.1f}, {ybody[i][step+1]:.1f}")

    progress_label.config(text=f"Pas: {step+1}")
    window.after(20, animate, step + 1)

# start simulation
def start_simulation():
    global simulation_running
    if not simulation_running:
        simulation_running = True
        animate(0)

def delete_trajectory():
    global trajectory_lines, isTrajectoryOff

    if isTrajectoryOff:
        # recreate all trajectory lines 
        trajectory_lines = []
        for i in range(file_number):
            current_x = ABS_ZERO_X + xbody[i][currentStep]
            current_y = ABS_ZERO_Y + ybody[i][currentStep]

            traj_line = main_canvas.create_line(current_x, current_y, current_x, current_y,
                                                fill=TRAJECTORY_COLORS[i % len(TRAJECTORY_COLORS)],
                                                width=1)
            trajectory_lines.append(traj_line)
        
        isTrajectoryOff = False
    else:
        # delete all trajectory lines
        for line_id in trajectory_lines:
            main_canvas.delete(line_id)
        isTrajectoryOff = True

def select_file(bodies, type):
    global getValuesFromFile
    global mainFile

    getValuesFromFile = True
    if bodies == 2 and type == 1:
        mainFile = "src/ref1/two_bodies_type_1.txt"
    elif bodies == 2 and type == 2:
        mainFile = "src/ref2/two_bodies_type_2.txt"
    elif bodies == 3:
        mainFile = "src/ref3/three_bodies.txt"
    elif bodies == 5:
        mainFile = "src/ref4/five_bodies.txt"
    elif bodies == 10:
        mainFile = "src/ref5/ten_bodies.txt"
    elif bodies == 20: 
        mainFile = "src/ref6/twenty_bodies.txt"

    reset_simulation(mainFile)

# buttons
start_button = Button(window, text="Start Simulare", command=start_simulation)
start_button.place(x=10, y=10)

reset_button = Button(window, text="Reset", command=lambda: reset_simulation(mainFile))
reset_button.place(x=10, y=40)

erase_trajectory = Button(window, text="ON/OFF trajectory", command=delete_trajectory)
erase_trajectory.place(x=10, y = 70)

two_bodies_type1 = Button(window, text="2 corpuri - v1", command = lambda:select_file(2,1))
two_bodies_type1.place(x = 1450, y = 10)

two_bodies_type2 = Button(window, text="2 corpuri - v2", command = lambda:select_file(2,2))
two_bodies_type2.place(x = 1450, y = 40)

three_bodies = Button(window, text="3 corpuri", command = lambda:select_file(3, None))
three_bodies.place(x = 1450, y = 70)

five_bodies = Button(window, text="5 corpuri", command = lambda:select_file(5, None))
five_bodies.place(x = 1450, y = 100)

ten_bodies = Button(window, text="10 corpuri", command = lambda:select_file(10, None))
ten_bodies.place(x = 1450, y = 130)

twenty_bodies = Button(window, text="20 corpuri", command = lambda:select_file(20, None))
twenty_bodies.place(x = 1450, y = 160)

# initialize the simulation once at start
getValuesFromFile = False
mainFile = "fileInfo.txt"

# saving image to avoid it being collected by the garbage collector
window.image_reference = tk_image

reset_simulation(mainFile)

window.mainloop()
