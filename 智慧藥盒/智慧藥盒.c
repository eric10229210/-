#include "HT66F2390.h"
#include "Typedef.h"
#include "ex1.h"

//AT指令
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

//網頁獲取資料
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

// 存放感感器讀取的濕度 時間 溫度數據
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
#define pause() GCC_DELAY(5) // 延?函?
void I2C_Init(void);

//實時模組定義
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

//溫溼度模組定義
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

//伺服馬達
void set_angle(u16 angle);
void pwmset(void);
const u16 PWM_TAB[] = {0,63,125,188,250,310};  //PWM
u8 soundTriggered = 0x00;
//蜂鳴器
void pspk();
#define fSYS   8000000
const u16 Pitch_TAB[] = {            //音調常數建表區
 fSYS/(523*2*4), fSYS/(659*2*4), fSYS/(785*2*4),  
    fSYS/(523*4*4), fSYS/(785*2*4),fSYS/(659*2*4),  
    fSYS/(523*2*4) };



void main() {

    _wdtc = 0b10101111;  // 禁用看門狗
	init_uart1();
	u8 i,error;
    u16 crctemp, crcDat;
    SCLPU = 1;
    SDAPU = 1;
    I2C_Init(); // 初始化 I2C
    ChangeBaudRate(38400);
  	init_uart2();     
    Delayms(1000); 
     xxx = 0;
    Delayms(3000); 
    Open();
    Delayms(3000);
    GetEnrollCount();
    
     init_uart0();  // 初始化 UART
     memset(RX_buf,0,16);
     xxx = 0;
//    Delayms(3000); 
      sendcmd0(connect_ap);
    Delayms(10000); 

    // 顯示選擇模式的初始訊息
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
        cnt=(Data[4]*256+Data[5]); //度C
        hum=(Data[2]*256+Data[3]);  //%RH
        temp=cnt/10;  //F=9*C/5+32
        humm=hum/10;
      
            }
            
        Readsec(8,&Datasec[0]);
        
        Readmin(8,&Datamin[0]);
     
        Readhr(8,&Datahr[0]);
      
        valtostr(hr, str_v0);  // 將小時轉換為字串
        valtostr(min, str_v1); // 將分鐘轉換為字串
        valtostr(sec, str_v2); // 將秒轉換為字串
      
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
            case 48:  // 註冊指紋模式
             xxx=0;
                Enroll_Mode();
                break;
            case 49:  // 指紋識別模式
                Identify_Mode();
                break;
            case 50:  // 刪除指紋模式
                Delete_Mode();
                break;
            case 51:  // 獲取已註冊指紋數量
            	
                GetCount_Mode();
                break;
           default:;
        }
    	}
        Delayms(500);
    
}


// 初始訊息顯示
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

