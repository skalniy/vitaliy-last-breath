#include <SPI.h>
#include <LoRa.h>
#include <SoftwareSerial.h>
#include "header.h"

#define  uchar unsigned char
#define uint  unsigned int
#define MAX_LEN 16
#define TIME_HEADER  "T"   // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message 

const int chipSelectPin = 53;//if the controller is UNO,328,168
const int NRSTPD = 5;
const int ss = 46;          
const int resete = 48;     
const int dio0 = 49;
int counter = 0;
int flag = 0;
int flag1 = 0;

byte buffer_lock = 1;

const char *gpsStream =
  "$GPRMC,045103.000,A,3014.1984,N,09749.2872,W,0.67,161.46,030913,,,A*7C\r\n";
  //"$GPGGA,045104.000,3014.1985,N,09749.2873,W,1,09,1.2,211.6,M,-22.5,M,,0000*62\r\n";

uchar serNum[5];
unsigned char str;
unsigned char info[256];
unsigned char cursa;
int n = 0;
uchar  writeDate[16] ={'T', 'e', 'n', 'g', ' ', 'B', 'o', 0, 0, 0, 0, 0, 0, 0, 0,0};
uchar sectorKeyA[16][16] = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                             {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                             {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                            };
uchar sectorNewKeyA[16][16] = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                                {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xff,0x07,0x80,0x69, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                                {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xff,0x07,0x80,0x69, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                               };

SoftwareSerial mySerial(10, 11); // RX, TX

void print_result()
{
  Serial.print("A7 board info: ");
  while( mySerial.available() ) {
    Serial.write(mySerial.read());
  }
  Serial.println();  
}

void setup() {                
    Serial.begin(9600);                       // RFID reader SOUT pin connected to Serial RX pin at 2400bps 
    Serial1.begin(115200);
    mySerial.begin(9600);
    
    delay(200); 

    // start the SPI library:
    SPI.begin();
    pinMode(ss, OUTPUT);             // Set digital pin 53 as OUTPUT to connect it to the RFID /ENABLE pin 
    digitalWrite(ss, HIGH);
    
    pinMode(chipSelectPin,OUTPUT);             // Set digital pin 53 as OUTPUT to connect it to the RFID /ENABLE pin 
    digitalWrite(chipSelectPin, LOW);          // Activate the RFID reader
    
    pinMode(NRSTPD,OUTPUT);               // Set digital pin 5 , Not Reset and Power-down
    digitalWrite(NRSTPD, HIGH);

    
    MFRC522_Init();  

    while( flag == 0 ) {
        uchar i,tmp;
        uchar status;
        uchar str[MAX_LEN];
        uchar RC_size;
        uchar blockAddr;  //Select the address of the operation 0～63

        status = MFRC522_Request(PICC_REQIDL, str); 
        status = MFRC522_Anticoll(str);
        memcpy(serNum, str, 5);
        if( status == MI_OK ) {
            Serial.println("The card's number is  : ");
            Serial.print(serNum[0], HEX);
            Serial.print(serNum[1], HEX);
            Serial.print(serNum[2], HEX);
            Serial.print(serNum[3], HEX);
            Serial.print(serNum[4], HEX);
            Serial.println(" ");
            flag = 1;
        }

        // select card, return card capacity
        RC_size = MFRC522_SelectTag(serNum);
       
        // write data card
        blockAddr = 7;    // data block 7   
        status = MFRC522_Auth(PICC_AUTHENT1A, blockAddr, sectorKeyA[blockAddr/4], serNum);  // authentication
        if (status == MI_OK)
        {
            // write data
            status = MFRC522_Write(blockAddr, sectorNewKeyA[blockAddr/4]);
            Serial.print("set the new card password, and can modify the data of the Sector: ");
            Serial.print(blockAddr/4,DEC);

            // write data
            blockAddr = blockAddr - 3 ; 
            status = MFRC522_Write(blockAddr, writeDate);
            if( status == MI_OK ) {
                Serial.println("OK!");
            }
        }

        // read card
        blockAddr = 7;    // data block 7   
        status = MFRC522_Auth(PICC_AUTHENT1A, blockAddr, sectorNewKeyA[blockAddr/4], serNum); // authentication
        if( status == MI_OK ) {
            // read data
            blockAddr = blockAddr - 3 ; 
            status = MFRC522_Read(blockAddr, str);
            if( status == MI_OK ) {
                Serial.println("Read from the card ,the data is : ");
                for( i = 0; i < 16; ++i ) {
                    Serial.print(str[i]);
                }
                Serial.println(" ");
            }
        }
        Serial.println(" ");
        MFRC522_Halt();     // command card to enter standby mode 

    }

    Serial.println("LoRa Sender");
    LoRa.setPins(ss, resete, dio0);
    digitalWrite(chipSelectPin, HIGH);
    digitalWrite(ss, LOW); 
    digitalWrite(NRSTPD, LOW);

    if (!LoRa.begin(433E6)) {
        Serial.println("Starting LoRa failed!");
        while (1);
    }

    lora_send_uid();

    LoRa.end();

    Serial.println();

    Serial.println("Send AT command");  
    Serial1.println("AT");
    delay(2500);
    print_result();

    Serial.println("AT+GPS turn on");  
    Serial1.println("AT+GPS=1");  
    delay(1000);
    print_result();

    Serial.println("AT+GPSRD turn on");  
    Serial1.println("AT+GPSRD=1");  
    delay(1000);
    print_result();

    Serial.println("AT+IPR turn on");  
    Serial1.println("AT+IPR=9600");  
    delay(1000);
    print_result();
    Serial.println();
}

void loop()
{
    if( Serial.available() ) {
        mySerial.write(Serial.read());
    }

    if( mySerial.available() ) {
        str = mySerial.read();
        Serial.write(str);
        if (str == '+') {
            cursa = 0;
            buffer_lock = 0;
        }
        if( ! buffer_lock ) {
          info[cursa++] = str;
        }
        if (str == '*') {
            info[cursa++] = 0x00;
            buffer_lock = 1;
            Serial.println(" ");
            Serial.println((char*)info);
            if( flag1 == 0 ) {
                displayInfo();
                flag1 = 1;
            }
        }
    }  
}

void displayInfo()
{

  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  lora_send_time();

  LoRa.end();

  delay(200);

  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  lora_send_location();

  LoRa.end();
  Serial.println();
  flag = 1;

  delay(1000);
}

void Write_MFRC522(uchar addr, uchar val)
{
  digitalWrite(chipSelectPin, LOW);

  SPI.transfer((addr<<1)&0x7E); 
  SPI.transfer(val);
  
  digitalWrite(chipSelectPin, HIGH);
}


uchar Read_MFRC522(uchar addr)
{
  uchar val;

  digitalWrite(chipSelectPin, LOW);

  // address format: 1XXXXXX0
  SPI.transfer(((addr<<1)&0x7E) | 0x80);  
  val =SPI.transfer(0x00);
  
  digitalWrite(chipSelectPin, HIGH);
  
  return val; 
}


void SetBitMask(uchar reg, uchar mask)  
{
    uchar tmp;
    tmp = Read_MFRC522(reg);
    Write_MFRC522(reg, tmp | mask);  // set bit mask
}



void ClearBitMask(uchar reg, uchar mask)  
{
    uchar tmp;
    tmp = Read_MFRC522(reg);
    Write_MFRC522(reg, tmp & (~mask));  // clear bit mask
} 



void AntennaOn(void)
{
  uchar temp;

  temp = Read_MFRC522(TxControlReg);
  if (!(temp & 0x03))
  {
    SetBitMask(TxControlReg, 0x03);
  }
}

void AntennaOff(void)
{
  ClearBitMask(TxControlReg, 0x03);
}

void MFRC522_Reset(void)
{
    Write_MFRC522(CommandReg, PCD_RESETPHASE);
}

void MFRC522_Init(void)
{
  digitalWrite(NRSTPD,HIGH);

  MFRC522_Reset();
    
  //Timer: TPrescaler*TreloadVal/6.78MHz = 24ms
    Write_MFRC522(TModeReg, 0x8D);    //Tauto=1; f(Timer) = 6.78MHz/TPreScaler
    Write_MFRC522(TPrescalerReg, 0x3E); //TModeReg[3..0] + TPrescalerReg
    Write_MFRC522(TReloadRegL, 30);           
    Write_MFRC522(TReloadRegH, 0);
  
  Write_MFRC522(TxAutoReg, 0x40);   //100%ASK
  Write_MFRC522(ModeReg, 0x3D);   //CRC initial value


  AntennaOn();    // open antenna
}
uchar MFRC522_Request(uchar reqMode, uchar *TagType)
{
  uchar status;  
  uint backBits;      // received data bits

  Write_MFRC522(BitFramingReg, 0x07);   //TxLastBists = BitFramingReg[2..0] ???
  
  TagType[0] = reqMode;
  status = MFRC522_ToCard(PCD_TRANSCEIVE, TagType, 1, TagType, &backBits);

  if ((status != MI_OK) || (backBits != 0x10))
  {    
    status = MI_ERR;
  }
   
  return status;
}

uchar MFRC522_ToCard(uchar command, uchar *sendData, uchar sendLen, uchar *backData, uint *backLen)
{
    uchar status = MI_ERR;
    uchar irqEn = 0x00;
    uchar waitIRq = 0x00;
    uchar lastBits;
    uchar n;
    uint i;

    switch (command)
    {
        case PCD_AUTHENT:   // card key authentication
    {
      irqEn = 0x12;
      waitIRq = 0x10;
      break;
    }
    case PCD_TRANSCEIVE:  // send data in FIFO
    {
      irqEn = 0x77;
      waitIRq = 0x30;
      break;
    }
    default:
      break;
    }
   
    Write_MFRC522(CommIEnReg, irqEn|0x80);  // Allow interrupt request
    ClearBitMask(CommIrqReg, 0x80);     // clear bits of all interrupt request
    SetBitMask(FIFOLevelReg, 0x80);     //FlushBuffer=1, FIFO initialization

    
  Write_MFRC522(CommandReg, PCD_IDLE);  //NO action; cancel current command ???

  // write data into FIFO
    for (i=0; i<sendLen; i++)
    {   
    Write_MFRC522(FIFODataReg, sendData[i]);    
  }

  // execute command
  Write_MFRC522(CommandReg, command);
    if (command == PCD_TRANSCEIVE)
    {    
    SetBitMask(BitFramingReg, 0x80);    //StartSend=1,transmission of data starts  
  }   
    
  // wait for the completion of data receiving
  i = 2000; // adjust i according to clock frequency, maximum waiting time of operating M1 is 25ms  ???
    do 
    {
    //CommIrqReg[7..0]
    //Set1 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq
        n = Read_MFRC522(CommIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x01) && !(n&waitIRq));

    ClearBitMask(BitFramingReg, 0x80);      //StartSend=0
  
    if (i != 0)
    {    
        if(!(Read_MFRC522(ErrorReg) & 0x1B))  //BufferOvfl Collerr CRCErr ProtecolErr
        {
            status = MI_OK;
            if (n & irqEn & 0x01)
            {   
        status = MI_NOTAGERR;     //??   
      }

            if (command == PCD_TRANSCEIVE)
            {
                n = Read_MFRC522(FIFOLevelReg);
                lastBits = Read_MFRC522(ControlReg) & 0x07;
                if (lastBits)
                {   
          *backLen = (n-1)*8 + lastBits;   
        }
                else
                {   
          *backLen = n*8;   
        }

                if (n == 0)
                {   
          n = 1;    
        }
                if (n > MAX_LEN)
                {   
          n = MAX_LEN;   
        }
        
        // read data which FIFO received
                for (i=0; i<n; i++)
                {   
          backData[i] = Read_MFRC522(FIFODataReg);    
        }
            }
        }
        else
        {   
      status = MI_ERR;  
    }
        
    }
  
    //SetBitMask(ControlReg,0x80);           //timer stops
    //Write_MFRC522(CommandReg, PCD_IDLE); 

    return status;
}

uchar MFRC522_Anticoll(uchar *serNum)
{
    uchar status;
    uchar i;
  uchar serNumCheck=0;
    uint unLen;
    
  Write_MFRC522(BitFramingReg, 0x00);   //TxLastBists = BitFramingReg[2..0]
 
    serNum[0] = PICC_ANTICOLL;
    serNum[1] = 0x20;
    status = MFRC522_ToCard(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);

    if (status == MI_OK)
  {
    // verify card sequence number
    for (i=0; i<4; i++)
    {   
      serNumCheck ^= serNum[i];
    }
    if (serNumCheck != serNum[i])
    {   
      status = MI_ERR;    
    }
    }

    //SetBitMask(CollReg, 0x80);    //ValuesAfterColl=1

    return status;
} 

void CalulateCRC(uchar *pIndata, uchar len, uchar *pOutData)
{
    uchar i, n;

    ClearBitMask(DivIrqReg, 0x04);      //CRCIrq = 0
    SetBitMask(FIFOLevelReg, 0x80);     // clear FIFO pointer
    //Write_MFRC522(CommandReg, PCD_IDLE);

  // write data into FIFO 
    for (i=0; i<len; i++)
    {   
    Write_MFRC522(FIFODataReg, *(pIndata+i));   
  }
    Write_MFRC522(CommandReg, PCD_CALCCRC);

  // wait for the completion of CRC calculation
    i = 0xFF;
    do 
    {
        n = Read_MFRC522(DivIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x04));      //CRCIrq = 1

  // read CRC calculation result
    pOutData[0] = Read_MFRC522(CRCResultRegL);
    pOutData[1] = Read_MFRC522(CRCResultRegM);
}

uchar MFRC522_SelectTag(uchar *serNum)
{
    uchar i;
  uchar status;
  uchar size;
    uint recvBits;
    uchar buffer[9]; 

  //ClearBitMask(Status2Reg, 0x08);     //MFCrypto1On=0

    buffer[0] = PICC_SElECTTAG;
    buffer[1] = 0x70;
    for (i=0; i<5; i++)
    {
      buffer[i+2] = *(serNum+i);
    }
  CalulateCRC(buffer, 7, &buffer[7]);   //??
    status = MFRC522_ToCard(PCD_TRANSCEIVE, buffer, 9, buffer, &recvBits);
    
    if ((status == MI_OK) && (recvBits == 0x18))
    {   
    size = buffer[0]; 
  }
    else
    {   
    size = 0;    
  }

    return size;
}

uchar MFRC522_Auth(uchar authMode, uchar BlockAddr, uchar *Sectorkey, uchar *serNum)
{
    uchar status;
    uint recvBits;
    uchar i;
  uchar buff[12]; 

  // Verification commands + block address + sector password + card sequence number
    buff[0] = authMode;
    buff[1] = BlockAddr;
    for (i=0; i<6; i++)
    {    
    buff[i+2] = *(Sectorkey+i);   
  }
    for (i=0; i<4; i++)
    {    
    buff[i+8] = *(serNum+i);   
  }
    status = MFRC522_ToCard(PCD_AUTHENT, buff, 12, buff, &recvBits);

    if ((status != MI_OK) || (!(Read_MFRC522(Status2Reg) & 0x08)))
    {   
    status = MI_ERR;   
  }
    
    return status;
}

uchar MFRC522_Read(uchar blockAddr, uchar *recvData)
{
    uchar status;
    uint unLen;

    recvData[0] = PICC_READ;
    recvData[1] = blockAddr;
    CalulateCRC(recvData,2, &recvData[2]);
    status = MFRC522_ToCard(PCD_TRANSCEIVE, recvData, 4, recvData, &unLen);

    if ((status != MI_OK) || (unLen != 0x90))
    {
        status = MI_ERR;
    }
    
    return status;
}

uchar MFRC522_Write(uchar blockAddr, uchar *writeData)
{
    uchar status;
    uint recvBits;
    uchar i;
  uchar buff[18]; 
    
    buff[0] = PICC_WRITE;
    buff[1] = blockAddr;
    CalulateCRC(buff, 2, &buff[2]);
    status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &recvBits);

    if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
    {   
    status = MI_ERR;   
  }
        
    if (status == MI_OK)
    {
        for (i=0; i<16; i++)    // write 16Byte data into FIFO
        {    
          buff[i] = *(writeData+i);   
        }
        CalulateCRC(buff, 16, &buff[16]);
        status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 18, buff, &recvBits);
        
    if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
        {   
      status = MI_ERR;   
    }
    }
    
    return status;
}

