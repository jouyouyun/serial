#include <fcntl.h>
#include <time.h>
#include <ctype.h>
#include <termio.h>
#include <stdio.h>

#define FALSE 0
#define TRUE 1


/***************************************************************************************
 *  This Program is a simple demo of how to read a table of 65 holding registers       *
 *  from a modicon 984 plc via an rs232 serial port using the Linux operating system   *
 *	please feel free to use and modify .                                               * 
 *	I have tested it on a free modicon plc simulator available from www.win-tech.com . * 
 *	Any comments to                                                                    *
 *	paul@pmcrae.freeserve.co.uk   (http://www.pmcrae.freeserve.co.uk)                  *
 *                                                                                     *
 ***************************************************************************************/



int set_up_comms(void);

unsigned int crc(unsigned char buf[],int start,int cnt);

int read_hold_reg(int slave, int com_port, int baud,char parity,
             int timeout,int offset,int num_regs,int *reg_table);

struct termio tnew,tsaved;
int ttyfd;               

int table[100];


int main()
{
  int result;
  int tabindex;
  
  /* setup communication parameters */

  set_up_comms();

  result = read_hold_reg(1,2,9600,'O',10,101,65,table);
  
  /* Read a table of 65 registers from plc address 1 holding register 400101 */

  if(result == FALSE) /* if no comms erros */
  {
    for(tabindex=0;tabindex < 66;tabindex++)       
      printf("%d\n",table[tabindex]);
  }
  
  ioctl(ttyfd,TCSETA,&tsaved);

  close(ttyfd);

  getchar();
  
  return 0;
}  
  


               
               
int read_hold_reg(int slave, int com_port, int baud,char parity,
               int timeout,int offset,int num_regs,int *reg_table)
    {
     
       int byte_count;
       int bytes_required;
       int bytes_received = 0;
       int function;
       unsigned short crc_calc = 0;
       unsigned short crc_received = 0;
       unsigned int temp;
       int i;
       int error_flag;
       unsigned char recv_crc_hi;
       unsigned char recv_crc_lo;      
       char rxchar = -1;
       unsigned char query[8];
       unsigned char recv[1024];
       unsigned char offset_hi;
       unsigned char offset_lo;
       unsigned char crc_hi;
       unsigned char crc_lo;
       unsigned char numreg_hi;
       unsigned char numreg_lo;
       time_t start;
       time_t finish;
       int diff_time;
       
       
      /****************** Initialise Variables  ***********************/
       
       offset = offset - 1;             /* split offset into hi & lo bytes */
       offset_hi = offset >> 8;
       offset_lo = offset & 0x00FF;
       
       function = 3;                    /* set code to read holding reg   */
       error_flag = FALSE;
       
       numreg_hi = num_regs >> 8;        /*split numregs int hi & lo bytes */
       numreg_lo = num_regs & 0x00FF; 
       
       bytes_required = 5 + (num_regs * 2);    /* two bytes per register read */
                                            /* plus 5 bytes header         */
                                            
                                                  
              
       query[0] = slave;       /* plc address */
       query[1] = function;    /* function code to read holding register */
       query[2] = offset_hi;   /* base address of register table Hi byte */
       query[3] = offset_lo;   /* base address of register table lo byte */
       query[4] = numreg_hi;   /* num of registers to read hi byte       */  
       query[5] = numreg_lo;   /* num of registers to read lo byte       */
       
       /* Calculate 16 bit CRC on query message */
       temp = crc(query,0,6);
       crc_hi = temp >> 8;
       crc_lo = temp & 0x00FF;
       
       query[6] = crc_hi;
       query[7] = crc_lo;
       query[8] = NULL;
       
 /****************************************************************/
       
       
 /******************** Communications Setup  *********************/       
                         
      /* ttyfd = setup_comms(com_port,baud,parity); */
      
      /* function to be finished when I've got time */     
       
 /****************************************************************/      
       
       
  /**************  send message to slave *************************/
       
       ioctl(ttyfd,TCFLSH,0);   /*  clear serial input  buffer */
       ioctl(ttyfd,TCFLSH,1);   /*  clear serial output buffer */
       
       write(ttyfd,query,8);    /*  send message to slave      */
       
       ioctl(ttyfd,TCFLSH,0);   /*  clear serial input  buffer */      
              
  /***************************************************************/     
       
       
  /*************  save time message was sent to slave*************/
         
       time(&start);
            
  /************** get slave response *****************************/
  
       bytes_received = 0;
  
        
       while(bytes_received < bytes_required)
       {         
         read(ttyfd,&rxchar,1);
         
         if (rxchar != -1)
         {
           recv[bytes_received] = rxchar;  
           printf("%s%d\n","char rx ",rxchar);
           bytes_received++;
           printf("%s%d\n","bytes received ",bytes_received);
         }  
         
         
                            
         time(&finish);
         
         diff_time = difftime(finish,start);
                       
         if(diff_time > timeout)
         {
           printf("timeout \n");
           error_flag = TRUE;
         }
       } /* end while */                               
               
                              
  /**************** Decode Response from Plc *******************/             
       
       /*********** check CRC of response ************/
       if(bytes_required == bytes_received)
       {
         crc_calc = crc(recv,0, bytes_received - 2);
       
         recv_crc_hi = (unsigned) recv[bytes_received -2];
         recv_crc_lo = (unsigned) recv[bytes_received -1];
       
         
         crc_received = recv[bytes_received -2];
         crc_received = (unsigned)  crc_received << 8;
         crc_received = crc_received | (unsigned) recv[bytes_received -1];
         
       
         if (crc_calc != crc_received)
         {
           printf("crc error \n");
           printf("%s%x\n","crc_received ",crc_received);
           printf("%s%x\n","crc_calc ",crc_calc);

           error_flag = TRUE;         
         }  
       }  
              
      /*********************************************/
      

      /********** check for exception response *****/
      
      if(bytes_received < bytes_required)
      {
        if (recv[1] != function)
        {
          printf("exception response \n");
          error_flag = TRUE;
        }    
      }
      
      /******** extract data part of message ******/

      
        
      if(error_flag == FALSE)
      {  
                     
        for(i = 0; i < num_regs; i++)
        {
          temp = recv[3 + i *2] << 8;     /* shift reg hi_byte to temp */
          temp = temp | recv[4 + i*2];    /* OR with lo_byte           */
        
          reg_table[i] = temp;
        }
       } 
      
      /********************************************/
      
      if(error_flag)
      {    
        return TRUE;
      }
      else
      {  
        return FALSE;
      }
      
     
 } 
 /*************************END OF READ_HOLD_REG**********************/
 
  
  
  
 