// 指紋註冊模式
void Enroll_Mode() {
    xxx = 0;
    sendcmd("Start Fingerprint Enrollment");  // 顯示開始註冊提示
    Delayms(2000);

    // 找到第一個未使用的 ID
    while (CheckEnrolled(FP_Num)) {
        FP_Num++;
        Delayms(500);
    }

    Delayms(1000);
    xxx = 0;
    EnrollStart(FP_Num);
    Delayms(1000);

    // 等待手指按下
    xxx = 0;
    while (!IsPressFinger()) Delayms(800);

    // 捕獲手指圖像並執行註冊流程
    xxx = 0;
    if (CaptureFinger()) {
        Delayms(800);
        sendcmd("Remove finger");
        Delayms(2000);

        // 執行 Enroll1 並檢查 ACK
        if (Enroll1() == 0) { 
            Delayms(1000);
            while (IsPressFinger()) Delayms(500);  // 等待手指移開

            sendcmd("Press same finger again: 2nd time");
            Delayms(800);  // 提示再次按下
            while (!IsPressFinger()) Delayms(500);
            
            // 再次捕獲並執行 Enroll2
            if (CaptureFinger()) {
                Delayms(500);
                sendcmd("Remove finger");
                xxx = 0;
                Delayms(500);

                if (Enroll2() == 0) {
                    Delayms(600);
                    xxx = 0;
                    while (IsPressFinger()) Delayms(500);  // 再次等待手指移開
                    Delayms(500);

                    sendcmd("Press same finger again: 3rd time");  // 提示再次按下
                    Delayms(500);
                    xxx = 0;
                    while (!IsPressFinger()) Delayms(500);
                    xxx = 0;

                    // 最後一次捕獲並執行 Enroll3
                    if (CaptureFinger()) {
                        Delayms(800);
                        sendcmd("OK");  // 最後一次提示手指移開

                        if (Enroll3() == 0) {
                            Delayms(2000);  // 延遲或重置
                            sendcmd("Fingerprint Enrolled Successfully!");
                            Delayms(100);
                            Initial_display();  // 註冊成功後返回主選單
                            xxx = 0;
                        } else {
                            sendcmd("Enroll Failed");  // 註冊失敗
                        }
                    } else {
                        sendcmd("Failed to capture third finger");  // 捕捉第三次指紋失敗
                    }
                } else {
                    sendcmd("Failed to capture second finger");  // 捕捉第二次指紋失敗
                }
            }
        } else {
            sendcmd("Failed to capture first finger");  // 捕捉第一次指紋失敗
        }
    }
}
// 指紋識別模式
void Identify_Mode() {
	xxx = 0;
    // 1. 提醒用戶按壓手指
    sendcmd("Please press finger within 2 seconds");
    Delayms(1000);  // 等待 1 秒

    // 2. 檢查手指是否按壓
    while (!IsPressFinger()) {
        sendcmd("Please press finger");
        Delayms(1000);  // 每隔 1 秒檢查
    }

    // 捕捉手指成功後延遲 1 秒
    Delayms(1000);  

    // 3. 捕捉指紋
    while (!CaptureFinger() && responseByte[8] !=ACK) {
        sendcmd("Capturing failed, retrying...");
        Delayms(2000);  // 每隔 1 秒嘗試捕捉
    }

    sendcmd("Finger captured");
    Delayms(2000);  // 捕捉完成後延遲 1 秒

    // 4. 辨識指紋
    while ((FP_Num = Identify1_N()) >= 100 && FP_Num != 0x100A && FP_Num != 0x1008) {
        sendcmd("Identifying failed, retrying...");
        Delayms(1000);  // 每隔 1 秒嘗試辨識
    }

    // 5. 顯示結果
    if (FP_Num < 100) {
        sendcmd("Verified ID:");
        char ascii_str[4];
        int_to_string(FP_Num, ascii_str);  // 將 ID 轉為字串
        sendcmd(ascii_str);  // 顯示識別 ID
    } else {
        if (FP_Num == 0x100A) {
            sendcmd("Database is empty, no ID is used.");
        } else if (FP_Num == 0x1008) {
            sendcmd("Finger not found");
        }
    }

    // 6. 回到主畫面
    Initial_display();  // 顯示初始畫面
    Delayms(1000);  // 完成流程後的延遲
}

// 刪除指紋模式
void Delete_Mode() {
//    if (DeleteID_Fun == 1) {
//        // 刪除單一ID
//        BOOL deleteStatus = DeleteID(FP_Num);
//        if (deleteStatus == TRUE) {
//            sendcmd("Delete Successful!");
//        } else {
//            sendcmd("Delete Failed! ID not found.");
//        }
//    } else if (DeleteID_Fun == 2) {
//        // 刪除所有指紋
//        BOOL deleteStatus = DeleteAll();
//        if (deleteStatus == TRUE) {
//            sendcmd("All fingerprints deleted.");
//        } else {
//           sendcmd("Database is already empty.");
//        }
//    }
    Initial_display();
}

// 獲取已註冊指紋數量
void GetCount_Mode() {
	xxx = 0;
    int count = GetEnrollCount();
    sendcmd("Total Enrolled IDs: ");
    sendcmd(count + '0');  // 顯示註冊的數量
    sendcmd("OK");
    Initial_display();
}

DEFINE_ISR(UART1, 0x3C)
{
    // 處理 UART0 中斷
    if (_ur0f == 1) {
        u8 i;
	 for(i=0;i<16;i++)
	 {
	  RX_buf[i] = RX_buf[i+1];
	 } 
	 RX_buf[11] = _txr_rxr0;
	 _ur0f=0;

    }

    // 處理 UART1 中斷
    if (_ur1f == 1) {
        if (xxx >= 12) xxx = 0;    // 若索引超過 12 則歸零
        RX_buf[xxx++] = _txr_rxr1; // 存入 UART1 接收到的數據
        _ur1f = 0;                 // 清除 UART1 中斷標誌
    }
}


