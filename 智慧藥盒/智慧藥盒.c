#include "HT66F2390.h"
#include "Typedef.h"
#include "ex1.h"

//AT���O
const unsigned char AT[8] = "AT\r\n";
const unsigned char crlf[4] = "\r\n";
const unsigned char cr[4] = "\r";
const unsigned char isok[6] = "\r\nOK\r\n";
const unsigned char iserr[9] = "\r\nERROR\r\n";
const unsigned char issend[8] = "\r\nOK\r\n> ";
const unsigned char isgotip[8] = "GOT IP\r\n";
const unsigned char connect_ap[64] = "AT+CWJAP=\"COMMMM\",\"8765432100\"\r\n";
const unsigned char open_tcp[48] = "AT+CIPSTART=\"TCP\",\"172.20.10.4\",8000\r\n";	//WIFI IP
const unsigned char close_tcp[24] = "AT+CIPCLOSE";
const unsigned char cipsend[24] = "AT+CIPSENDEX=256\r\n";
const unsigned char request_header[128] = "POST /reading HTTP/1.1\r\nHost: 172.20.10.4:8000\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 62";
const unsigned char backslashzero[3] = {0x5c, 0x30, 0x00};

volatile unsigned char RX_buf[16];
volatile  uint8_t FP_Num = 0;
#define true 1
#define false 0
#define TRUE 1
#define FALSE 0
volatile int xxx =0;
uint8_t commandByte[12];
uint8_t responseByte[12];

//����������
const unsigned char hr1[4] = "hr=";
const unsigned char min1[5] = "min=";
const unsigned char sec1[5] = "sec=";
const unsigned char temp1[6] = "temp=";
const unsigned char humm1[6] = "humm=";
volatile unsigned char str_v0[8];
volatile unsigned char str_v1[8];
volatile unsigned char str_v2[8];
volatile unsigned char str_v3[8];
volatile unsigned char str_v4[8];
volatile uint8_t data_ready = 0;
volatile uint8_t rx_index = 0;

// �s��P�P��Ū������� �ɶ� �ū׼ƾ�
u8 Data[8];
u8 Datasec[8] ;
u8 Datamin[8] ;
u8 Datahr[8] ;
char result[4];
char secASCII;
char minASCII;
char hrASCII;
u8 sec_bcd;
u8 sec;
u8 min_bcd;
u8 min;
u8 hr_bcd;
u8 hr;
u8 minset1 =33;
u8 minset =31;
u8 hrset =0;
float temp;
float humm;

// I2C
#define  SCLC  _pfc3
#define  SDAC  _pfc2
#define  SCL  _pf3
#define  SDA  _pf2
#define SDA_LOW()  (_pf2 = 0, _pfc2 = 0) 
#define SDA_OPEN() (_pfc2 = 1)           
#define SCL_LOW()  (_pf3 = 0, _pfc3 = 0) 
#define SCL_OPEN() (_pfc3 = 1)           
#define SDA_READ   (_pf2)     
#define SCL_READ   (_pf3)     
#define SCLPU _pfpu3
#define SDAPU _pfpu2
#define ACK 0
#define NAK 1
#define Mode_F 1 
#define NO_ERROR  0
#define ACK_ERROR 1
void I2c_StopCondition(void);
void I2c_StartCondition(void);
void Delayms(u16 del);
#define pause() GCC_DELAY(5) // ��?��?
void I2C_Init(void);

//��ɼҲթw�q
#define SDA_sec 0x00
#define SDA_min 0x01
#define SDA_hr  0x02
#define DeviceID_write 0b11010000
#define DeviceID_read  0b11010001
void init_uart0();
u8 Readsec(u8 len,volatile u8* pdat);
u8 Readmin(u8 len,volatile u8* pdat);
u8 Readhr(u8 len,volatile u8* pdat);
u8 Binary_to_BCD(u8 bin);
u8 Decimal_Add(u8 bcd);

//�ŷë׼Ҳթw�q
volatile u8 serialNumber[8]; 
volatile unsigned char data_msg[62];
volatile unsigned char* data_ptr;
void valtostr();
const unsigned char ampersand[4] = "&";
void setdatamsg(unsigned char *msg_ptr);
volatile u8 temper_f_h;
volatile u8 temper_f_l;
volatile unsigned int cnt,hum, cnt_0, cnt_1;
#define Function_Code 0x03
#define I2C_ADDR  (0x5c << 1)
#define SENSOR_ADDR_WRITE (0x5C << 1) 
#define SENSOR_ADDR_READ  ((0x5C << 1) | 0x01) 
u8 I2C_WriteByte(u8 txdata);
u8 I2C_ReadByte(u8 ack);
u8 readData(u8 cmd, u8 len, volatile u8* pdat);
u16 RH_Module_CheckSum(volatile u8 *Data, u8 length);
volatile u8 serialNumber[8]; 
volatile u8 temper_f_h;
volatile u8 temper_f_l;
volatile unsigned int cnt,hum, cnt_0, cnt_1;
char RX_wait(unsigned char arr[],unsigned char size);

//���A���F
void set_angle(u16 angle);
void pwmset(void);
const u16 PWM_TAB[] = {0,63,125,188,250,310};  //PWM
u8 soundTriggered = 0x00;
//���ﾹ
void pspk();
#define fSYS   8000000
const u16 Pitch_TAB[] = {            //���ձ`�ƫت��
 fSYS/(523*2*4), fSYS/(659*2*4), fSYS/(785*2*4),  
    fSYS/(523*4*4), fSYS/(785*2*4),fSYS/(659*2*4),  
    fSYS/(523*2*4) };



