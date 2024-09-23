import multiprocessing
import time
import os
import subprocess

num_chunks = 30

def call_chunk_command(filename: str, chunk: int):
    print(f"processing chunk {chunk}...")
    with open(f"out/{filename}/{chunk}.txt", "w") as f:
        subprocess.call(["./raytracer", f"--chunk={chunk}"], stdout=f)
    
    print(f"chunk {chunk} done")
    

def call_raytracer_command():
    filename = str(time.time())
    os.mkdir(f"out/{filename}")
    print(f"Output will be saved in out/{filename}.png")
    print(f"Preview command: python preview.py {filename}")

    with open(f"out/{filename}.ppm", "w") as f:
        subprocess.call(["./raytracer", "--chunk=-2"], stdout=f)

    pool = multiprocessing.Pool(multiprocessing.cpu_count())

    t_start = time.time()
    for i in range(num_chunks):
        pool.apply_async(call_chunk_command, (filename, i))

    pool.close()
    pool.join()
    print("All chunks done, combining files")

    with open(f'out/{filename}.ppm', 'a') as f:
        for chunk in range(num_chunks):
            with open(f"out/{filename}/{chunk}.txt", "r") as chunk_file:
                f.write(chunk_file.read()) 
    t_3 = time.time()

    # convert to png
    exit_code = subprocess.call(["magick", f"out/{filename}.ppm", f"out/{filename}.png"])

    print(f"Done. Final output in out/{filename}.png")
    print(f"Time taken: {t_3 - t_start:.2f}s")
    
    # cleanup
    if exit_code == 0:
        for i in range(num_chunks):
            subprocess.call(["rm", f"out/{filename}/{i}.txt"])

        subprocess.call(["rmdir", f"out/{filename}"])
        subprocess.call(["rm", f"out/{filename}.ppm"])



def preview(filename):
    base = f"out/{filename}.ppm"
    with open(base) as f:
        ppm = f.read()
    
    resolution = ppm.split('\n')[1].split(' ')
    image_width = int(resolution[0])
    image_height = int(resolution[1])
    rows_per_chunk = (image_height + num_chunks - 1) // num_chunks
    preview_filename = f'out/preview/{filename}{time.time()}.ppm'
    incomplete_pixels = 0
    with open(preview_filename, 'a') as f:
        f.truncate(0)
        f.write(ppm)
        for i in range(num_chunks):
            chunk_start = i * rows_per_chunk
            # chunk_end = image_height if i == num_chunks - 1 else min(chunk_start + rows_per_chunk, image_height)
            chunk_end = min(chunk_start + rows_per_chunk, image_height)
            expected_num_pixels = image_width * (chunk_end - chunk_start)
            chunk_filename = f"out/{filename}/{i}.txt"
            if not os.path.exists(chunk_filename):
                chunk_data = ["0 0 0"] * expected_num_pixels
                incomplete_pixels += expected_num_pixels
            else:
                with open(chunk_filename, "r") as chunk_file:
                    chunk_data = chunk_file.read().strip().split('\n')
            
            for i, line in enumerate(chunk_data):
                if len(line.split(' ')) != 3:
                    chunk_data[i] = "0 0 0"
            
            actual_num_pixels = len(chunk_data)
            # pad chunk_data with black pixels
            incomplete_pixels += expected_num_pixels - actual_num_pixels
            for _ in range(expected_num_pixels - actual_num_pixels):
                chunk_data.append("0 0 0")
            
            f.write('\n'.join(chunk_data) + '\n')

    print(f"Preview={preview_filename}, Progress={100 * (1 - incomplete_pixels / (image_width * image_height)):.2f}%")

if __name__ == '__main__':
    import sys
    if len(sys.argv) > 1:
        preview(sys.argv[1])
    else:
        call_raytracer_command()
