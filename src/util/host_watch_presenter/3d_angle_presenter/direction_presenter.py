import thread
import time, sys
import direction

def data_gen():
    t = data_gen.t
    data_gen.t = time.time()
    return (1.0/(data_gen.t - t), 1.0/(data_gen.t - t), 1.0/(data_gen.t - t)) #np.sin(2*np.pi*t) * np.exp(-t/10.)
data_gen.t = 0

class session():
	def __init__(self, generator):
		self.generator = generator
		self.direction = direction.DirectionPresenter()

	def run(self, *args):
		thread.start_new(self.direction.run, ())

		while 1:
			print "In run!"
			data = self.generator()
			angle = direction.angle3d_t()
			angle.x = data[0]
			angle.y = data[1]
			angle.z = data[2]
			self.direction.set_angle(angle)


if __name__ == "__main__":
		a = session(data_gen)
		a.run()

