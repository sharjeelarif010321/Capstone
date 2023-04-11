txDataSTM = "119 111 0000010001000100010000000000"
if txDataSTM[0] == " ":
    txDataSTM = txDataSTM[8:36]
    print(txDataSTM)

    arrDataSTM = txDataSTM
    position = arrDataSTM
    for i in range(0, len(position), 4):
        print(position[i:i+4])

else:
    arrDataSTM = txDataSTM.split()

    heart_rate = arrDataSTM[1]
    spo2 = arrDataSTM[0]
    position = arrDataSTM[2]

    print(f"Heart rate: {heart_rate}")
    print(f"SPO2: {spo2}")

    for i in range(0, len(position), 4):
        print(position[i:i+4])  
