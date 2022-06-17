# q1decode

Q1decode is supposed to allow decoding of disks written By the Q1 computer.

```
CNT: 01783 ADDRESS MARK: 55424954   
00    00    00    10  V
CNT: 017D9 DATA    MARK: 55424945   
00    00    49 I  4E N  44 D  45 E  58 X  20    20    20    0F    00    28 (  00    82    00       
00    00    00    00    00    00    00    00    00    00    00    00    00    00    00    00    
00    00    00    00    00    00    00    00    2C ,  10    00  V
CNT: 01987 ADDRESS MARK: 55424954   
00    01    01    10    03    FF    FF    FF    FF    FF  V
CNT: 019DC DATA    MARK: 55424945   
00    00    56 V  31 1  30 0  52 R  47 G  45 E  4E N  41 A  23 #  02    4F O  00    52 R  00    
01    00    07    00    00    00    00    00    00    00    00    00    00    00    00    00    
00    00    00    00    00    00    00    00    8D    10  V  00  
CNT: 01B7C ADDRESS MARK: 55424954   
00    02    02    10    00    00    00    00    00    00  V
CNT: 01BD2 DATA    MARK: 55424945   
00    00    58 X  4B K  4F O  4C L  46 F  4F O  52 R  4D M  07    00    2F /  00    7A z  00    
08    00    08    00    00    00    00    00    00    00    00    00    00    00    00    00    
00    00    00    00    00    00    00    00  
CNT: 01783 ADDRESS MARK: 55424954   
00    00    00    10  V
CNT: 017D9 DATA    MARK: 55424945   
00    00    49 I  4E N  44 D  45 E  58 X  20    20    20    0F    00    28 (  00    82    00       
00    00    00    00    00    00    00    00    00    00    00    00    00    00    00    00    
00    00    00    00    00    00    00    00    2C ,  10    00  V
CNT: 01987 ADDRESS MARK: 55424954   
00    01    01    10    03    FF    FF    FF    FF    FF  V
CNT: 019DC DATA    MARK: 55424945   
00    00    56 V  31 1  30 0  52 R  47 G  45 E  4E N  41 A  23 #  02    4F O  00    52 R  00    
01    00    07    00    00    00    00    00    00    00    00    00    00    00    00    00    
00    00    00    00    00    00    00    00    8D    10  V  00  
CNT: 01B7C ADDRESS MARK: 55424954   
00    02    02    10    00    00    00    00    00    00  V
CNT: 01BD2 DATA    MARK: 55424945   
00    00    58 X  4B K  4F O  4C L  46 F  4F O  52 R  4D M  07    00    2F /  00    7A z  00    
08    00    08    00    00    00    00    00    00    00    00    00    00    00    00    00    
00    00    00    00    00    00    00    00    CD    10    00    00    00    00    00    00    
00    9F    FF    FF    FF    FF    FF  V
CNT: 01D74 ADDRESS MARK: 55424954   
00    03    03    10    00    00    00    00    00    00  V
CNT: 01DC9 DATA    MARK: 55424945   
00    00    59 Y  4B K  4F O  4C L  46 F  4F O  52 R  4D M  02    00    2F /  00    7A z  00    
09    00    09    00    00    00    00    00    00    00    00    00    00    00    00    00    
00    00    00    00    00    00    00    00    CB    10    01    FF    FF    FF    FF    FF    
FD  V
CD    10    00    00    00    00    00    00    
00    9F    FF    FF    FF    FF    FF  V
CNT: 01D74 ADDRESS MARK: 55424954   
00    03    03    10    00    00    00    00    00    00  V
CNT: 01DC9 DATA    MARK: 55424945   
00    00    59 Y  4B K  4F O  4C L  46 F  4F O  52 R  4D M  02    00    2F /  00    7A z  00    
09    00    09    00    00    00    00    00    00    00    00    00    00    00    00    00    
00    00    00    00    00    00    00    00    CB    10    01    FF    FF    FF    FF    FF    
FD  V
```

This is a partial decode of the first track. All records end with a 10h digit it seems. It also seems that the byte just before the end marker is a checksum. For the address field it seems like this is just a simple sum of the prior bytes.

For the address field the first byte is the track and the second byte is the record.

For the data field the meaning of the bytes are as follows:
* Byte 0-1: Unknown.
* Byte 2-9: Filename.
* Byte 10-11: Number of records in file. The first byte is the LSB and the following byte is the MSB. The record counter should stay below this value. 
* Byte 12-13: Record size in bytes. The first byte is the LSB and the following byte is the MSB.
* Byte 14-15: Records per track in this file.
* Byte 16-17: Start track
* Byte 18-19: End track.
* Byte 20-39: Unknown. Seems to be zero.
* Byte 40: Some kind of checksum.
* Byte 41: Always 10h.

This is the layout of the directory. Called INDEX in Q1. But the same general principle hold for the other tracks where data is stored. Could it be text or binary object files. It can be noted that the file called V10REGNA has a record size of 79 bytes, and the contents are a source code file. Binary object file seems to be using 255 bytes record size.

I think this is amoong one of the weirdest disk layouts I have seen for a microcomputer but ist somehow ressembles how the IBM S/360 did the disk layout. Where everyting is very centered on cylinders, and where you need to specify the record length of a file and size of the file when it is created.
