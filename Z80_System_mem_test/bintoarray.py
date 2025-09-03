
with open('a.bin', 'rb') as f:
    count = 0
    while (b := f.read(1)):
        #print(type(b))
        print('0x{:02x}, '.format(int.from_bytes(b, "big")), end='')
        count += 1
        if count % 16 == 0 :
            print()
print()