void MFRC522_Halt(void)
{
  uchar status;
    uint unLen;
    uchar buff[4]; 

    buff[0] = PICC_HALT;
    buff[1] = 0;
    CalulateCRC(buff, 2, &buff[2]);
 
    status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff,&unLen);
}



void lora_send_time() {
  Serial.print("lora_time_send...");

  LoRa.beginPacket();
  LoRa.print(char(info[14]));
  LoRa.print(char(info[15]));
  LoRa.print(':');
  LoRa.print(char(info[16]));
  LoRa.print(char(info[17]));
  LoRa.print(':');
  LoRa.print(char(info[18]));
  LoRa.print(char(info[19]));
  LoRa.print('/');
  LoRa.endPacket();

  print_view(info, 14, 20);
  Serial.println("  Done");
}

void lora_send_uid() {
    
  Serial.print("lora_send_uid... ");

  LoRa.beginPacket();
  for( int j = 0; j < 5; ++j) {
    Serial.print(serNum[j], HEX);
    LoRa.print(serNum[j], HEX);
  }
  LoRa.print("/21.03.2019/");
  LoRa.endPacket();

  Serial.println("  Done");
}

void lora_send_location() {
  Serial.print("lora_send_location... ");


  LoRa.beginPacket();

  for (char i = 25; i < 35; ++i) {
    Serial.print((char)info[i]);
    LoRa.print((char)info[i]);
  }
  
  Serial.print(F(",N,"));
  LoRa.print(",N,");
  
  for( char i = 38; i < 49; ++i ) {
    Serial.print((char)info[i]);
    LoRa.print((char)info[i]);
  }
  
  Serial.print(",E  //  ");
  LoRa.print(",E");

  LoRa.print("\n");

  LoRa.endPacket();

  Serial.println("  Done");
}

void print_view(const char* buffer, byte begin, byte end) {
  for( byte i = begin; i < end; ++i ) {
    Serial.print(buffer[i]);
    Serial.print(' ');
  }
}