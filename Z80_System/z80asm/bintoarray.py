import sys
file_name = sys.argv[1]
with open(file_name, 'rb') as f:
    count = 0
    while (b := f.read(1)):
        #print(type(b))
        print('0x{:02x}, '.format(int.from_bytes(b, "big")), end='')
        count += 1
        if count % 16 == 0 :
            print()
print()
