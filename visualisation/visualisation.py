import numpy as np
import sys
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

def read_points_and_titles(filepath):
    with open(filepath, 'r') as file:
        lines = file.readlines()[1:]  
    points = []
    titles = []
    temp_points = []
    for line in lines:
        if ',' in line:  
            x, y = map(float, line.strip().split(','))
            temp_points.append((x, y))
        else:  
            if temp_points:
                points.append(temp_points)
                temp_points = []
            titles.append(line.strip())
    if temp_points:  
        points.append(temp_points)
    return points, titles

def init():
    scatter.set_data([], [])
    title_text.set_text('')
    return scatter, title_text

def animate(i):
    batch = points_batches[i % len(points_batches)]
    title = batch_titles[i % len(batch_titles)]
    x, y = zip(*batch) if batch else ([], [])
    scatter.set_data(x, y)
    title_text.set_text(f'epoch: {i}, {title}')
    return scatter, title_text

if __name__ == "__main__":
    
    if len(sys.argv) != 2:
        print("Usage: python visualisation.py <task1/task2> ")
        print("Example: python visualisation.py task1")
        sys.exit(1)

    task = sys.argv[1]
    
    if task == 'task1':
        x = np.linspace(-40, 40, 800)
        y = np.linspace(-40, 40, 800)
        X, Y = np.meshgrid(x, y)
        Z = (1/40) * (X**2 + Y**2) + 1 - np.cos(X) * np.cos(Y/2)
        file_path = 'visualisation.csv'
        name = 'task1.mp4'
        fig_levels = 50
    else:
        x = np.linspace(-10, 10, 400)
        y = np.linspace(-10, 10, 400)
        X, Y = np.meshgrid(x, y)
        Z = 100*(Y - X**2)**2 + (1-X)**2
        file_path = 'visualisation2.csv'
        name = 'task2.mp4'
        fig_levels = 10

    points_batches, batch_titles = read_points_and_titles(file_path)
    
    fig, ax = plt.subplots()
    contourf = ax.contourf(X, Y, Z, cmap='plasma', levels=fig_levels)
    ax.set_xlabel('X1')
    ax.set_ylabel('X2')
    scatter, = ax.plot([], [], 'wo', markersize=2, label='Points')  
    title_text = ax.set_title('', fontsize=15)

    fig.colorbar(contourf)
    ani = FuncAnimation(fig, animate, init_func=init, frames=len(points_batches), interval=1000, blit=True)
    ani.save(name, writer='ffmpeg', fps=30, dpi=300, extra_args=['-vcodec', 'libx264', '-pix_fmt', 'yuv420p', '-crf', '18'])

    plt.show()