void main() {

    _wdtc = 0b10101111;  // �T�άݪ���
	init_uart1();
	u8 i,error;
    u16 crctemp, crcDat;
    SCLPU = 1;
    SDAPU = 1;
    I2C_Init(); // ��l�� I2C
    ChangeBaudRate(38400);
  	init_uart2();     
    Delayms(1000); 
     xxx = 0;
    Delayms(3000); 
    Open();
    Delayms(3000);
    GetEnrollCount();
    
     init_uart0();  // ��l�� UART
     memset(RX_buf,0,16);
     xxx = 0;
//    Delayms(3000); 
      sendcmd0(connect_ap);
    Delayms(10000); 

    // ��ܿ�ܼҦ�����l�T��
    Initial_display();
    while (1) {
    	  error = readData(0x00, 8, &Data[0]);
        if (error == NO_ERROR) {
            crcDat = RH_Module_CheckSum(&Data[0], 6);
            crctemp = (u16)Data[7] << 8 | Data[6];
            if (Data[0] == 0x03 && Data[1] == 0x04 && crcDat == crctemp) {
            GCC_DELAY(10);
            }
        if (Mode_F) {
        cnt=(Data[4]*256+Data[5]); //��C
        hum=(Data[2]*256+Data[3]);  //%RH
        temp=cnt/10;  //F=9*C/5+32
        humm=hum/10;
      
            }
            
        Readsec(8,&Datasec[0]);
        
        Readmin(8,&Datamin[0]);
     
        Readhr(8,&Datahr[0]);
      
        valtostr(hr, str_v0);  // �N�p���ഫ���r��
        valtostr(min, str_v1); // �N�����ഫ���r��
        valtostr(sec, str_v2); // �N���ഫ���r��
      
//      sendpostreq();
         
        Delayms(1000);
    if(min==minset && hr==hrset && soundTriggered == 0){
    pwmset();
     _ptm3al=(u8)PWM_TAB[1];  
     _ptm3ah=PWM_TAB[1]>>8;
     pspk();
     _pcc0=1;
     soundTriggered=1;  
    }
        }
        
    	uint8_t user_input = function(); 	
        switch (function()) {
            case 48:  // ���U�����Ҧ�
             xxx=0;
                Enroll_Mode();
                break;
            case 49:  // �����ѧO�Ҧ�
                Identify_Mode();
                break;
            case 50:  // �R�������Ҧ�
                Delete_Mode();
                break;
            case 51:  // ����w���U�����ƶq
            	
                GetCount_Mode();
                break;
           default:;
        }
    	}
        Delayms(500);
    
}


// ��l�T�����
void Initial_display(void) {
    sendcmd("==============================");
    Delayms(200);
    sendcmd("Demo Functions:");
    Delayms(200);
    sendcmd("(1) Fingerprint Enrollment");
    Delayms(200);
    sendcmd("(2) Fingerprint Identification");
    Delayms(200);
    sendcmd("(3) Delete ID");
    Delayms(200);
    sendcmd("(4) Get Enrolled ID Count");
    Delayms(200);
   	sendcmd("==============================");
   	Delayms(200);
    sendcmd("Initializing Fingerprint Sensor...");
    Delayms(200);
	Delayms(1000);
}

// �������U�Ҧ�
void Enroll_Mode() {
    xxx = 0;
    sendcmd("Start Fingerprint Enrollment");  // ��ܶ}�l���U����
    Delayms(2000);

    // ���Ĥ@�ӥ��ϥΪ� ID
    while (CheckEnrolled(FP_Num)) {
        FP_Num++;
        Delayms(500);
    }

    Delayms(1000);
    xxx = 0;
    EnrollStart(FP_Num);
    Delayms(1000);

    // ���ݤ�����U
    xxx = 0;
    while (!IsPressFinger()) Delayms(800);

    // �������Ϲ��ð�����U�y�{
    xxx = 0;
    if (CaptureFinger()) {
        Delayms(800);
        sendcmd("Remove finger");
        Delayms(2000);

        // ���� Enroll1 ���ˬd ACK
        if (Enroll1() == 0) { 
            Delayms(1000);
            while (IsPressFinger()) Delayms(500);  // ���ݤ�����}

            sendcmd("Press same finger again: 2nd time");
            Delayms(800);  // ���ܦA�����U
            while (!IsPressFinger()) Delayms(500);
            
            // �A������ð��� Enroll2
            if (CaptureFinger()) {
                Delayms(500);
                sendcmd("Remove finger");
                xxx = 0;
                Delayms(500);

                if (Enroll2() == 0) {
                    Delayms(600);
                    xxx = 0;
                    while (IsPressFinger()) Delayms(500);  // �A�����ݤ�����}
                    Delayms(500);

                    sendcmd("Press same finger again: 3rd time");  // ���ܦA�����U
                    Delayms(500);
                    xxx = 0;
                    while (!IsPressFinger()) Delayms(500);
                    xxx = 0;

                    // �̫�@������ð��� Enroll3
                    if (CaptureFinger()) {
                        Delayms(800);
                        sendcmd("OK");  // �̫�@�����ܤ�����}

                        if (Enroll3() == 0) {
                            Delayms(2000);  // ����έ��m
                            sendcmd("Fingerprint Enrolled Successfully!");
                            Delayms(100);
                            Initial_display();  // ���U���\���^�D���
                            xxx = 0;
                        } else {
                            sendcmd("Enroll Failed");  // ���U����
                        }
                    } else {
                        sendcmd("Failed to capture third finger");  // �����ĤT����������
                    }
                } else {
                    sendcmd("Failed to capture second finger");  // �����ĤG����������
                }
            }
        } else {
            sendcmd("Failed to capture first finger");  // �����Ĥ@����������
        }
    }
}
// �����ѧO�Ҧ�
void Identify_Mode() {
	xxx = 0;
    // 1. �����Τ�������
    sendcmd("Please press finger within 2 seconds");
    Delayms(1000);  // ���� 1 ��

    // 2. �ˬd����O�_����
    while (!IsPressFinger()) {
        sendcmd("Please press finger");
        Delayms(1000);  // �C�j 1 ���ˬd
    }

    // ����������\�᩵�� 1 ��
    Delayms(1000);  

    // 3. ��������
    while (!CaptureFinger() && responseByte[8] !=ACK) {
        sendcmd("Capturing failed, retrying...");
        Delayms(2000);  // �C�j 1 ����ծ���
    }

    sendcmd("Finger captured");
    Delayms(2000);  // ���������᩵�� 1 ��

    // 4. ���ѫ���
    while ((FP_Num = Identify1_N()) >= 100 && FP_Num != 0x100A && FP_Num != 0x1008) {
        sendcmd("Identifying failed, retrying...");
        Delayms(1000);  // �C�j 1 ����տ���
    }

    // 5. ��ܵ��G
    if (FP_Num < 100) {
        sendcmd("Verified ID:");
        char ascii_str[4];
        int_to_string(FP_Num, ascii_str);  // �N ID �ର�r��
        sendcmd(ascii_str);  // ����ѧO ID
    } else {
        if (FP_Num == 0x100A) {
            sendcmd("Database is empty, no ID is used.");
        } else if (FP_Num == 0x1008) {
            sendcmd("Finger not found");
        }
    }

    // 6. �^��D�e��
    Initial_display();  // ��ܪ�l�e��
    Delayms(1000);  // �����y�{�᪺����
}

