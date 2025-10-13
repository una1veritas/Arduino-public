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
            if count % 16 == 0 :
                print(':', end='')
            print(f'{int.from_bytes(b, "big"):02x} ', end='')
            count += 1
            if count % 16 == 0 :
                print() 
            if count % 256 == 0 :
                print()
print()
