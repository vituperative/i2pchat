# I2P Chat Protocol

@protocol specs
The protocol spec is at in the source - code (cProcol.h and cProtocol.cpp).
It's a easy - Stringbased Protocol...

First packet on connection must be:
`CHATSYSTEM\tProtocolVersion\n` → for normal Connection
`CHATSYSTEMFILETRANSFER\tProtocolVersion\nSizeinBit\nFileName` → for FileTransfer
else
send `<the html info-page >`


Every packet must be &gt;= 8 Byte
1-4 Byte = Paketlength in Byte (HEX) without the 4 Byte Paketlength
5-8 Byte = PaketInfo
&gt;8 Byte = PaketData

Byte 5-8 are for the Tags like, GET_CLIENTNAME = 1002 and the other side answer with 
ANSWER_OF_GET_CLIENTVERSION = 0001... and so on 

//from http://forum.i2p/viewtopic.php?p=30353#30353