// �R�������Ҧ�
void Delete_Mode() {
//    if (DeleteID_Fun == 1) {
//        // �R����@ID
//        BOOL deleteStatus = DeleteID(FP_Num);
//        if (deleteStatus == TRUE) {
//            sendcmd("Delete Successful!");
//        } else {
//            sendcmd("Delete Failed! ID not found.");
//        }
//    } else if (DeleteID_Fun == 2) {
//        // �R���Ҧ�����
//        BOOL deleteStatus = DeleteAll();
//        if (deleteStatus == TRUE) {
//            sendcmd("All fingerprints deleted.");
//        } else {
//           sendcmd("Database is already empty.");
//        }
//    }
    Initial_display();
}

// ����w���U�����ƶq
void GetCount_Mode() {
	xxx = 0;
    int count = GetEnrollCount();
    sendcmd("Total Enrolled IDs: ");
    sendcmd(count + '0');  // ��ܵ��U���ƶq
    sendcmd("OK");
    Initial_display();
}

DEFINE_ISR(UART1, 0x3C)
{
    // �B�z UART0 ���_
    if (_ur0f == 1) {
        u8 i;
	 for(i=0;i<16;i++)
	 {
	  RX_buf[i] = RX_buf[i+1];
	 } 
	 RX_buf[11] = _txr_rxr0;
	 _ur0f=0;

    }

    // �B�z UART1 ���_
    if (_ur1f == 1) {
        if (xxx >= 12) xxx = 0;    // �Y���޶W�L 12 �h�k�s
        RX_buf[xxx++] = _txr_rxr1; // �s�J UART1 �����쪺�ƾ�
        _ur1f = 0;                 // �M�� UART1 ���_�лx
    }
}


char function() {
    unsigned char num;
    num = RX_buf[xxx-1];  
    return num;  // ��^�����쪺�r��
}


char RX_wait(unsigned char arr[],unsigned char size)
{	u8 i;
	for(i=0;i<size;i++)
	{
		if(RX_buf[i+16-size]!=arr[i]) return 0;
	}
	memset(RX_buf,0,16);
	return 1;
}


BOOL debug = 1;
uint8_t responseByte[12];
uint8_t commandByte[12];
void init_uart0() {
   _pas1 = 0b11110000;  // TX0->PA7 RX0->PA6
    _u0cr1 = 0b10000000;  // UARTEN0/8-Bit/No_parity/1 Stop Bit
    _u0cr2 = 0b11100100;  // TXEN0/BRGH0
    _brg0 = fH / ((u32)16 * BR) - 1;  
    _mf5e = 1;
    _ur0e = 1;
    _pac5 = 0;
    _pa5 = 0;
    _emi = 1;
    _hircc = 0b00000011;
}



void init_uart1() {
   _pds0 = 0b00101000;  // TX0->PF7 RX0->PF6
    _u1cr1 = 0b10000000;  // UARTEN0/8-Bit/No_parity/1 Stop Bit
    _u1cr2 = 0b11100100;  // TXEN0/BRGH0
    _brg1 = 12000000 / ((u32)16 * 57600) - 1; 
    _mf5e = 1;
    _ur1e = 1;
    _emi = 1;
    _hircc = 0b00000111;
}

void init_uart2() {
   _pds0 = 0b00101000;  // TX0->PA7 RX0->PA6
    _u1cr1 = 0b10000000;  // UARTEN0/8-Bit/No_parity/1 Stop Bit
    _u1cr2 = 0b11100100;  // TXEN0/BRGH0
    _brg1 = fH / ((u32)16 * BR) - 1;  // ?�m�i�S�v
    _mf5e = 1;
    _ur1e = 1;
    _emi = 1;
    _hircc = 0b00000011;
}






