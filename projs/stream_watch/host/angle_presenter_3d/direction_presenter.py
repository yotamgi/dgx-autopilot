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

	def start(self, *args):
		self.direction.run()
                self.direction.show_angle(True)

		while 1:
                        try:
        			data = self.generator()
        		except IOError as e:
                                print e
                                print "Closing"
                                self.direction.stop()
                                return
			angle = direction.angle3d_t()
			angle.ax = data[0]
			angle.ay = data[1]
			angle.az = data[2]
			self.direction.set_angle(angle)
			time.sleep(0.01)


if __name__ == "__main__":
		a = session(data_gen)
		a.start()

