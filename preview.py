import time
import os
import matplotlib
import matplotlib.pyplot as plt
import matplotlib.image as mpimg
import subprocess
import sys


filename = sys.argv[1]
print("ctrl-c to quit at any time")
# set recursion limit much higher
sys.setrecursionlimit(10**6)
done = False


# taken from https://stackoverflow.com/a/45734500
def mypause(interval):
    backend = plt.rcParams["backend"]
    if backend in matplotlib.backends.backend_registry.list_builtin(
        matplotlib.backends.BackendFilter.INTERACTIVE
    ):
        figManager = matplotlib._pylab_helpers.Gcf.get_active()
        if figManager is not None:
            canvas = figManager.canvas
            if canvas.figure.stale:
                canvas.draw()
            canvas.start_event_loop(interval)
            return


def _update():
    global done
    result = subprocess.run(
        ["python", "multiprocess.py", filename], capture_output=True, text=True
    )
    output = result.stdout.split("\n")

    try:
        fn = output[-3].split(": ")[1]
    except IndexError:
        done = True
        time.sleep(0.5)
        # print("Done")
        return f"out/{filename}.png"

    try:
        progress = float(output[-2].split(": ")[1].strip("%"))
        print(f"Progress: {progress:.2f}%", end="\r")
        fig.canvas.manager.set_window_title(f"Preview of {filename} - {progress:.2f}%")
    except (IndexError, ValueError):
        pass

    return fn


def update_image(ax, fig):
    xlim = ax.get_xlim()
    ylim = ax.get_ylim()

    # Update the image
    preview_filename = _update()
    img = mpimg.imread(preview_filename)
    ax.clear()
    ax.imshow(img)

    # Restore the previous zoom settings
    ax.set_xlim(xlim)
    ax.set_ylim(ylim)
    if done:
        return
    try:
        mypause(3)
    except KeyboardInterrupt:
        print("\nQuitting...")
        sys.exit(1)

    os.remove(preview_filename)
    update_image(ax, fig)  # Recursive update


plt.ion()

# Initial plot setup
fig, ax = plt.subplots()
fig.canvas.manager.set_window_title(f"Preview of {filename}")

preview_filename = _update()
img = mpimg.imread(preview_filename)
ax.imshow(img)
plt.axis("off")  # Turn off axes

# Start the recursive image update
if not done:
    update_image(ax, fig)

fig.canvas.manager.set_window_title(f"Preview of {filename} - 100%")
plt.ioff()
# print(f"Final file in out/{filename}.png")
# Display the plot
try:
    plt.show()
except KeyboardInterrupt:
    print("\nQuitting...")
    sys.exit(1)
