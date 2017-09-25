/*
 Hi there.
mem size = 64
01 H     [0010] 0 
01 L     [0010] 0 
02 H     [0050] 0 
02 L     [0010] 0 
00 H M   [0000] 0 
00 L M R [0000] 3E (op. LD A, n)
01 H M R [0000] 3E (op. LD A, n)
01 L M R [0000] 3E (op. LD A, n)
02 H     [0000] 3E 
02 L     [0000] 3E (refresh row)
03 H     [0000] 3E (refresh row)
03 L     [0000] 3E 
04 H     [0001] 3E 
04 L   R [0001] 6 (data)
05 H   R [0001] 6 (data)
05 L   R [0001] 6 (data)
06 H   R [0001] 6 (data)
06 L     [0001] 6 
00 H M   [0002] 6 
00 L M R [0002] 32 (op. LD (nn),A)
01 H M R [0002] 32 (op. LD (nn),A)
01 L M R [0002] 32 (op. LD (nn),A)
02 H     [0001] 32 
02 L     [0001] 32 (refresh row)
03 H     [0001] 32 (refresh row)
03 L     [0001] 32 
04 H     [0003] 32 
04 L   R [0003] 18 (data)
05 H   R [0003] 18 (data)
05 L   R [0003] 18 (data)
06 H   R [0003] 18 (data)
06 L     [0003] 18 
07 H     [0004] 18 
07 L   R [0004] 0 (data)
08 H   R [0004] 0 (data)
08 L   R [0004] 0 (data)
09 H   R [0004] 0 (data)
09 L     [0004] 0 
10 H     [0008] 0 
10 L     [0008] 0 
11 H     [0008] 0 
10
32 19 0 A7 C2 5 0 76 24 0 0 0 0 0 0 0 
11 L   W [0008] 24 (write)
10
32 19 0 A7 C2 5 0 76 24 0 0 0 0 0 0 0 
12 H   W [0008] 24 (write)
12 L     [0008] 24 
00 H M   [0005] 24 
00 L M R [0005] 3A (op. LD A,(nn))
01 H M R [0005] 3A (op. LD A,(nn))
01 L M R [0005] 3A (op. LD A,(nn))
02 H     [0002] 3A 
02 L     [0002] 3A (refresh row)
03 H     [0002] 3A (refresh row)
03 L     [0002] 3A 
04 H     [0006] 3A 
04 L   R [0006] 18 (data)
05 H   R [0006] 18 (data)
05 L   R [0006] 18 (data)
06 H   R [0006] 18 (data)
06 L     [0006] 18 
07 H     [0007] 18 
07 L   R [0007] 0 (data)
08 H   R [0007] 0 (data)
08 L   R [0007] 0 (data)
09 H   R [0007] 0 (data)
09 L     [0007] 0 
10 H     [0008] 0 
10 L   R [0008] 24 (data)
11 H   R [0008] 24 (data)
11 L   R [0008] 24 (data)
12 H   R [0008] 24 (data)
12 L     [0008] 24 
00 H M   [0008] 24 
00 L M R [0008] 3C (op. INC A)
01 H M R [0008] 3C (op. INC A)
01 L M R [0008] 3C (op. INC A)
02 H     [0003] 3C 
02 L     [0003] 3C (refresh row)
03 H     [0003] 3C (refresh row)
03 L     [0003] 3C 
00 H M   [0009] 3C 
00 L M R [0009] D3 (op. OUT (n),A)
01 H M R [0009] D3 (op. OUT (n),A)
01 L M R [0009] D3 (op. OUT (n),A)
02 H     [0004] D3 
02 L     [0004] D3 (refresh row)
03 H     [0004] D3 (refresh row)
03 L     [0004] D3 
04 H     [000A] D3 
04 L   R [000A] 2 (data)
05 H   R [000A] 2 (data)
05 L   R [000A] 2 (data)
06 H   R [000A] 2 (data)
06 L     [000A] 2 
07 H     [0072] 2 
07 L     [0072] 2 
08 H   O [0072] 25 
08 L   O [0072] 25 
09 H   O [0072] 25 
09 L   O [0072] 25 
10 H   O [0072] 25 
10 L     [0072] 25 
00 H M   [000B] 25 
00 L M R [000B] 32 (op. LD (nn),A)
01 H M R [000B] 32 (op. LD (nn),A)
01 L M R [000B] 32 (op. LD (nn),A)
02 H     [0005] 32 
02 L     [0005] 32 (refresh row)
03 H     [0005] 32 (refresh row)
03 L     [0005] 32 
04 H     [000C] 32 
04 L   R [000C] 18 (data)
05 H   R [000C] 18 (data)
05 L   R [000C] 18 (data)
06 H   R [000C] 18 (data)
06 L     [000C] 18 
07 H     [000D] 18 
07 L   R [000D] 0 (data)
08 H   R [000D] 0 (data)
08 L   R [000D] 0 (data)
09 H   R [000D] 0 (data)
09 L     [000D] 0 
10 H     [0008] 0 
10 L     [0008] 0 
11 H     [0008] 0 
10
32 19 0 A7 C2 5 0 76 25 0 0 0 0 0 0 0 
11 L   W [0008] 25 (write)
10
32 19 0 A7 C2 5 0 76 25 0 0 0 0 0 0 0 
12 H   W [0008] 25 (write)
12 L     [0008] 25 
00 H M   [000E] 25 
00 L M R [000E] DB (op. )
01 H M R [000E] DB (op. )
01 L M R [000E] DB (op. )
02 H     [0006] DB 
02 L     [0006] DB (refresh row)
03 H     [0006] DB (refresh row)
03 L     [0006] DB 
04 H     [000F] DB 
04 L   R [000F] 1 (data)
05 H   R [000F] 1 (data)
05 L   R [000F] 1 (data)
06 H   R [000F] 1 (data)
06 L     [000F] 1 
07 H     [0071] 1 
07 L     [0071] 1 
08 H   I [0071] 1 
08 L   I [0071] 1 
09 H   I [0071] 1 
09 L   I [0071] 1 
10 H   I [0071] 1 
10 L     [0071] 1 
00 H M   [0000] 1 
00 L M R [0000] 32 (op. LD (nn),A)
01 H M R [0000] 32 (op. LD (nn),A)
01 L M R [0000] 32 (op. LD (nn),A)
02 H     [0007] 32 
02 L     [0007] 32 (refresh row)
03 H     [0007] 32 (refresh row)
03 L     [0007] 32 
04 H     [0001] 32 
04 L   R [0001] 19 (data)
05 H   R [0001] 19 (data)
05 L   R [0001] 19 (data)
06 H   R [0001] 19 (data)
06 L     [0001] 19 
07 H     [0002] 19 
07 L   R [0002] 0 (data)
08 H   R [0002] 0 (data)
08 L   R [0002] 0 (data)
09 H   R [0002] 0 (data)
09 L     [0002] 0 
10 H     [0009] 0 
10 L     [0009] 0 
11 H     [0009] 0 
10
32 19 0 A7 C2 5 0 76 25 1 0 0 0 0 0 0 
11 L   W [0009] 1 (write)
10
32 19 0 A7 C2 5 0 76 25 1 0 0 0 0 0 0 
12 H   W [0009] 1 (write)
12 L     [0009] 1 
00 H M   [0003] 1 
00 L M R [0003] A7 (op. AND A)
01 H M R [0003] A7 (op. AND A)
01 L M R [0003] A7 (op. AND A)
02 H     [0008] A7 
02 L     [0008] A7 (refresh row)
03 H     [0008] A7 (refresh row)
03 L     [0008] A7 
00 H M   [0004] A7 
00 L M R [0004] C2 (op. JP NZ,nn)
01 H M R [0004] C2 (op. JP NZ,nn)
01 L M R [0004] C2 (op. JP NZ,nn)
02 H     [0009] C2 
02 L     [0009] C2 (refresh row)
03 H     [0009] C2 (refresh row)
03 L     [0009] C2 
04 H     [0005] C2 
04 L   R [0005] 5 (data)
05 H   R [0005] 5 (data)
05 L   R [0005] 5 (data)
06 H   R [0005] 5 (data)
06 L     [0005] 5 
07 H     [0006] 5 
07 L   R [0006] 0 (data)
08 H   R [0006] 0 (data)
08 L   R [0006] 0 (data)
09 H   R [0006] 0 (data)
09 L     [0006] 0 
00 H M   [0005] 0 
00 L M R [0005] 3A (op. LD A,(nn))
01 H M R [0005] 3A (op. LD A,(nn))
01 L M R [0005] 3A (op. LD A,(nn))
02 H     [000A] 3A 
02 L     [000A] 3A (refresh row)
03 H     [000A] 3A (refresh row)
03 L     [000A] 3A 
04 H     [0006] 3A 
04 L   R [0006] 18 (data)
05 H   R [0006] 18 (data)
05 L   R [0006] 18 (data)
06 H   R [0006] 18 (data)
06 L     [0006] 18 
07 H     [0007] 18 
07 L   R [0007] 0 (data)
08 H   R [0007] 0 (data)
08 L   R [0007] 0 (data)
09 H   R [0007] 0 (data)
09 L     [0007] 0 
10 H     [0008] 0 
10 L   R [0008] 25 (data)
11 H   R [0008] 25 (data)
11 L   R [0008] 25 (data)
12 H   R [0008] 25 (data)
12 L     [0008] 25 
00 H M   [0008] 25 
00 L M R [0008] 3C (op. INC A)
01 H M R [0008] 3C (op. INC A)
01 L M R [0008] 3C (op. INC A)
02 H     [000B] 3C 
02 L     [000B] 3C (refresh row)
03 H     [000B] 3C (refresh row)
03 L     [000B] 3C 
00 H M   [0009] 3C 
00 L M R [0009] D3 (op. OUT (n),A)
01 H M R [0009] D3 (op. OUT (n),A)
01 L M R [0009] D3 (op. OUT (n),A)
02 H     [000C] D3 
02 L     [000C] D3 (refresh row)
03 H     [000C] D3 (refresh row)
03 L     [000C] D3 
04 H     [000A] D3 
04 L   R [000A] 2 (data)
05 H   R [000A] 2 (data)
05 L   R [000A] 2 (data)
06 H   R [000A] 2 (data)
06 L     [000A] 2 
07 H     [0062] 2 
07 L     [0062] 2 
08 H   O [0062] 26 
08 L   O [0062] 26 
09 H   O [0062] 26 
09 L   O [0062] 26 
10 H   O [0062] 26 
10 L     [0062] 26 
00 H M   [000B] 26 
00 L M R [000B] 32 (op. LD (nn),A)
01 H M R [000B] 32 (op. LD (nn),A)
01 L M R [000B] 32 (op. LD (nn),A)
02 H     [000D] 32 
02 L     [000D] 32 (refresh row)
03 H     [000D] 32 (refresh row)
03 L     [000D] 32 
04 H     [000C] 32 
04 L   R [000C] 18 (data)
05 H   R [000C] 18 (data)
05 L   R [000C] 18 (data)
06 H   R [000C] 18 (data)
06 L     [000C] 18 
07 H     [000D] 18 
07 L   R [000D] 0 (data)
08 H   R [000D] 0 (data)
08 L   R [000D] 0 (data)
09 H   R [000D] 0 (data)
09 L     [000D] 0 
10 H     [0008] 0 
10 L     [0008] 0 
11 H     [0008] 0 
10
32 19 0 A7 C2 5 0 76 26 1 0 0 0 0 0 0 
11 L   W [0008] 26 (write)
10
32 19 0 A7 C2 5 0 76 26 1 0 0 0 0 0 0 
12 H   W [0008] 26 (write)
12 L     [0008] 26 
00 H M   [000E] 26 
00 L M R [000E] DB (op. )
01 H M R [000E] DB (op. )
01 L M R [000E] DB (op. )
02 H     [000E] DB 
02 L     [000E] DB (refresh row)
03 H     [000E] DB (refresh row)
03 L     [000E] DB 
04 H     [000F] DB 
04 L   R [000F] 1 (data)
05 H   R [000F] 1 (data)
05 L   R [000F] 1 (data)
06 H   R [000F] 1 (data)
06 L     [000F] 1 
07 H     [0061] 1 
07 L     [0061] 1 
08 H   I [0061] 1 
08 L   I [0061] 1 
09 H   I [0061] 1 
09 L   I [0061] 1 
10 H   I [0061] 1 
10 L     [0061] 1 
00 H M   [0000] 1 
00 L M R [0000] 32 (op. LD (nn),A)
01 H M R [0000] 32 (op. LD (nn),A)
01 L M R [0000] 32 (op. LD (nn),A)
02 H     [000F] 32 
02 L     [000F] 32 (refresh row)
03 H     [000F] 32 (refresh row)
03 L     [000F] 32 
04 H     [0001] 32 
04 L   R [0001] 19 (data)
05 H   R [0001] 19 (data)
05 L   R [0001] 19 (data)
06 H   R [0001] 19 (data)
06 L     [0001] 19 
07 H     [0002] 19 
07 L   R [0002] 0 (data)
08 H   R [0002] 0 (data)
08 L   R [0002] 0 (data)
09 H   R [0002] 0 (data)
09 L     [0002] 0 
10 H     [0009] 0 
10 L     [0009] 0 
11 H     [0009] 0 
10
32 19 0 A7 C2 5 0 76 26 1 0 0 0 0 0 0 
11 L   W [0009] 1 (write)
10
32 19 0 A7 C2 5 0 76 26 1 0 0 0 0 0 0 
12 H   W [0009] 1 (write)
12 L     [0009] 1 
00 H M   [0003] 1 
00 L M R [0003] A7 (op. AND A)
01 H M R [0003] A7 (op. AND A)
01 L M R [0003] A7 (op. AND A)
02 H     [0000] A7 
02 L     [0000] A7 (refresh row)
03 H     [0000] A7 (refresh row)
03 L     [0000] A7 
00 H M   [0004] A7 
00 L M R [0004] C2 (op. JP NZ,nn)
01 H M R [0004] C2 (op. JP NZ,nn)
01 L M R [0004] C2 (op. JP NZ,nn)
02 H     [0001] C2 
02 L     [0001] C2 (refresh row)
03 H     [0001] C2 (refresh row)
03 L     [0001] C2 
04 H     [0005] C2 
04 L   R [0005] 5 (data)
05 H   R [0005] 5 (data)
05 L   R [0005] 5 (data)
06 H   R [0005] 5 (data)
06 L     [0005] 5 
07 H     [0006] 5 
07 L   R [0006] 0 (data)
08 H   R [0006] 0 (data)
08 L   R [0006] 0 (data)
09 H   R [0006] 0 (data)
09 L     [0006] 0 
00 H M   [0005] 0 
00 L M R [0005] 3A (op. LD A,(nn))
01 H M R [0005] 3A (op. LD A,(nn))
01 L M R [0005] 3A (op. LD A,(nn))
02 H     [0002] 3A 
02 L     [0002] 3A (refresh row)
03 H     [0002] 3A (refresh row)
03 L     [0002] 3A 
04 H     [0006] 3A 
04 L   R [0006] 18 (data)
05 H   R [0006] 18 (data)
05 L   R [0006] 18 (data)
06 H   R [0006] 18 (data)
06 L     [0006] 18 
07 H     [0007] 18 
07 L   R [0007] 0 (data)
08 H   R [0007] 0 (data)
08 L   R [0007] 0 (data)
09 H   R [0007] 0 (data)
09 L     [0007] 0 
10 H     [0008] 0 
10 L   R [0008] 26 (data)
11 H   R [0008] 26 (data)
11 L   R [0008] 26 (data)
12 H   R [0008] 26 (data)
12 L     [0008] 26 
00 H M   [0008] 26 
00 L M R [0008] 3C (op. INC A)
01 H M R [0008] 3C (op. INC A)
01 L M R [0008] 3C (op. INC A)
02 H     [0003] 3C 
02 L     [0003] 3C (refresh row)
03 H     [0003] 3C (refresh row)
03 L     [0003] 3C 
00 H M   [0009] 3C 
00 L M R [0009] D3 (op. OUT (n),A)
01 H M R [0009] D3 (op. OUT (n),A)
01 L M R [0009] D3 (op. OUT (n),A)
02 H     [0004] D3 
02 L     [0004] D3 (refresh row)
03 H     [0004] D3 (refresh row)
03 L     [0004] D3 
04 H     [000A] D3 
04 L   R [000A] 2 (data)
05 H   R [000A] 2 (data)
05 L   R [000A] 2 (data)
06 H   R [000A] 2 (data)
06 L     [000A] 2 
07 H     [0072] 2 
07 L     [0072] 2 
08 H   O [0072] 27 
08 L   O [0072] 27 
09 H   O [0072] 27 
09 L   O [0072] 27 
10 H   O [0072] 27 
10 L     [0072] 27 
00 H M   [000B] 27 
00 L M R [000B] 32 (op. LD (nn),A)
01 H M R [000B] 32 (op. LD (nn),A)
01 L M R [000B] 32 (op. LD (nn),A)
02 H     [0005] 32 
02 L     [0005] 32 (refresh row)
03 H     [0005] 32 (refresh row)
03 L     [0005] 32 
04 H     [000C] 32 
04 L   R [000C] 18 (data)
05 H   R [000C] 18 (data)
05 L   R [000C] 18 (data)
06 H   R [000C] 18 (data)
06 L     [000C] 18 
07 H     [000D] 18 
07 L   R [000D] 0 (data)
08 H   R [000D] 0 (data)
08 L   R [000D] 0 (data)
09 H   R [000D] 0 (data)
09 L     [000D] 0 
10 H     [0008] 0 
10 L     [0008] 0 
11 H     [0008] 0 
10
32 19 0 A7 C2 5 0 76 27 1 0 0 0 0 0 0 
11 L   W [0008] 27 (write)
10
32 19 0 A7 C2 5 0 76 27 1 0 0 0 0 0 0 
12 H   W [0008] 27 (write)
12 L     [0008] 27 
00 H M   [000E] 27 
00 L M R [000E] DB (op. )
01 H M R [000E] DB (op. )
01 L M R [000E] DB (op. )
02 H     [0006] DB 
02 L     [0006] DB (refresh row)
03 H     [0006] DB (refresh row)
03 L     [0006] DB 
04 H     [000F] DB 
04 L   R [000F] 1 (data)
05 H   R [000F] 1 (data)
05 L   R [000F] 1 (data)
06 H   R [000F] 1 (data)
06 L     [000F] 1 
07 H     [0071] 1 
07 L     [0071] 1 
08 H   I [0071] 1 
08 L   I [0071] 1 
09 H   I [0071] 1 
09 L   I [0071] 1 
10 H   I [0071] 1 
10 L     [0071] 1 
00 H M   [0000] 1 
00 L M R [0000] 32 (op. LD (nn),A)
01 H M R [0000] 32 (op. LD (nn),A)
01 L M R [0000] 32 (op. LD (nn),A)
02 H     [0007] 32 
02 L     [0007] 32 (refresh row)
03 H     [0007] 32 (refresh row)
03 L     [0007] 32 
04 H     [0001] 32 
04 L   R [0001] 19 (data)
05 H   R [0001] 19 (data)
05 L   R [0001] 19 (data)
06 H   R [0001] 19 (data)
06 L     [0001] 19 
07 H     [0002] 19 
07 L   R [0002] 0 (data)
08 H   R [0002] 0 (data)
08 L   R [0002] 0 (data)
09 H   R [0002] 0 (data)
09 L     [0002] 0 
10 H     [0009] 0 
10 L     [0009] 0 
11 H     [0009] 0 
10
32 19 0 A7 C2 5 0 76 27 1 0 0 0 0 0 0 
11 L   W [0009] 1 (write)
10
32 19 0 A7 C2 5 0 76 27 1 0 0 0 0 0 0 
12 H   W [0009] 1 (write)
12 L     [0009] 1 
00 H M   [0003] 1 
00 L M R [0003] A7 (op. AND A)
01 H M R [0003] A7 (op. AND A)
01 L M R [0003] A7 (op. AND A)
02 H     [0008] A7 
02 L     [0008] A7 (refresh row)
03 H     [0008] A7 (refresh row)
03 L     [0008] A7 
00 H M   [0004] A7 
00 L M R [0004] C2 (op. JP NZ,nn)
01 H M R [0004] C2 (op. JP NZ,nn)
01 L M R [0004] C2 (op. JP NZ,nn)
02 H     [0009] C2 
02 L     [0009] C2 (refresh row)
03 H     [0009] C2 (refresh row)
03 L     [0009] C2 
04 H     [0005] C2 
04 L   R [0005] 5 (data)
05 H   R [0005] 5 (data)
05 L   R [0005] 5 (data)
06 H   R [0005] 5 (data)
06 L     [0005] 5 
07 H     [0006] 5 
07 L   R [0006] 0 (data)
08 H   R [0006] 0 (data)
08 L   R [0006] 0 (data)
09 H   R [0006] 0 (data)
09 L     [0006] 0 
00 H M   [0005] 0 
00 L M R [0005] 3A (op. LD A,(nn))
01 H M R [0005] 3A (op. LD A,(nn))
01 L M R [0005] 3A (op. LD A,(nn))
02 H     [000A] 3A 
02 L     [000A] 3A (refresh row)
03 H     [000A] 3A (refresh row)
03 L     [000A] 3A 
04 H     [0006] 3A 
04 L   R [0006] 18 (data)
05 H   R [0006] 18 (data)
05 L   R [0006] 18 (data)
06 H   R [0006] 18 (data)
06 L     [0006] 18 
07 H     [0007] 18 
07 L   R [0007] 0 (data)
08 H   R [0007] 0 (data)
08 L   R [0007] 0 (data)
09 H   R [0007] 0 (data)
09 L     [0007] 0 
10 H     [0008] 0 
10 L   R [0008] 27 (data)
11 H   R [0008] 27 (data)
11 L   R [0008] 27 (data)
12 H   R [0008] 27 (data)
12 L     [0008] 27 
00 H M   [0008] 27 
00 L M R [0008] 3C (op. INC A)
01 H M R [0008] 3C (op. INC A)
01 L M R [0008] 3C (op. INC A)
02 H     [000B] 3C 
02 L     [000B] 3C (refresh row)
03 H     [000B] 3C (refresh row)
03 L     [000B] 3C 
00 H M   [0009] 3C 
00 L M R [0009] D3 (op. OUT (n),A)
01 H M R [0009] D3 (op. OUT (n),A)
01 L M R [0009] D3 (op. OUT (n),A)
02 H     [000C] D3 
02 L     [000C] D3 (refresh row)
03 H     [000C] D3 (refresh row)
03 L     [000C] D3 
04 H     [000A] D3 
04 L   R [000A] 2 (data)
05 H   R [000A] 2 (data)
05 L   R [000A] 2 (data)
06 H   R [000A] 2 (data)
06 L     [000A] 2 
07 H     [00A2] 2 
07 L     [00A2] 2 
08 H   O [00A2] 28 
08 L   O [00A2] 28 
09 H   O [00A2] 28 
09 L   O [00A2] 28 
10 H   O [00A2] 28 
10 L     [00A2] 28 
00 H M   [000B] 28 
00 L M R [000B] 32 (op. LD (nn),A)
01 H M R [000B] 32 (op. LD (nn),A)
01 L M R [000B] 32 (op. LD (nn),A)
02 H     [000D] 32 
02 L     [000D] 32 (refresh row)
03 H     [000D] 32 (refresh row)
03 L     [000D] 32 
04 H     [000C] 32 
04 L   R [000C] 18 (data)
05 H   R [000C] 18 (data)
05 L   R [000C] 18 (data)
06 H   R [000C] 18 (data)
06 L     [000C] 18 
07 H     [000D] 18 
07 L   R [000D] 0 (data)
08 H   R [000D] 0 (data)
08 L   R [000D] 0 (data)
09 H   R [000D] 0 (data)
09 L     [000D] 0 
10 H     [0008] 0 
10 L     [0008] 0 
11 H     [0008] 0 
10
32 19 0 A7 C2 5 0 76 28 1 0 0 0 0 0 0 
11 L   W [0008] 28 (write)
10
32 19 0 A7 C2 5 0 76 28 1 0 0 0 0 0 0 
12 H   W [0008] 28 (write)
12 L     [0008] 28 
00 H M   [000E] 28 
00 L M R [000E] DB (op. )
01 H M R [000E] DB (op. )
01 L M R [000E] DB (op. )
02 H     [000E] DB 
02 L     [000E] DB (refresh row)
03 H     [000E] DB (refresh row)
03 L     [000E] DB 
04 H     [000F] DB 
04 L   R [000F] 1 (data)
05 H   R [000F] 1 (data)
05 L   R [000F] 1 (data)
06 H   R [000F] 1 (data)
06 L     [000F] 1 
07 H     [00A1] 1 
07 L     [00A1] 1 
08 H   I [00A1] 1 
08 L   I [00A1] 1 
09 H   I [00A1] 1 
09 L   I [00A1] 1 
10 H   I [00A1] 1 
10 L     [00A1] 1 
00 H M   [0000] 1 
00 L M R [0000] 32 (op. LD (nn),A)
01 H M R [0000] 32 (op. LD (nn),A)
01 L M R [0000] 32 (op. LD (nn),A)
02 H     [000F] 32 
02 L     [000F] 32 (refresh row)
03 H     [000F] 32 (refresh row)
03 L     [000F] 32 
04 H     [0001] 32 
04 L   R [0001] 19 (data)
05 H   R [0001] 19 (data)
05 L   R [0001] 19 (data)
06 H   R [0001] 19 (data)
06 L     [0001] 19 
07 H     [0002] 19 
07 L   R [0002] 0 (data)
08 H   R [0002] 0 (data)
08 L   R [0002] 0 (data)
09 H   R [0002] 0 (data)
09 L     [0002] 0 
10 H     [0009] 0 
10 L     [0009] 0 
11 H     [0009] 0 
10
32 19 0 A7 C2 5 0 76 28 1 0 0 0 0 0 0 
11 L   W [0009] 1 (write)
10
32 19 0 A7 C2 5 0 76 28 1 0 0 0 0 0 0 
12 H   W [0009] 1 (write)
12 L     [0009] 1 
00 H M   [0003] 1 
00 L M R [0003] A7 (op. AND A)
01 H M R [0003] A7 (op. AND A)
01 L M R [0003] A7 (op. AND A)
02 H     [0000] A7 
02 L     [0000] A7 (refresh row)
03 H     [0000] A7 (refresh row)
03 L     [0000] A7 
00 H M   [0004] A7 
00 L M R [0004] C2 (op. JP NZ,nn)
01 H M R [0004] C2 (op. JP NZ,nn)
01 L M R [0004] C2 (op. JP NZ,nn)
02 H     [0001] C2 
02 L     [0001] C2 (refresh row)
03 H     [0001] C2 (refresh row)
03 L     [0001] C2 
04 H     [0005] C2 
04 L   R [0005] 5 (data)
05 H   R [0005] 5 (data)
05 L   R [0005] 5 (data)
06 H   R [0005] 5 (data)
06 L     [0005] 5 
07 H     [0006] 5 
07 L   R [0006] 0 (data)
08 H   R [0006] 0 (data)
08 L   R [0006] 0 (data)
09 H   R [0006] 0 (data)
09 L     [0006] 0 
00 H M   [0005] 0 
00 L M R [0005] 3A (op. LD A,(nn))
01 H M R [0005] 3A (op. LD A,(nn))
01 L M R [0005] 3A (op. LD A,(nn))
02 H     [0002] 3A 
02 L     [0002] 3A (refresh row)
03 H     [0002] 3A (refresh row)
03 L     [0002] 3A 
04 H     [0006] 3A 
04 L   R [0006] 18 (data)
05 H   R [0006] 18 (data)
05 L   R [0006] 18 (data)
06 H   R [0006] 18 (data)
06 L     [0006] 18 
07 H     [0007] 18 
07 L   R [0007] 0 (data)
08 H   R [0007] 0 (data)
08 L   R [0007] 0 (data)
09 H   R [0007] 0 (data)
09 L     [0007] 0 
10 H     [0008] 0 
10 L   R [0008] 28 (data)
11 H   R [0008] 28 (data)
11 L   R [0008] 28 (data)
12 H   R [0008] 28 (data)
12 L     [0008] 28 
00 H M   [0008] 28 
00 L M R [0008] 3C (op. INC A)
01 H M R [0008] 3C (op. INC A)
01 L M R [0008] 3C (op. INC A)
02 H     [0003] 3C 
02 L     [0003] 3C (refresh row)
03 H     [0003] 3C (refresh row)
03 L     [0003] 3C 
00 H M   [0009] 3C 
00 L M R [0009] D3 (op. OUT (n),A)
01 H M R [0009] D3 (op. OUT (n),A)
01 L M R [0009] D3 (op. OUT (n),A)
02 H     [0004] D3 
02 L     [0004] D3 (refresh row)
03 H     [0004] D3 (refresh row)
03 L     [0004] D3 
04 H     [000A] D3 
04 L   R [000A] 2 (data)
05 H   R [000A] 2 (data)
05 L   R [000A] 2 (data)
06 H   R [000A] 2 (data)
06 L     [000A] 2 
07 H     [00B2] 2 
07 L     [00B2] 2 
08 H   O [00B2] 29 
08 L   O [00B2] 29 
09 H   O [00B2] 29 
09 L   O [00B2] 29 
10 H   O [00B2] 29 
10 L     [00B2] 29 
00 H M   [000B] 29 
00 L M R [000B] 32 (op. LD (nn),A)
01 H M R [000B] 32 (op. LD (nn),A)
01 L M R [000B] 32 (op. LD (nn),A)
02 H     [0005] 32 
02 L     [0005] 32 (refresh row)
03 H     [0005] 32 (refresh row)
03 L     [0005] 32 
04 H     [000C] 32 
04 L   R [000C] 18 (data)
05 H   R [000C] 18 (data)
05 L   R [000C] 18 (data)
06 H   R [000C] 18 (data)
06 L     [000C] 18 
07 H     [000D] 18 
07 L   R [000D] 0 (data)
08 H   R [000D] 0 (data)
08 L   R [000D] 0 (data)
09 H   R [000D] 0 (data)
09 L     [000D] 0 
10 H     [0008] 0 
10 L     [0008] 0 
11 H     [0008] 0 
10
32 19 0 A7 C2 5 0 76 29 1 0 0 0 0 0 0 
11 L   W [0008] 29 (write)
10
32 19 0 A7 C2 5 0 76 29 1 0 0 0 0 0 0 
12 H   W [0008] 29 (write)
12 L     [0008] 29 
00 H M   [000E] 29 
00 L M R [000E] DB (op. )
01 H M R [000E] DB (op. )
01 L M R [000E] DB (op. )
02 H     [0006] DB 
02 L     [0006] DB (refresh row)
03 H     [0006] DB (refresh row)
03 L     [0006] DB 
04 H     [000F] DB 
04 L   R [000F] 1 (data)
05 H   R [000F] 1 (data)
05 L   R [000F] 1 (data)
06 H   R [000F] 1 (data)
06 L     [000F] 1 
07 H     [00B1] 1 
07 L     [00B1] 1 
08 H   I [00B1] 1 
08 L   I [00B1] 1 
09 H   I [00B1] 1 
09 L   I [00B1] 1 
10 H   I [00B1] 1 
10 L     [00B1] 1 
00 H M   [0000] 1 
00 L M R [0000] 32 (op. LD (nn),A)
01 H M R [0000] 32 (op. LD (nn),A)
01 L M R [0000] 32 (op. LD (nn),A)
02 H     [0007] 32 
02 L     [0007] 32 (refresh row)
03 H     [0007] 32 (refresh row)
03 L     [0007] 32 
04 H     [0001] 32 
04 L   R [0001] 19 (data)
05 H   R [0001] 19 (data)
05 L   R [0001] 19 (data)
06 H   R [0001] 19 (data)
06 L     [0001] 19 
07 H     [0002] 19 
07 L   R [0002] 0 (data)
08 H   R [0002] 0 (data)
08 L   R [0002] 0 (data)
09 H   R [0002] 0 (data)
09 L     [0002] 0 
10 H     [0009] 0 
10 L     [0009] 0 
11 H     [0009] 0 
10
32 19 0 A7 C2 5 0 76 29 1 0 0 0 0 0 0 
11 L   W [0009] 1 (write)
10
32 19 0 A7 C2 5 0 76 29 1 0 0 0 0 0 0 
12 H   W [0009] 1 (write)
12 L     [0009] 1 
00 H M   [0003] 1 
00 L M R [0003] A7 (op. AND A)
01 H M R [0003] A7 (op. AND A)
01 L M R [0003] A7 (op. AND A)
02 H     [0008] A7 
02 L     [0008] A7 (refresh row)
03 H     [0008] A7 (refresh row)
03 L     [0008] A7 
00 H M   [0004] A7 
00 L M R [0004] C2 (op. JP NZ,nn)
01 H M R [0004] C2 (op. JP NZ,nn)
01 L M R [0004] C2 (op. JP NZ,nn)
02 H     [0009] C2 
02 L     [0009] C2 (refresh row)
03 H     [0009] C2 (refresh row)
03 L     [0009] C2 
04 H     [0005] C2 
04 L   R [0005] 5 (data)
05 H   R [0005] 5 (data)
05 L   R [0005] 5 (data)
06 H   R [0005] 5 (data)
06 L     [0005] 5 
07 H     [0006] 5 
07 L   R [0006] 0 (data)
08 H   R [0006] 0 (data)
08 L   R [0006] 0 (data)
09 H   R [0006] 0 (data)
09 L     [0006] 0 
00 H M   [0005] 0 
00 L M R [0005] 3A (op. LD A,(nn))
01 H M R [0005] 3A (op. LD A,(nn))
01 L M R [0005] 3A (op. LD A,(nn))
02 H     [000A] 3A 
02 L     [000A] 3A (refresh row)
03 H     [000A] 3A (refresh row)
03 L     [000A] 3A 
04 H     [0006] 3A 
04 L   R [0006] 18 (data)
05 H   R [0006] 18 (data)
05 L   R [0006] 18 (data)
06 H   R [0006] 18 (data)
06 L     [0006] 18 
07 H     [0007] 18 
07 L   R [0007] 0 (data)
08 H   R [0007] 0 (data)
08 L   R [0007] 0 (data)
09 H   R [0007] 0 (data)
09 L     [0007] 0 
10 H     [0008] 0 
10 L   R [0008] 29 (data)
11 H   R [0008] 29 (data)
11 L   R [0008] 29 (data)
12 H   R [0008] 29 (data)
12 L     [0008] 29 
00 H M   [0008] 29 
00 L M R [0008] 3C (op. INC A)
01 H M R [0008] 3C (op. INC A)
01 L M R [0008] 3C (op. INC A)
02 H     [000B] 3C 
02 L     [000B] 3C (refresh row)
03 H     [000B] 3C (refresh row)
03 L     [000B] 3C 
00 H M   [0009] 3C 
00 L M R [0009] D3 (op. OUT (n),A)
01 H M R [0009] D3 (op. OUT (n),A)
01 L M R [0009] D3 (op. OUT (n),A)
02 H     [000C] D3 
02 L     [000C] D3 (refresh row)
03 H     [000C] D3 (refresh row)
03 L     [000C] D3 
04 H     [000A] D3 
04 L   R [000A] 2 (data)
05 H   R [000A] 2 (data)
05 L   R [000A] 2 (data)
06 H   R [000A] 2 (data)
06 L     [000A] 2 
07 H     [00A2] 2 
07 L     [00A2] 2 
08 H   O [00A2] 2A 
08 L   O [00A2] 2A 
09 H   O [00A2] 2A 
09 L   O [00A2] 2A 
10 H   O [00A2] 2A 
10 L     [00A2] 2A 
00 H M   [000B] 2A 
00 L M R [000B] 32 (op. LD (nn),A)
01 H M R [000B] 32 (op. LD (nn),A)
01 L M R [000B] 32 (op. LD (nn),A)
02 H     [000D] 32 
02 L     [000D] 32 (refresh row)
03 H     [000D] 32 (refresh row)
03 L     [000D] 32 
04 H     [000C] 32 
04 L   R [000C] 18 (data)
05 H   R [000C] 18 (data)
05 L   R [000C] 18 (data)
06 H   R [000C] 18 (data)
06 L     [000C] 18 
07 H     [000D] 18 
07 L   R [000D] 0 (data)
08 H   R [000D] 0 (data)
08 L   R [000D] 0 (data)
09 H   R [000D] 0 (data)
09 L     [000D] 0 
10 H     [0008] 0 
10 L     [0008] 0 
11 H     [0008] 0 
10
32 19 0 A7 C2 5 0 76 2A 1 0 0 0 0 0 0 
11 L   W [0008] 2A (write)
10
32 19 0 A7 C2 5 0 76 2A 1 0 0 0 0 0 0 
12 H   W [0008] 2A (write)
12 L     [0008] 2A 
00 H M   [000E] 2A 
00 L M R [000E] DB (op. )
01 H M R [000E] DB (op. )
01 L M R [000E] DB (op. )
02 H     [000E] DB 
02 L     [000E] DB (refresh row)
03 H     [000E] DB (refresh row)
03 L     [000E] DB 
04 H     [000F] DB 
04 L   R [000F] 1 (data)
05 H   R [000F] 1 (data)
05 L   R [000F] 1 (data)
06 H   R [000F] 1 (data)
06 L     [000F] 1 
07 H     [00A1] 1 
07 L     [00A1] 1 
08 H   I [00A1] 1 
08 L   I [00A1] 1 
09 H   I [00A1] 1 
09 L   I [00A1] 1 
10 H   I [00A1] 1 
10 L     [00A1] 1 
00 H M   [0000] 1 
00 L M R [0000] 32 (op. LD (nn),A)
01 H M R [0000] 32 (op. LD (nn),A)
01 L M R [0000] 32 (op. LD (nn),A)
02 H     [000F] 32 
02 L     [000F] 32 (refresh row)

 */