// ���o�Ҳզ^���ƾ�
void GetResponse(uint8_t *response, uint8_t length) {
    memset(response, 0, length);  // �M�� response �w�İ�
    uint8_t i;
    for (i = 0; i < length; i++) {
        response[i] = RX_buf[i];  // �N�����쪺�ƾڦs�J response
    }
}
// ���L�R�O�ƾڡ]�ȭ� Debug�^

// �o�e�ƾکR�O
void sendbytes(unsigned char *cmd_ptr, uint8_t length) {
uint8_t i;
    for (i = 0; i < length; i++) {
        while (!_txif1);  // ����?�e�H�s����
        _txr_rxr1 = cmd_ptr[i];  // ?�e?�e�r?
    }
}

void sendcmd(unsigned char *cmd_ptr)
{
	while(*cmd_ptr){
		while(!_txif1);	
		_txr_rxr1 = *cmd_ptr;
		cmd_ptr++;
	}
}
	
void sendcmd0(unsigned char *cmd_ptr)
{
	while(*cmd_ptr){
		while(!_txif0);	
		_txr_rxr0 = *cmd_ptr;
		cmd_ptr++;
	}
}



// �Ҳժ�l�ơ]Open�^
uint16_t Open() {
    commandByte[0] = 0x55;
  commandByte[1] = 0xAA;
  commandByte[2] = 0x01;
  commandByte[3] = 0;
  commandByte[4] = 0;
  commandByte[5] = 0;
  commandByte[6] = 0;
  commandByte[7] = 0;  
  commandByte[8] = 0x01;
  commandByte[9] = 0;
  commandByte[10] = 0x01;
  commandByte[11] = 0x01;

    sendbytes(commandByte, 12);
    Delayms(200);
    GetResponse(responseByte, 12);

    return (responseByte[8] == ACK) ? ACK : NACK_COMM_ERR;
}

// �Ҳ������]Close�^
uint16_t Close() {
    commandByte[8] = 0x02;  // Close command
    commandByte[10] = 0x02;

    sendbytes(commandByte, 12);
    Delayms(10);
    GetResponse(responseByte, 12);

    return (responseByte[8] == ACK) ? ACK : NACK_COMM_ERR;
}


// ����w���U�����ƶq
uint16_t GetEnrollCount() {
   commandByte[0] = 0x55;
  commandByte[1] = 0xAA;
  commandByte[2] = 0x01;
  commandByte[3] = 0;
  commandByte[4] = 0;
  commandByte[5] = 0;
  commandByte[6] = 0;
  commandByte[7] = 0;  
  commandByte[8] = 0x20;
  commandByte[9] = 0;
  commandByte[10] = 0x20;
  commandByte[11] = 0x01;

    sendbytes(commandByte, 12);
    Delayms(10);
    GetResponse(responseByte, 12);

  if (responseByte[8] == 0x30)return responseByte[4];
  else return NACK_COMM_ERR;
}

// �ˬd ID �O�_�w���U
BOOL CheckEnrolled(uint8_t FP_Num ) {
 uint8_t i;
uint16_t sum = 0;

// �c�y���O�ƾ�
 uint8_t commandByte[12]={0x55,0xAA,0x01,0x00,FP_Num ,0,0,0,0x21,0};

// �p�����M
for (i = 0; i < 10; i++) {
    sum += commandByte[i];
}
commandByte[10] = (uint8_t)(sum & 0xFF);  // �C8��
commandByte[11] = (uint8_t)((sum >> 8) & 0xFF);  // ��8��

    // �o�e�R�O�������Ҳ�
    sendbytes(commandByte, 12);
    Delayms(10);

    // ����Ҳզ^��
    GetResponse(responseByte, 12);
  
    // �ˬd�^���O�_��ACK�A����w���U
  if (responseByte[8] == 0x30)return true;
  else return false;
}

// �}�l���U�L�{
uint16_t EnrollStart(uint8_t FP_Num ) {
	uint8_t i;
  uint16_t sum = 0;
   commandByte[0] = 0x55;
  commandByte[1] = 0xAA;
  commandByte[2] = 0x01;
  commandByte[3] = 0;
  commandByte[4] = FP_Num;
  commandByte[5] = 0;
  commandByte[6] = 0;
  commandByte[7] = 0;  
  commandByte[8] = 0x22;
  commandByte[9] = 0;
  for (i=0; i<10; i++)  {sum += commandByte[i];}   // Calculate checksum
  commandByte[10] = (uint8_t)sum&0xFF;
  commandByte[11] = (uint8_t)(sum>>8)&0xFF;
  
  sendbytes(commandByte, 12);
   Delayms(10);
    GetResponse(responseByte, 12);
    
   if (responseByte[8] == ACK)return 0;
}

// ���� Enroll1
uint16_t Enroll1() {
  commandByte[0] = 0x55;
  commandByte[1] = 0xAA;
  commandByte[2] = 0x01;
  commandByte[3] = 0;
  commandByte[4] = 0;
  commandByte[5] = 0;
  commandByte[6] = 0;
  commandByte[7] = 0;  
  commandByte[8] = 0x023;
  commandByte[9] = 0;
  commandByte[10] = 0x23;
  commandByte[11] = 0x01;
    sendbytes(commandByte, 12);
    Delayms(10);
    GetResponse(responseByte, 12);

    if (responseByte[8] == ACK) return 0;
    if (responseByte[4] == 0x0D) return 1;  // Enroll Failed
    if (responseByte[4] == 0x0C) return 2;  // Bad finger
    return 3;  // ID already used
}

