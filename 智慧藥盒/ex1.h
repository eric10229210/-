#include "HT66F2390.h"
#include <STRING.h>
#include "Typedef.h"

// Error code definition
#define ACK                   0x30
#define NACK                  0x31
#define NACK_INVALID_POS      0x1003
#define NACK_IS_ALREADY_USED  0x1005
#define NACK_COMM_ERR         0x1006
#define NACK_VERIFY_FAILED    0x1007
#define NACK_IDENTIFY_FAILED  0x1008
#define NACK_DB_IS_FULL       0x1009
#define NACK_DB_IS_EMPTY      0x100A
#define NACK_BAD_FINGER       0x100C
#define NACK_ENROLL_FAILED    0x100D


#define fH 8000000  // MCU HIRC ?率
#define BR 38400  // 波特率
#define true 1
#define false 0
// 常量字符串
uint16_t Open();
void init_uart0();
void init_uart1();
void init_uart8266();
char RX_wait(unsigned char arr[],unsigned char size);
void GetResponse(uint8_t *response, uint8_t length);
void sendchar(char s);
void sendstring(const char *str);
void sendcmd(unsigned char *cmd_ptr);
void sendbytes(unsigned char *cmd_ptr, uint8_t length);
char function();



/*
 * object for controlling the BM92S2222-A Fingerprint Sensor Module
 */

    // Enable/Disable debug output using hardware UART 
    extern BOOL debug ;
    
    // the command packet to the BM92S2222-A module
    // the length is fixed 12 bytes, the first 4 bytes are always 0x55 0xAA 0x01 0x00
    extern uint8_t commandByte[12];

    // the response packet from the BM92S2222-A module
    // the length is fixed 12 bytes, the first 4 bytes are always 0x55 0xAA 0x01 0x00
    extern uint8_t responseByte[12];

    // Initialize the module
    // Return: 
    // 0x30 - ACK
    // 0x1006 - Failed (NACK_COMM_ERR)
	void Init(uint8_t getProductInfo);

    // Shut down the module
    // Return: 
    // 0x30 - ACK
    // 0x1006 - Failed (NACK_COMM_ERR)
    uint16_t Close();

    // Change baud rate
    // Parameter: baud rate 9600, 19200, 38400, 57600, 115200
    // Return: 
    // true - success
    // false - invalid baud
    BOOL ChangeBaudRate(unsigned long baud);   

    // Get the number of enrolled fingerprints
    // Return: 
    // the number of enrolled fingerprints
    // 0x1006 - Failed (NACK_COMM_ERR)
    uint16_t GetEnrollCount();

    // Check to see if the ID number is enrolled or not
    // Parameter: 0-99
    // Return: 
    // true - the ID number is enrolled. 
    // false - the ID number is not enrolled.
    BOOL CheckEnrolled(uint16_t);

    // Start the enrollment process
    // Parameter: 0-99
    // Return:
    //  0 - ACK
    //  1 - Database is full
    //  2 - Invalid Position
    //  3 - Position(ID) is already used    
    uint16_t EnrollStart(FP_Num);

    // Get the first scan of an enrollment
    // Return:
    //  0 - ACK
    //  1 - Enroll Failed
    //  2 - Bad finger
    //  3 - ID is already used
    uint16_t Enroll1();

    // Get the 2nd scan of an enrollment
    // Return:
    //  0 - ACK
    //  1 - Enroll Failed
    //  2 - Bad finger
    //  3 - ID is already used
    uint16_t Enroll2();    

    // Get the 3rd scan of an enrollment
    // Return:
    //  0 - ACK
    //  1 - Enroll Failed
    //  2 - Bad finger
    //  3 - ID is already used
    uint16_t Enroll3();    

    // Check to see if a finger is pressed on the fingerprint sensor
    // Return:
    // true - a finger is pressed on the fingerprint sensor
    // false - no finger is pressed on the fingerprint sensor
    BOOL IsPressFinger();

    // Delete the specified ID from the database
    // Parameter: 0-99 (the ID number to be deleted)
    // Return: 
    // true - successful 
    // false - invalid position
    BOOL DeleteID(uint16_t);

    // Delete all IDs from the database
    // Return: 
    // true - successful 
    // false - database is empty
    BOOL DeleteAll();

    // Check the currently pressed finger against a specific ID
    // Parameter: 0-99 (the ID number to be checked)
    // Return:
    //  0 - Verified OK (the correct finger)
    //  1 - Invalid Position
    //  2 - ID is not used
    //  3 - Verified FALSE (not the correct finger)
    uint16_t Verify1_1(uint16_t);

    // Check the currently pressed finger against all enrolled fingerprints
    // Return:
    //  0-99: the ID number of the pressed finger 
    //  0x1008: Identify Failed
    //  0x100A: Database is empty
    uint16_t Identify1_N();
    
    // Get the fingerprint data and store in the memory of the module
    // The data is then used for enrollment, verification, identification
    // Return:
    // true - successful
    // false - finger is not pressed
    BOOL CaptureFinger();
   
    // create a new object to interface with the BM92S2222-A
    _BM92S2222(uint8_t rx, uint8_t tx);

    //destructor

    
    // Get the response date from the BM92S2222-A module
    // Parameter:
    // the pointer of the array for storing response date
    // the response date length: for READ commands, length=6. for WRITE commands, length=3.
    void GetResponse(uint8_t *response, uint8_t length);
    
	// Print the command packet on the serial monitor, for debug purpose
	void printSend();
    
void Delayms(u16 del);
