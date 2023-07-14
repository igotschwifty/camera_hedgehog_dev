
import seaborn as sns
import matplotlib.pyplot as plt
import numpy as np
import struct
import os

import csv

fileName = "output.bin"

# Specify the data types and their corresponding field names
dt = np.dtype([("timeTotalWithWait", np.float32),
               ("timeTotalNoWait", np.float32),
               ("timeWaiting", np.float32),
               ("timeProcess", np.float32),
               ("timeGrab", np.float32),
               # ("timeStamps", np.float64),
               # ("droppedFrames", np.float64),
               # ("framesPopped", np.float64)])
              ])

# Load the binary file with the specified dtype
data = np.fromfile(fileName, dtype=dt)

# Access the loaded data
timeWaiting = data["timeWaiting"]
timeToGrab = data["timeGrab"]
timeToProcess = data["timeProcess"]
totalTimeNoWait = data["timeTotalNoWait"]
timeTotalWithWait = data["timeTotalWithWait"]
# timeStamps = data["timeStamps"]
# droppedFrames = data["droppedFrames"]
# framesPopped = data["framesPopped"]
# plt.hist(timeToGrab, 200)
# plt.xlabel("Time to grab image (us)")
# plt.ylabel("Number of occurences")
# plt.title("Time to Grab Images")
# plt.show()
# plt.hist(timeToProcess, 200)
# plt.xlabel("Time to process image (us)")
# plt.ylabel("Number of occurences")
# plt.title("Time to Process Images")
# plt.show()
# plt.hist(totalTimeNoWait, 200)
# plt.xlabel("Total time (us)")
# plt.ylabel("Number of occurences")
# plt.title("Time to Grab and Process Image")
# plt.show()
print("Maximum time for frame: ", np.max(totalTimeNoWait))
print("Number of frames above 130us: ", np.count_nonzero(np.where(np.array(totalTimeNoWait) > 130.0)))
for i in np.where(np.array(totalTimeNoWait) > 130.0)[0]:
    print('{}: {}'.format(str(i), totalTimeNoWait[int(i)]))