// ���� Enroll2
uint16_t Enroll2() {
    commandByte[0] = 0x55;
  commandByte[1] = 0xAA;
  commandByte[2] = 0x01;
  commandByte[3] = 0;
  commandByte[4] = 0;
  commandByte[5] = 0;
  commandByte[6] = 0;
  commandByte[7] = 0;  
  commandByte[8] = 0x24;
  commandByte[9] = 0;
  commandByte[10] = 0x24;
  commandByte[11] = 0x01;

    sendbytes(commandByte, 12);
    Delayms(10);
    GetResponse(responseByte, 12);

    if (responseByte[8] == ACK) return 0;
    if (responseByte[4] == 0x0D) return 1;  // Enroll Failed
    if (responseByte[4] == 0x0C) return 2;  // Bad finger
    return 3;  // ID already used
}

// ���� Enroll3
uint16_t Enroll3() {
   commandByte[0] = 0x55;
  commandByte[1] = 0xAA;
  commandByte[2] = 0x01;
  commandByte[3] = 0;
  commandByte[4] = 0;
  commandByte[5] = 0;
  commandByte[6] = 0;
  commandByte[7] = 0;  
  commandByte[8] = 0x25;
  commandByte[9] = 0;
  commandByte[10] = 0x25;
  commandByte[11] = 0x01;
    sendbytes(commandByte, 12);
    Delayms(10);
    GetResponse(responseByte, 12);

    if (responseByte[8] == ACK) return 0;
    if (responseByte[4] == 0x0D) return 1;  // Enroll Failed
    if (responseByte[4] == 0x0C) return 2;  // Bad finger
    return 3;  // ID already used
}

//�ˬd�O�_�������
BOOL IsPressFinger() {
  commandByte[0] = 0x55;
  commandByte[1] = 0xAA;
  commandByte[2] = 0x01;
  commandByte[3] = 0;
  commandByte[4] = 0;
  commandByte[5] = 0;
  commandByte[6] = 0;
  commandByte[7] = 0;  
  commandByte[8] = 0x26;
  commandByte[9] = 0;
  commandByte[10] = 0x26;
  commandByte[11] = 0x01;

    sendbytes(commandByte, 12);
    Delayms(10);
    GetResponse(responseByte, 12);

     if (responseByte[8] == 0x30)return true;
  else return false;
}

// �R�����w ID
BOOL DeleteID(uint16_t id) {
    commandByte[4] = id & 0xFF;
    commandByte[8] = 0x40;  // DeleteID command
    commandByte[10] = 0x40;

    sendbytes(commandByte, 12);
    Delayms(10);
    GetResponse(responseByte, 12);

    return (responseByte[8] == ACK) ? true : false;
}

// �R���Ҧ�����
BOOL DeleteAll() {
    commandByte[8] = 0x41;  // DeleteAll command
    commandByte[10] = 0x41;

    sendbytes(commandByte, 12);
    Delayms(10);
    GetResponse(responseByte, 12);

    return (responseByte[8] == ACK) ? true : false;
}

// ���Ҥ���P���w ID �O�_�ǰt
uint16_t Verify1_1(uint16_t id) {
    commandByte[4] = id & 0xFF;
    commandByte[8] = 0x50;  // Verify1_1 command
    commandByte[10] = 0x50;

    sendbytes(commandByte, 12);
    Delayms(10);
    GetResponse(responseByte, 12);

    if (responseByte[8] == ACK) return 0;
    if (responseByte[4] == 0x03) return 1;  // Invalid Position
    if (responseByte[4] == 0x04) return 2;  // ID not used
    return 3;  // Verified false
}

// 1:N �������
uint16_t Identify1_N() {
   commandByte[0] = 0x55;
  commandByte[1] = 0xAA;
  commandByte[2] = 0x01;
  commandByte[3] = 0;
  commandByte[4] = 0;
  commandByte[5] = 0;
  commandByte[6] = 0;
  commandByte[7] = 0;  
  commandByte[8] = 0x051;
  commandByte[9] = 0;
  commandByte[10] = 0x51;
  commandByte[11] = 0x01;

    sendbytes(commandByte, 12);
    Delayms(1000);
    GetResponse(responseByte, 12);

    if (responseByte[8] == ACK) return responseByte[4];  // ��^�ѧO��ID
    if (responseByte[4] == 0x0A) return 0x100A;  // Database is empty
    if (responseByte[4] == 0x08) return 0x1008;  // Identify failed
    return NACK_COMM_ERR;
}

// ��������

void Delayms(u16 del) {
	u16 i;
    for ( i = 0; i < del; i++) {
        GCC_DELAY(2000);  
    }
}


BOOL ChangeBaudRate(unsigned long baud) {
    uint8_t i;
    uint16_t sum = 0;
    
    if ((baud == 9600) || (baud == 19200) || (baud == 38400) || (baud == 57600) || (baud == 115200)) {
        commandByte[0] = 0x55;
        commandByte[1] = 0xAA;
        commandByte[2] = 0x01;
        commandByte[3] = 0x00;
        commandByte[4] = (baud & 0x000000ff);
        commandByte[5] = (baud & 0x0000ff00) >> 8;
        commandByte[6] = (baud & 0x00ff0000) >> 16;
        commandByte[7] = (baud & 0xff000000) >> 24;  
        commandByte[8] = 0x04;
        commandByte[9] = 0x00;


        for (i = 0; i < 10; i++) {
            sum += commandByte[i];
        }
        commandByte[10] =  sum & 0xFF;
        commandByte[11] =  (sum >> 8) & 0xFF;


        sendbytes(commandByte, 12);
        Delayms(10);

        GetResponse(responseByte, 12);
        Delayms(10);

        if (responseByte[8] == 0x30) {
            return true;
        }
    }
    return false;
}


