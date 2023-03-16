from pathlib import Path

import plotly.graph_objs as go

if __name__ == '__main__':

    # open the file for reading
    fp = Path('piezo1md.txt')
    with open(fp, 'r') as f:
        lines = f.readlines()

    # parse the elapsed time and value from each line
    times = []
    values = []
    for line in lines:
        parts = line.strip().split(': ')
        times.append(int(parts[0]))
        values.append(int(parts[1]))

    # create the time-series graph
    fig = go.Figure()
    fig.add_trace(go.Scatter(x=times, y=values, mode='lines', name='Value'))
    fig.update_layout(title=f'Time-Series Graph ({fp})', xaxis_title='Elapsed Time', yaxis_title='Value')
    fig.show()