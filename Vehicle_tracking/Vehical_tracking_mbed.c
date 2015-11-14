#include "mbed.h"
#include "string.h"
DigitalOut myled(LED1);
int command_size(char *cmdp);
void CheckModem();
Serial pc(SERIAL_TX,SERIAL_RX);
RawSerial uart3(PC_10,PC_11);
char *cmd[]={"AT+CMGF=1","AT+CMGS=\"9916822399\""};
enum CMDRESULT
{
    AT_SUCESS = 1,
    AT_ERROR =  2,
    AT_TIMEOUT = 3
};
CMDRESULT run_command(char *command,char *responce);

int main() {

    pc.baud(9600);
    uart3.baud(9600);
    char buf[20];
    int ii=0;
    char c;
    char respString[3];
    CMDRESULT tt;
    pc.printf("Device Booting Up..\r\n");
    CheckModem(); // Test if the moden is up and running
    pc.printf("Device Ready..\r\n");
    pc.printf("Press 'c' to checkmodem, or 'm' to transmit data\r\n");
    while(1) {

        if(pc.readable())
        {
         switch(pc.getc())
            {
                case 'c':
                CheckModem();
                break;
                
                case 'm': // send message
                ii=0;
                memset(buf,0,20);
                tt=run_command(cmd[0],respString);
                if(tt==AT_SUCESS)
                {
                    pc.printf("\nAT_SUCESS\r\n");
                    pc.printf("Data Transmitted To Server..");
                }
                else
                {
                    pc.printf("\nAT_ERROR\r\n");
                }
                break;
            }
         }
         
    }// out of while
    
}

CMDRESULT run_command(char *command,char *responce)
{
    char local_buff[64];
    char respString[3];
    char *ptr,c,d,data[64];
    int flag = 0,ii=0;
    static int test=0; // flag for extracting the 2 char after first \r\n
    memset(local_buff,0,64);
    int index=0,size_cmd=0,count=0;
    size_cmd=command_size(command);
    //pc.printf("size of command is %d\r\n",size_cmd);
    uart3.puts(command);
    uart3.puts("\r");
    // read the serial port untile the modem gives the command + responce 
    // travers until the command is obtained
    //read from serial port one by one till you find match to cmd_string<cr>
    while(!(ptr=strstr(local_buff,command)) && local_buff[size_cmd + index] != 0x0D )  //local_buff[size_cmd + index] will get the next char after the command string
    {
        {
           c=uart3.getc();
            //pc.printf("%c",c);
            local_buff[index]=c;
            index++;
        }      
    }
    //reset the buffer to empty and continue reading
    memset(local_buff,0,64);
    index = 0;
    //search for <cr><lf> first and second – respString = extract string in between
    while(flag != 2)
    {
        if(uart3.readable())
        {
            c=uart3.getc();
            local_buff[index]=c;
            if(local_buff[index]=='\n')flag++;
            index++;
         }
    }//end of while     
     local_buff[index]='\0';
     if(ptr=strstr(local_buff,"\r\n"))
        {
                respString[0]=*(ptr+2);
                respString[1]=*(ptr+3);
                respString[2]='\0';
        }
     //pc.printf("\n respString=%s \r\n",respString);
    //continue reading till prompt character(‘>’) is received
    uart3.puts(cmd[1]);
    uart3.puts("\r");
    while(uart3.getc() != '>');
    sprintf(data,"%d",count);
    uart3.puts(data);
    uart3.putc(26);
    uart3.puts("\r\n");
    //wait(0.2);
    strcpy(responce,respString);

    if(strcmp(respString,"OK") == 0)
    {
        return AT_SUCESS;
    }
    else
    {
        return AT_ERROR;
    }
}

int command_size(char *cmdp)
{
    int size_cmd=0;
    while(*cmdp != '\0')
    {
        cmdp++;
        size_cmd++;
    }
    return size_cmd;
}

void CheckModem()
{
    int ii=0;char buf[20],c;
    pc.printf("Modem Checking..\r\n");
    uart3.puts("AT\r");
    //wait(0.2);
    while(ii<8){
        if(uart3.readable()){
        c=uart3.getc();buf[ii++]=c;}}
        buf[ii]='\0';
        pc.printf("\r%s\r\n",buf);
        if(strstr(buf,"OK"))
        printf("Moden Working Fine\r\n");
        else
        printf("Error with the modem,Try Reconnecting the modem..\r\n");
}
