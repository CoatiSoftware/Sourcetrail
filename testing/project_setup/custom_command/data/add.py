import sys
# import time
# time.sleep(1)

with open("out.txt", "a") as myfile:
    myfile.write(sys.argv[1] + ' ' + sys.argv[2] + ' ' + sys.argv[3] + ' ' + sys.argv[4] + '\n')
