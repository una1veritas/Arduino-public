import sys
file_name = sys.argv[1]
if file_name.split('.')[-1] != 'rom' :
    file_name += '.rom'

try:
    f = open(file_name, 'rb') 
except FileNotFoundError:
    print('Error: cannot open file '.file_name)
else:
    with f:
        count = 0
        while (b := f.read(1)):
            if (count & 0x0f == 0):
                print(': ',end='')
            print('{:02x} '.format(int.from_bytes(b, "big")), end='')
            count += 1
            if count & 0x0f == 0 :
                print() 
            if count & 0xff == 0 :
                print()
print()