int set_up_comms( )
{


  ttyfd = open("/dev/ttyS1",O_RDWR);
  
  ioctl(ttyfd,TCGETA,&tsaved);
  
  tnew.c_line = 0;
  tnew.c_oflag &=~ OPOST;
  tnew.c_lflag &=~ ICANON;
  tnew.c_cflag &=~ CBAUD;
  tnew.c_cflag |= B9600;                     /* baud rate 9600 */
  tnew.c_cflag &=~ CSIZE;
  tnew.c_cflag |= CS8;                       /* 8 bits RTU */
  tnew.c_cflag |= (PARENB | PARODD);         /* odd parity */
  tnew.c_cflag |= CREAD;                     /* enable read */
  tnew.c_cflag |= CLOCAL;                    /* ignore modem lines */
  tnew.c_cc[VMIN] = 0;
  tnew.c_cc[VTIME] = 0;
  ioctl(ttyfd,TCSETA,&tnew);

}
 
  
  
  


/*
*****************************************************************************
***************************** [  BEGIN:  crc ] ******************************
*****************************************************************************
INPUTS:
   buf   ->  Array containing message to be sent to controller.
   start ->  Start of loop in crc counter, usually 0.
   cnt   ->  Amount of bytes in message being sent to controller/
OUTPUTS:
   temp  ->  Returns crc byte for message.
COMMENTS:
   This routine receives the data message to be sent down to the controller
and calculates the crc high and low byte of that message.
*****************************************************************************
*/
unsigned int crc(unsigned char buf[],int start,int cnt)
{
   int      i,j;
   unsigned temp,temp2,flag;

   temp=0xFFFF;

   for (i=start; i<cnt; i++){
      temp=temp ^ buf[i];

      for (j=1; j<=8; j++){
	 flag=temp & 0x0001;
	 temp=temp >> 1;
	 if (flag) temp=temp ^ 0xA001;
      }
   }

   /*
   ** Reverse byte order.
   */
   temp2=temp >> 8;
   temp=(temp << 8) | temp2;
   return(temp);
}
/*
************************** [ END:  crc ] ************************************
*/

  
/*******************************************************************************************
    Please Note  There should be a gap of at least 20ms between multiple modbus messages
	as comms errors can result this has been ommited for simplicity .
	I will try and find the time to correct this but the aim of this little project was
	to try and spark off other peoples interest in producing linux software for modbus
	and not to produce a perfect system as I don't have the time.
********************************************************************************************/
  
  
  
  
  
  
  
  
  
  
                
                 
                       
