#include <Link.h>
#include <cstdio>

namespace Link {

/**
  * Explicitet constructor for linklaget.
  * Åbner den serielle port og sætter baudraten, paritet og antal stopbits
  */
Link::Link(int bufsize)
  :bufsize_(bufsize)
{
	buffer = new char[(bufsize_*2)];
	
    serialPort=v24OpenPort("/dev/ttyS1",V24_STANDARD);
    if ( serialPort==NULL )
    {
        fputs("error: sorry, open failed!\n",stderr);
        exit(1);
    }

    int rc=v24SetParameters(serialPort,V24_B115200,V24_8BIT,V24_NONE);
    if ( rc!=V24_E_OK )
    {
        fputs("error: setup of the port failed!\n",stderr);
        v24ClosePort(serialPort);
        exit(1);
    }

    rc=v24SetTimeouts(serialPort,5);
    if ( rc!=V24_E_OK )
    {
        fputs("error: setup of the port timeout failed!\n",stderr);
        v24ClosePort(serialPort);
        exit(1);
    }

    rc=v24FlushRxQueue(serialPort);
    if ( rc!= V24_E_OK )
    {
    	fputs("error: flushing receiverqueue\n", stderr);
    	v24ClosePort(serialPort);
    	exit(1);
    }

    rc=v24FlushTxQueue(serialPort);
    if ( rc!= V24_E_OK )
    {
    	fputs("error: flushing transmitterqueue\n", stderr);
    	v24ClosePort(serialPort);
    	exit(1);
    }
}

/**
  * Destructor for linklaget.
  * Lukker den serielle port
  */
Link::~Link()
{
	if(serialPort != NULL)
		v24ClosePort(serialPort);
	if(buffer != NULL)
		delete [] buffer;
}

/**
 * Bytestuffer buffer, dog maximalt det antal som angives af size
 * Herefter sendes de til modtageren via den serielle port
 *
 * @param buffer De bytes som skal sendes
 * @param size Antal bytes der skal sendes
 */
 //int v24Write(v24_port_t * port,const unsigned char * Buffer,size_t Len) 	
 // int v24Putc(v24_port_t * port,unsigned char TheData) 	
void Link::send(char buf[], short size)
{
 	v24Putc(serialPort, 'A');

 	for(int i = 0; i < size; i++)
 	{
 		if(buf[i] == 'A')
 		{
 			v24Putc(serialPort, 'B');
 			v24Putc(serialPort, 'C');
 		}
 		else if(buf[i] == 'B')
 		{
 			v24Putc(serialPort, 'B');
 			v24Putc(serialPort, 'D');
 		}
 		else
 		{
 			v24Putc(serialPort, buf[i]);
 		}
 	}


 	v24Putc(serialPort, 'A');
}

/**
 * Modtager data fra den serielle port og debytestuffer data og gemmer disse i buffer.
 * Modtager højst det antal bytes som angives af size.
 * Husk kun at læse en byte ad gangen fra den serielle port.
 *
 * @param buffer De bytes som modtages efter debytestuffing
 * @param size Antal bytes der maximalt kan modtages
 * @return Antal bytes modtaget
 */

 // int v24Read(v24_port_t * port, unsigned char * Buffer, size_t Len)
 //  int v24Getc 	( 	v24_port_t *  	port 	 )  
 // int v24QueryErrno 	( 	v24_port_t *  	port 	 ) 
 // V24_OK, V24_E_NULL_POINTER, V24_E_ILLHANDLE, V24_E_TIMEOUT or V24_READ 	
short Link::receive(char buf[], short size)
{
 	short sizeofbuf = 0;
 	bool stop = false, start = false;
 	char sidsteTegn = NULL;

 	do
 	{
 		int RXdata = v24Getc(serialPort);
 		
 		if(RXdata == -1)
 		{
 			switch(v24QueryErrno(serialPort))
 			{
 			case V24_OK:
 			case V24_READ:
 			break;
 			case V24_E_TIMEOUT:
 			default:
 				return -1;
 			break;
 			}
 		}
 		else if((char)RXdata == 'A' && !start)
 		{
 			start = true;
 		}
 		else if((char)RXdata == 'A' && start)
 		{
 			stop = true;
 		}
 		else if(start)
 		{
 			if(sidsteTegn == 'B')
 			{
 				if((char)RXdata == 'C')
 					buf[sizeofbuf] = 'A';
 				else if((char)RXdata == 'D')
 					buf[sizeofbuf] = 'B';
 				else
 					return -1;

 				sidsteTegn = NULL;
				
 			}
 			else
 			{
 				buf[sizeofbuf]=(char)RXdata;
 				sidsteTegn=buf[sizeofbuf];
 				if(buf[sizeofbuf] != 'B')
 					sizeofbuf++;
 			}
 		}

 	}while(stop == false && sizeofbuf != size);

 	if(stop == false)
 	{
 		return -1;
 	}

 	return sizeofbuf;
}

} /* namespace Link */
