import thread
import time, sys
import numpy as np
import matplotlib.pyplot as plt


def data_gen():
    t = data_gen.t
    data_gen.t = time.time()
    return 1.0/(data_gen.t - t) #np.sin(2*np.pi*t) * np.exp(-t/10.)
data_gen.t = 0

class session():
	def __init__(self, generator):
		self.fig = plt.figure()
		self.ax = self.fig.add_subplot(111)
		self.line, = self.ax.plot([], [], animated=True, lw=2)
		self.ax.set_ylim(0, 40)
		self.ax.set_xlim(0, 100)
		self.ax.grid()

		self.xdata, self.ydata = [], []
		self.ydata = [0 for i in range(100)]
		self.xdata = [i for i in range(100)]

		self.generator = generator

	def _run_nb(self, *args):
		thread.start_new(self._run, ())

	def _run(self):
		self.background = self.fig.canvas.copy_from_bbox(self.ax.bbox)

		while 1:
			# restore the clean slate background
			self.fig.canvas.restore_region(self.background)

			self.ydata.append(self.generator())
			self.ydata = self.ydata[1:]
			self.line.set_data(self.xdata, self.ydata)

			# just draw the animated artist
			self.ax.draw_artist(self.line)
			# just redraw the axes rectangle
			self.fig.canvas.blit(self.ax.bbox)

	def start(self):
		manager = plt.get_current_fig_manager()
		manager.window.after(100, self._run_nb, self)
		plt.show()

if __name__ == "__main__":
		a = session(data_gen)
		a.start()