BOOL CaptureFinger() {
   commandByte[0] = 0x55;
  commandByte[1] = 0xAA;
  commandByte[2] = 0x01;
  commandByte[3] = 0;
  commandByte[4] = 0;
  commandByte[5] = 0;
  commandByte[6] = 0;
  commandByte[7] = 0;  
  commandByte[8] = 0x60;
  commandByte[9] = 0;
  commandByte[10] = 0x60;
  commandByte[11] = 0x01;

    sendbytes(commandByte, 12);
    Delayms(100);
    GetResponse(responseByte, 12);

    if (responseByte[8] == 0x30) return true;
  else return false;
}

void int_to_string(uint8_t FP_Num, char *ascii_str) {
   int i = 0;
    char temp[4];  // ���] FP_Num ���W�L 3 ��Ʀr
    if (FP_Num == 0) {
        ascii_str[i++] = '0';  // �S��B�z 0
    } else {
        while (FP_Num > 0) {
            temp[i++] = (FP_Num % 10) + '0';  // �N�C�@���ഫ�������� ASCII �r��
            FP_Num /= 10;
        }
    }

    // �N�r�Ŷ��Ǥ���
    int j;
    for (j = 0; j < i; j++) {
        ascii_str[j] = temp[i - j - 1];
    }
    ascii_str[i] = '\0';  // ������
}


void pwmset(void){
  _pb2=0; _pbc2=0;
  _pbs0=0x30;
  _ptm3rpl=(u8)2500; _ptm3rph=2500>>8; 
  _ptm3c0=0b00111000;       
  _ptm3c1=0b10101000;       
 }
 
 void pspk(void){
 u8 counter = 0;
 u8 maxcount = 3;
 _pdc6=0;                   //�W��pSPK����X�ݩ�
 _pcc0=0;
 _sledc1=0xc0;         //�]�mPD[6:4]���y��Level 3
 _stm2c0=0; _stm2c1=0b11000001;                  //STM2��������Ȧs���W��
 _stm2ae=1; _mf4e=1; _emi=1;      //�P��T2A���_,EMI�`�}��
 while(counter < maxcount){
  _stm2al=Pitch_TAB[0];             //���o�p�Ʈɶ��`��     
  _stm2ah=Pitch_TAB[0]>>8;
  _st2on=1; Delayms(400);      //�Ұ�STM2�p�ƨé���400mS 
  _st2on=0;          //����STM2�p��
  _stm2al=Pitch_TAB[1];             //���o�p�Ʈɶ��`��     
  _stm2ah=Pitch_TAB[1]>>8;
  _st2on=1; Delayms(400);      //�Ұ�STM2�p�ƨé���400mS 
  _st2on=0;          //����STM2�p��
  _stm2al=Pitch_TAB[2];             //���o�p�Ʈɶ��`��     
  _stm2ah=Pitch_TAB[2]>>8;
  _st2on=1; Delayms(400);      //�Ұ�STM2�p�ƨé���400mS 
  _st2on=0;          //����STM2�p��  
  _stm2al=Pitch_TAB[3];             //���o�p�Ʈɶ��`��     
  _stm2ah=Pitch_TAB[3]>>8;
  _st2on=1; Delayms(400);      //�Ұ�STM2�p�ƨé���400mS 
  _st2on=0;          //����STM2�p��
  _stm2al=Pitch_TAB[4];             //���o�p�Ʈɶ��`��     
  _stm2ah=Pitch_TAB[4]>>8;
  _st2on=1; Delayms(400);      //�Ұ�STM2�p�ƨé���400mS 
  _st2on=0;          //����STM2�p��
  _stm2al=Pitch_TAB[5];             //���o�p�Ʈɶ��`��     
  _stm2ah=Pitch_TAB[5]>>8;
  _st2on=1; Delayms(400);      //�Ұ�STM2�p�ƨé���400mS 
  _st2on=0;          //����STM2�p��
  _stm2al=Pitch_TAB[6];             //���o�p�Ʈɶ��`��     
  _stm2ah=Pitch_TAB[6]>>8;
  _st2on=1; Delayms(400);      //�Ұ�STM2�p�ƨé���400mS 
  _st2on=0;          //����STM2�p��  
  _stm2al=Pitch_TAB[7];             //���o�p�Ʈɶ��`��     
  _stm2ah=Pitch_TAB[7]>>8;
  _st2on=1; Delayms(400);      //�Ұ�STM2�p�ƨé���400mS 
  _st2on=0;          //����STM2�p��
  counter++;
 }
 }
 
 DEFINE_ISR(ISR_STM2,0x38)
{ _pd6=!_pd6;             //pSPK���A�ϦV�@��
 _stm2af=0;          //�M��STM2AF
}
 
void I2C_Init(void) {
    SDA_OPEN();
    SCL_OPEN(); 
}

