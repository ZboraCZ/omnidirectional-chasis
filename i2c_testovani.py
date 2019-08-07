from smbus2 import SMBusWrapper
import time

address = 0x04

def sendBlockDataProcess():
    with SMBusWrapper(1) as bus:
        data = [255,255,255, 255]
        bus.write_i2c_block_data(address, 0, data)
        
        #readData = bus.read_byte_data(address,0)
        #print(readData)

startTime = time.time()

for x in range(500):
    print(x)
    sendBlockDataProcess()
    time.sleep(0.3)
    
finishTime = time.time()

print('Start time: ' + str(startTime))
print('Finish time: ' + str(finishTime))
print('Program process time: ' + str(finishTime - startTime))