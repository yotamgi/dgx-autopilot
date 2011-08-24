import thread
import time
import time, sys
import direction

def data_gen():
	data_gen.t += 0.4
	return (data_gen.t, data_gen.t, data_gen.t)
data_gen.t = 0.

class session():
	def __init__(self, generator):
		self.generator = generator
		self.direction = direction.DirectionPresenter()

	def run(self, *args):
		self.direction.run()

		while 1:
			data = self.generator()
			angle = direction.angle3d_t()
			angle.ax = data[0]
			angle.ay = data[1]
			angle.az = data[2]
			self.direction.set_angle(angle)
			time.sleep(0.01)
			print angle.ax, angle.ay, angle.az


if __name__ == "__main__":
		a = session(data_gen)
		a.run()

