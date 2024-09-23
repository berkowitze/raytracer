import time
import os
import matplotlib
import matplotlib.pyplot as plt
import matplotlib.image as mpimg
import subprocess
import sys

filename = sys.argv[1]
# set recursion limit much higher
sys.setrecursionlimit(10**6)

# taken from https://stackoverflow.com/a/45734500
def mypause(interval):
    backend = plt.rcParams['backend']
    if backend in matplotlib.rcsetup.interactive_bk:
        figManager = matplotlib._pylab_helpers.Gcf.get_active()
        if figManager is not None:
            canvas = figManager.canvas
            if canvas.figure.stale:
                canvas.draw()
            canvas.start_event_loop(interval)
            return



def _update():
    global done
    result = subprocess.run(["python", "multiprocess.py", filename], capture_output=True, text=True)
    # last line output: print(f"Preview={preview_filename}, Progress={100 * (1 - incomplete_pixels / (image_width * image_height)):.2f}%")

    try:
        fn = result.stdout.split('\n')[-2].split('=')[1].split(',')[0]
    except IndexError:
        done = True
        time.sleep(0.5)
        print("Done")
        return f"out/{filename}.png"

    try:
        progress = float(result.stdout.split('\n')[-2].split('=')[2].split('%')[0])
        print(f"Progress: {progress:.2f}%", end='\r')
    except (IndexError, ValueError):
        pass


    return fn


def update_image(ax, fig):
    preview_filename = _update()
    img = mpimg.imread(preview_filename)
    ax.clear()
    ax.imshow(img)
    fig.canvas.draw()
    mypause(3)
    os.remove(preview_filename)
    update_image(ax, fig)  # Recursive update

plt.ion()

# Initial plot setup
fig, ax = plt.subplots()
fig.canvas.manager.set_window_title(f"Preview of {filename}")

preview_filename = _update()
img = mpimg.imread(preview_filename)
ax.imshow(img)
plt.axis('off')  # Turn off axes

# Start the recursive image update
update_image(ax, fig)

# Display the plot
plt.show()