char function() {
    unsigned char num;
    num = RX_buf[xxx-1];  
    return num;  // 返回接收到的字符
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
    _brg1 = fH / ((u32)16 * BR) - 1;  // ?置波特率
    _mf5e = 1;
    _ur1e = 1;
    _emi = 1;
    _hircc = 0b00000011;
}






// 取得模組回應數據
void GetResponse(uint8_t *response, uint8_t length) {
    memset(response, 0, length);  // 清空 response 緩衝區
    uint8_t i;
    for (i = 0; i < length; i++) {
        response[i] = RX_buf[i];  // 將接收到的數據存入 response
    }
}
// 打印命令數據（僅限 Debug）

// 發送數據命令
void sendbytes(unsigned char *cmd_ptr, uint8_t length) {
uint8_t i;
    for (i = 0; i < length; i++) {
        while (!_txif1);  // 等待?送寄存器空
        _txr_rxr1 = cmd_ptr[i];  // ?送?前字?
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



// 模組初始化（Open）
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

// 模組關閉（Close）
uint16_t Close() {
    commandByte[8] = 0x02;  // Close command
    commandByte[10] = 0x02;

    sendbytes(commandByte, 12);
    Delayms(10);
    GetResponse(responseByte, 12);

    return (responseByte[8] == ACK) ? ACK : NACK_COMM_ERR;
}


// 獲取已註冊指紋數量
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

// 檢查 ID 是否已註冊
BOOL CheckEnrolled(uint8_t FP_Num ) {
 uint8_t i;
uint16_t sum = 0;

// 構造指令數據
 uint8_t commandByte[12]={0x55,0xAA,0x01,0x00,FP_Num ,0,0,0,0x21,0};

// 計算校驗和
for (i = 0; i < 10; i++) {
    sum += commandByte[i];
}
commandByte[10] = (uint8_t)(sum & 0xFF);  // 低8位
commandByte[11] = (uint8_t)((sum >> 8) & 0xFF);  // 高8位

    // 發送命令給指紋模組
    sendbytes(commandByte, 12);
    Delayms(10);

    // 獲取模組回應
    GetResponse(responseByte, 12);
  
    // 檢查回應是否為ACK，表明已註冊
  if (responseByte[8] == 0x30)return true;
  else return false;
}

// 開始註冊過程
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

// 執行 Enroll1
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

// 執行 Enroll2
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

// 執行 Enroll3
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

//檢查是否按壓手指
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

// 刪除指定 ID
BOOL DeleteID(uint16_t id) {
    commandByte[4] = id & 0xFF;
    commandByte[8] = 0x40;  // DeleteID command
    commandByte[10] = 0x40;

    sendbytes(commandByte, 12);
    Delayms(10);
    GetResponse(responseByte, 12);

    return (responseByte[8] == ACK) ? true : false;
}

// 刪除所有指紋
BOOL DeleteAll() {
    commandByte[8] = 0x41;  // DeleteAll command
    commandByte[10] = 0x41;

    sendbytes(commandByte, 12);
    Delayms(10);
    GetResponse(responseByte, 12);

    return (responseByte[8] == ACK) ? true : false;
}

// 驗證手指與指定 ID 是否匹配
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

// 1:N 指紋比對
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

    if (responseByte[8] == ACK) return responseByte[4];  // 返回識別的ID
    if (responseByte[4] == 0x0A) return 0x100A;  // Database is empty
    if (responseByte[4] == 0x08) return 0x1008;  // Identify failed
    return NACK_COMM_ERR;
}

// 捕捉指紋

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
    char temp[4];  // 假設 FP_Num 不超過 3 位數字
    if (FP_Num == 0) {
        ascii_str[i++] = '0';  // 特殊處理 0
    } else {
        while (FP_Num > 0) {
            temp[i++] = (FP_Num % 10) + '0';  // 將每一位轉換為對應的 ASCII 字符
            FP_Num /= 10;
        }
    }

    // 將字符順序反轉
    int j;
    for (j = 0; j < i; j++) {
        ascii_str[j] = temp[i - j - 1];
    }
    ascii_str[i] = '\0';  // 結尾符
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
 _pdc6=0;                   //規劃pSPK為輸出屬性
 _pcc0=0;
 _sledc1=0xc0;         //設置PD[6:4]源流為Level 3
 _stm2c0=0; _stm2c1=0b11000001;                  //STM2相關控制暫存器規劃
 _stm2ae=1; _mf4e=1; _emi=1;      //致能T2A中斷,EMI總開關
 while(counter < maxcount){
  _stm2al=Pitch_TAB[0];             //取得計數時間常數     
  _stm2ah=Pitch_TAB[0]>>8;
  _st2on=1; Delayms(400);      //啟動STM2計數並延遲400mS 
  _st2on=0;          //停止STM2計數
  _stm2al=Pitch_TAB[1];             //取得計數時間常數     
  _stm2ah=Pitch_TAB[1]>>8;
  _st2on=1; Delayms(400);      //啟動STM2計數並延遲400mS 
  _st2on=0;          //停止STM2計數
  _stm2al=Pitch_TAB[2];             //取得計數時間常數     
  _stm2ah=Pitch_TAB[2]>>8;
  _st2on=1; Delayms(400);      //啟動STM2計數並延遲400mS 
  _st2on=0;          //停止STM2計數  
  _stm2al=Pitch_TAB[3];             //取得計數時間常數     
  _stm2ah=Pitch_TAB[3]>>8;
  _st2on=1; Delayms(400);      //啟動STM2計數並延遲400mS 
  _st2on=0;          //停止STM2計數
  _stm2al=Pitch_TAB[4];             //取得計數時間常數     
  _stm2ah=Pitch_TAB[4]>>8;
  _st2on=1; Delayms(400);      //啟動STM2計數並延遲400mS 
  _st2on=0;          //停止STM2計數
  _stm2al=Pitch_TAB[5];             //取得計數時間常數     
  _stm2ah=Pitch_TAB[5]>>8;
  _st2on=1; Delayms(400);      //啟動STM2計數並延遲400mS 
  _st2on=0;          //停止STM2計數
  _stm2al=Pitch_TAB[6];             //取得計數時間常數     
  _stm2ah=Pitch_TAB[6]>>8;
  _st2on=1; Delayms(400);      //啟動STM2計數並延遲400mS 
  _st2on=0;          //停止STM2計數  
  _stm2al=Pitch_TAB[7];             //取得計數時間常數     
  _stm2ah=Pitch_TAB[7]>>8;
  _st2on=1; Delayms(400);      //啟動STM2計數並延遲400mS 
  _st2on=0;          //停止STM2計數
  counter++;
 }
 }
 
 DEFINE_ISR(ISR_STM2,0x38)
{ _pd6=!_pd6;             //pSPK狀態反向一次
 _stm2af=0;          //清除STM2AF
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

//實時模組副程式
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
    bin += 1;  // 加1
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
    // 開始發送數據
    sendcmd(open_tcp);  // 打開 TCP 連接
    do 
 { _pg0=~_pg0;
  Delayms(100);
  if(RX_wait(iserr,9)) break;
 } while(!RX_wait(isok,6));      //等待tcp開啟
 _pg0=1;
 sendcmd(cipsend);
 do 
 { _pg0=~_pg0;
  Delayms(100);
  if(RX_wait(iserr,9)) break;
 } while(!RX_wait(issend,8));     //等待傳送指令
 _pg0=1;
    sendcmd(request_header);  // 送出 HTTP POST 請求頭
    sendcmd(crlf);
    sendcmd(crlf);
    
 data_ptr = &data_msg[0];
 
    // 設置數據並發送
    setdatamsg(hr1);
    setdatamsg(str_v2);        // 發送小時數據
    setdatamsg(ampersand);     // 發送 '&'
    setdatamsg(min1);
    setdatamsg(str_v1);        // 發送小時數據
    setdatamsg(ampersand);     // 發送 '&'
    setdatamsg(sec1);
    setdatamsg(str_v0);        // 發送小時數據
    setdatamsg(ampersand);     // 發送 '&'
    setdatamsg(temp1);
    setdatamsg(str_v3);        // 發送溫度
    setdatamsg(ampersand);     // 發送 '&'
    setdatamsg(humm1);
    setdatamsg(str_v4);        // 發送濕度
 
    // 發送最終數據並完成
    
    sendcmd(data_msg);
    sendcmd(crlf);
    sendcmd(backslashzero);
    
    // 清空 data_msg 緩衝區
    
    memset(data_msg, 0x20, 62);
}

void setdatamsg(unsigned char *msg_ptr){
 while(*msg_ptr){
  *data_ptr = *msg_ptr;
  msg_ptr++;
  data_ptr++; 
 }
}