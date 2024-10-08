import multiprocessing
import time
import os
import subprocess

# Number of chunks for parallel processing
NUM_CHUNKS = 30


def call_chunk_command(filename: str, chunk: int):
    """Process a single chunk by invoking the raytracer command."""
    print(f"Processing chunk {chunk}...")

    # Save chunk output to a file
    output_path = f"out/{filename}/{chunk}.txt"
    with open(output_path, "w") as f:
        subprocess.call(["./raytracer", f"--chunk={chunk}"], stdout=f)

    print(f"Chunk {chunk} done")


def call_preview_command(filename: str):
    """Process a single chunk by invoking the raytracer command."""
    subprocess.Popen(["python", "preview.py", filename])


def call_raytracer_command():
    """Main function to manage raytracer execution and chunk processing."""
    filename = str(time.time())  # Use a timestamp for unique filenames
    os.makedirs(f"out/{filename}", exist_ok=True)

    print(f"Output will be saved in out/{filename}.png")
    print(f"Preview command: python preview.py {filename}")

    # Run the raytracer for the entire image, output in PPM format
    with open(f"out/{filename}.ppm", "w") as f:
        subprocess.call(["./raytracer", "--chunk=-2"], stdout=f)

    call_preview_command(filename)
    pool = multiprocessing.Pool(multiprocessing.cpu_count() - 1)

    t_start = time.time()
    for i in range(NUM_CHUNKS):
        pool.apply_async(call_chunk_command, (filename, i))

    pool.close()
    pool.join()
    print("All chunks processed, combining files...")

    # Combine all chunk outputs into the final PPM file
    with open(f"out/{filename}.ppm", "a") as f:
        for chunk in range(NUM_CHUNKS):
            chunk_path = f"out/{filename}/{chunk}.txt"
            with open(chunk_path, "r") as chunk_file:
                f.write(chunk_file.read())

    t_end = time.time()

    # Convert the PPM file to PNG using ImageMagick
    exit_code = subprocess.call(
        ["magick", f"out/{filename}.ppm", f"out/{filename}.png"]
    )

    print(f"Done. Final output is saved in out/{filename}.png")
    print(f"Time taken: {t_end - t_start:.2f} seconds")

    # Cleanup if conversion was successful
    if exit_code == 0:
        for i in range(NUM_CHUNKS):
            os.remove(f"out/{filename}/{i}.txt")
        os.rmdir(f"out/{filename}")
        os.remove(f"out/{filename}.ppm")


def preview(filename):
    """Generate a preview of the raytraced image based on available chunks."""
    base_path = f"out/{filename}.ppm"
    with open(base_path, "r") as f:
        ppm_header = f.read()

    resolution = ppm_header.split("\n")[1].split(" ")
    image_width = int(resolution[0])
    image_height = int(resolution[1])

    rows_per_chunk = image_height // NUM_CHUNKS
    preview_filename = f"out/preview/{filename}{time.time()}.ppm"
    incomplete_pixels = 0
    total_pixels = image_width * image_height

    os.makedirs("out/preview", exist_ok=True)

    # Generate the preview image by combining available chunks
    with open(preview_filename, "w") as f:
        f.write(ppm_header)  # Write the PPM header

        for i in range(NUM_CHUNKS):
            chunk_start = i * rows_per_chunk
            chunk_end = (
                image_height if i == NUM_CHUNKS - 1 else chunk_start + rows_per_chunk
            )
            expected_num_pixels = image_width * (chunk_end - chunk_start)
            chunk_filename = f"out/{filename}/{i}.txt"

            # Handle missing or incomplete chunks
            if not os.path.exists(chunk_filename):
                chunk_data = [
                    "0 0 0"
                ] * expected_num_pixels  # Black pixels for missing data
                incomplete_pixels += expected_num_pixels
            else:
                with open(chunk_filename, "r") as chunk_file:
                    chunk_data = chunk_file.read().strip().split("\n")

                actual_num_pixels = len(chunk_data)
                chunk_data = [
                    pixel if len(pixel.split(" ")) == 3 else "0 0 0"
                    for pixel in chunk_data
                ]

                # Pad with black pixels if the chunk is incomplete
                if actual_num_pixels < expected_num_pixels:
                    incomplete_pixels += expected_num_pixels - actual_num_pixels
                    chunk_data += ["0 0 0"] * (expected_num_pixels - actual_num_pixels)

            f.write("\n".join(chunk_data) + "\n")

    # Calculate and print progress based on complete chunks
    progress_ratio = (total_pixels - incomplete_pixels) / total_pixels
    print(f"Preview: {preview_filename}")
    print(f"Progress: {progress_ratio * 100:.2f}%")


if __name__ == "__main__":
    import sys

    if len(sys.argv) > 1:
        preview(sys.argv[1])
    else:
        call_raytracer_command()
