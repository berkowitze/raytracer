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
    result = subprocess.run(["python", "multiprocess.py", filename], capture_output=True, text=True)
    return result.stdout.split('\n')[-2].split(' ')[-1]


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