void valtostr()
{   u8 i,k;
  unsigned char *ptr,*buf_ptr;
  unsigned char buff[8];
  u16 buf;
  memset(str_v0,0,8);
  memset(str_v1,0,8);
  memset(str_v2,0,8);
  memset(str_v3,0,8);
  memset(str_v4,0,8);
  memset(buff,0,8);
  
  ptr = &buff[7];
  buf = (u16)sec;
   for(i=0;i<8;i++)
   { if(buf==0)
    {*ptr = 0;}
    else
    {*ptr = (buf%10)+48; buf/=10;}
    ptr--;
   }
  buf_ptr = &buff[0];
  ptr = &str_v0[0];
   for(i=0;i<8;i++)
   { if(*buf_ptr>=48 && *buf_ptr<=57)
    {*ptr = *buf_ptr;ptr++;}
    buf_ptr++;
   }
  memset(buff,0,8);
  
  ptr = &buff[7];
  buf = (u16)min;
   for(i=0;i<8;i++)
   { if(buf==0)
    {*ptr = 0;}
    else
    {*ptr = (buf%10)+48; buf/=10;}
    ptr--;
   }
  buf_ptr = &buff[0];
  ptr = &str_v1[0];
   for(i=0;i<8;i++)
   { if(*buf_ptr>=48 && *buf_ptr<=57)
    {*ptr = *buf_ptr;ptr++;}
    buf_ptr++;
   }
  memset(buff,0,8);
  
  ptr = &buff[7];
  buf = (u16)hr;
   for(i=0;i<8;i++)
   { if(buf==0)
    {*ptr = 0;}
    else
    {*ptr = (buf%10)+48; buf/=10;}
    ptr--;
   }
  buf_ptr = &buff[0];
  ptr = &str_v2[0];
   for(i=0;i<8;i++)
   { if(*buf_ptr>=48 && *buf_ptr<=57)
    {*ptr = *buf_ptr;ptr++;}
    buf_ptr++;
   }
    memset(buff,0,8);
    
  ptr = &buff[7];
  buf = (u16)temp;
   for(i=0;i<8;i++)
   { if(buf==0)
    {*ptr = 0;}
    else
    {*ptr = (buf%10)+48; buf/=10;}
    ptr--;
   }
  buf_ptr = &buff[0];
  ptr = &str_v3[0];
   for(i=0;i<8;i++)
   { if(*buf_ptr>=48 && *buf_ptr<=57)
    {*ptr = *buf_ptr;ptr++;}
    buf_ptr++;
   }
    memset(buff,0,8);
    
  ptr = &buff[7];
  buf = (u16)humm;
   for(i=0;i<8;i++)
   { if(buf==0)
    {*ptr = 0;}
    else
    {*ptr = (buf%10)+48; buf/=10;}
    ptr--;
   }
  buf_ptr = &buff[0];
  ptr = &str_v4[0];
   for(i=0;i<8;i++)
   { if(*buf_ptr>=48 && *buf_ptr<=57)
    {*ptr = *buf_ptr;ptr++;}
    buf_ptr++;
   }
}

void I2c_StartCondition(void) 
{ 
 SDA_OPEN(); 
 GCC_DELAY(2);
 SCL_OPEN(); 
 GCC_DELAY(2);
 
 SDA_LOW(); 
 GCC_DELAY(10);
 SCL_LOW(); 
 GCC_DELAY(10); 
}

void I2c_StopCondition(void) 
{ 
 SCL_LOW(); 
 GCC_DELAY(2);
 SDA_LOW(); 
 GCC_DELAY(2);
 SCL_OPEN(); 
 GCC_DELAY(10); 
 SDA_OPEN(); 
 GCC_DELAY(10);
}

//��ɼҲհƵ{��
u8 Binary_to_BCD(u8 bin) {
    u8 bcd;
    bcd = ((bin / 10) << 4) | (bin % 10);  
    return bcd;
}

u8 BCD_to_Binary(u8 bcd) {
    u8 bin;
    bin = ((bcd >> 4) * 10) + (bcd & 0x0F); 
    return bin;
}

u8 Decimal_Add(u8 bcd) {
    u8 bin = BCD_to_Binary(bcd); 
    bin += 1;  // �[1
    return Binary_to_BCD(bin);  
}

u8 Readsec(u8 len,volatile u8* data) {
    u8 error = NO_ERROR;
    u8 i;
    I2c_StartCondition();
    error = I2C_WriteByte(DeviceID_write);
    if (error == NO_ERROR) {
        error = I2C_WriteByte(SDA_sec);
        I2c_StopCondition();
        GCC_DELAY(200); 
    }
    if (error == NO_ERROR) {
        I2c_StartCondition();
        error = I2C_WriteByte(DeviceID_read);
        GCC_DELAY(10);
    }
    if (error == NO_ERROR) {
        for (i = 0; i < len - 2; i++) {
            *data = I2C_ReadByte(ACK);
            data++;
        }
        *data = I2C_ReadByte(NAK);
    
    sec_bcd = Datasec[0];
    sec = BCD_to_Binary(sec_bcd);  
    GCC_DELAY(100);
    I2c_StopCondition();
    Delayms(10);
 }return error;
}
u8 Readmin(u8 len,volatile u8* data) {
    u8 error = NO_ERROR;
    u8 i;
    I2c_StartCondition();
    error = I2C_WriteByte(DeviceID_write);
    if (error == NO_ERROR) {
        error = I2C_WriteByte(SDA_min);
        I2c_StopCondition();
        GCC_DELAY(200); 
    }
    if (error == NO_ERROR) {

        I2c_StartCondition();

        error = I2C_WriteByte(DeviceID_read);
        GCC_DELAY(10);
    }
    if (error == NO_ERROR) {
        for (i = 0; i < len - 2; i++) {
            *data = I2C_ReadByte(ACK);
            data++;
        }
        *data = I2C_ReadByte(NAK);
    }

    min_bcd = Datamin[0];
    min = BCD_to_Binary(min_bcd); 
    GCC_DELAY(100);
    I2c_StopCondition();
    Delayms(10);
    
    return error;
}
u8 Readhr(u8 len,volatile u8* data) {
    u8 error = NO_ERROR;
    u8 i;
    I2c_StartCondition();
    error = I2C_WriteByte(DeviceID_write);
    if (error == NO_ERROR) {
        error = I2C_WriteByte(SDA_hr);
        I2c_StopCondition();
        GCC_DELAY(200); 
    }
    if (error == NO_ERROR) {
        I2c_StartCondition();


        error = I2C_WriteByte(DeviceID_read);
        GCC_DELAY(10);
    }
    if (error == NO_ERROR) {
        for (i = 0; i < len - 2; i++) {
            *data = I2C_ReadByte(ACK);
            data++;
        }
        *data = I2C_ReadByte(NAK);
    }

    hr_bcd = Datahr[0];
    hr = BCD_to_Binary(hr_bcd);  
    GCC_DELAY(100);
    I2c_StopCondition();
    Delayms(10);
    return error;
}

