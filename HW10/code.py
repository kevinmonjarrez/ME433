from ulab import numpy as np
import time

sin_1 = np.linspace(-np.pi, np.pi, 1024)
sin_1 = np.sin(sin_1)

sin_2 = np.linspace(-np.pi, np.pi, 512)
sin_2 = np.concatenate((sin_2, sin_2))
sin_2 = np.sin(sin_2)

sin_3 = np.linspace(-np.pi, np.pi, 256)
sin_3 = np.concatenate((sin_3, sin_3,sin_3,sin_3))
sin_3 = np.sin(sin_3)

FTT = (sin_1+sin_2+sin_3)

FTT = np.fft.fft(FTT)

while 1:
    for i in FTT:
        for x in i:
            print((10000*x,))
            time.sleep(.1)









