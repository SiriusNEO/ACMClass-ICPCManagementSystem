import os

com = 'time ./ICPC < data/test.in >> test.out'
os.system(com)
print("Diffing Output")
com = 'diff data/test.out test.out'
os.system(com)

com = 'time ./ICPC < data/small.in >> small.out'
os.system(com)
print("Diffing Output")
com = 'diff data/small.out small.out'
os.system(com)

com = 'time ./ICPC < data/big.in >> big.out'
os.system(com)
print("Diffing Output")
com = 'diff data/big.out big.out'
os.system(com)

com = 'time ./ICPC < data/bigger.in >> bigger.out'
os.system(com)
print("Diffing Output")
com = 'diff data/bigger.out bigger.out'
os.system(com)

com = 'time ./ICPC < data/error.in >> error.out'
os.system(com)
print("Diffing Output")
com = 'diff data/error.out error.out'
os.system(com)

for i in range(3, 8):
    com = 'time ./ICPC < data/' + str(i) + '.in >> '+str(i)+'.out'
    os.system(com)
    print("Diffing Output")
    com = 'diff ' + str(i)+'.out '+'data/'+str(i)+'.out' 
    os.system(com)