u8 I2C_WriteByte(u8 txdata) {
    u8 i;
    u8 error = NO_ERROR;
    SCL_LOW();
    for (i = 0; i < 8; i++) {
        if (txdata & 0x80) {
            SDA_OPEN(); 
        } else {
            SDA_LOW(); 
        }
        GCC_DELAY(4);
        SCL_OPEN();
        GCC_DELAY(4);
        SCL_LOW();
        txdata = txdata << 1;
    }

    GCC_DELAY(4);
    SDA_OPEN(); 
    SCL_OPEN(); 
    GCC_DELAY(10);

    if (SDA_READ == 1) {
        error = ACK_ERROR;
    }
 
    GCC_DELAY(10);
    SCL_LOW(); 
    GCC_DELAY(50);
    
    return error;
}

u8 I2C_ReadByte(u8 ack) {
    u8 rxdata = 0;
    u8 i;
    SDA_OPEN();
    GCC_DELAY(20); 
 for(i = 0; i < 8; i++)
 {
  SCL_OPEN(); 
  GCC_DELAY(6); 
 
   rxdata = rxdata << 1;
  
   // read bit 
  if(SDA_READ == 1) 
  { 
      rxdata = rxdata | 0x01; 
  } 
  SCL_LOW(); 
  GCC_DELAY(6);    
 } 

    if (ack == ACK) {
        SDA_LOW(); 
    } else {
        SDA_OPEN();   
    }
    GCC_DELAY(50); 

    SCL_OPEN(); 
    GCC_DELAY(10); 
    SCL_LOW(); 
    SDA_OPEN(); 
    GCC_DELAY(50); 

    return rxdata;
}

u8 readData(u8 cmd, u8 length, volatile u8* data) {
    u8 error = NO_ERROR;
    u8 i;

    I2c_StartCondition();

    error = I2C_WriteByte(I2C_ADDR);

    if (error == NO_ERROR) {
        error = I2C_WriteByte(Function_Code);
        error = I2C_WriteByte(cmd);
        error = I2C_WriteByte(length - 4);
        I2c_StopCondition();
        GCC_DELAY(2000);  
    }

    if (error == NO_ERROR) {
        I2c_StartCondition();

        error = I2C_WriteByte(I2C_ADDR | 0x01);
        GCC_DELAY(10);
    }

    if (error == NO_ERROR) {
        for (i = 0; i < length - 1; i++) {
            *data = I2C_ReadByte(ACK);
            data++;
        }
        *data = I2C_ReadByte(NAK);
    }

    I2c_StopCondition();

    return error;
}

u16 RH_Module_CheckSum(volatile u8 *Data, u8 length) {
    u16 a_temp_crc = 0xFFFF;
    u8 i;
    while (length--) {
        a_temp_crc ^= *Data++;
        for (i = 0; i < 8; i++) {
            if (a_temp_crc & 0x01) 
            {
                a_temp_crc >>= 1;
                a_temp_crc ^= 0xA001;
            } else {
                a_temp_crc >>= 1;
            }
        }
    }
    return a_temp_crc;
}

void sendpostreq() {
    // �}�l�o�e�ƾ�
    sendcmd(open_tcp);  // ���} TCP �s��
    do 
 { _pg0=~_pg0;
  Delayms(100);
  if(RX_wait(iserr,9)) break;
 } while(!RX_wait(isok,6));      //����tcp�}��
 _pg0=1;
 sendcmd(cipsend);
 do 
 { _pg0=~_pg0;
  Delayms(100);
  if(RX_wait(iserr,9)) break;
 } while(!RX_wait(issend,8));     //���ݶǰe���O
 _pg0=1;
    sendcmd(request_header);  // �e�X HTTP POST �ШD�Y
    sendcmd(crlf);
    sendcmd(crlf);
    
 data_ptr = &data_msg[0];
 
    // �]�m�ƾڨõo�e
    setdatamsg(hr1);
    setdatamsg(str_v2);        // �o�e�p�ɼƾ�
    setdatamsg(ampersand);     // �o�e '&'
    setdatamsg(min1);
    setdatamsg(str_v1);        // �o�e�p�ɼƾ�
    setdatamsg(ampersand);     // �o�e '&'
    setdatamsg(sec1);
    setdatamsg(str_v0);        // �o�e�p�ɼƾ�
    setdatamsg(ampersand);     // �o�e '&'
    setdatamsg(temp1);
    setdatamsg(str_v3);        // �o�e�ū�
    setdatamsg(ampersand);     // �o�e '&'
    setdatamsg(humm1);
    setdatamsg(str_v4);        // �o�e���
 
    // �o�e�̲׼ƾڨç���
    
    sendcmd(data_msg);
    sendcmd(crlf);
    sendcmd(backslashzero);
    
    // �M�� data_msg �w�İ�
    
    memset(data_msg, 0x20, 62);
}

void setdatamsg(unsigned char *msg_ptr){
 while(*msg_ptr){
  *data_ptr = *msg_ptr;
  msg_ptr++;
  data_ptr++; 
 }
}