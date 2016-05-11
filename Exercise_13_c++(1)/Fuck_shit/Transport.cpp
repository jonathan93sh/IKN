#include <iostream>
#include <cstring>
#include <cstdio>
#include <Transport.h>
#include <TransConst.h>

#define DEFAULT_SEQNO 2

namespace Transport
{
	/// <summary>
	/// Initializes a new instance of the <see cref="Transport"/> class.
	/// </summary>
	Transport::Transport (short BUFSIZE)
	{
		link = new Link::Link(BUFSIZE+ACKSIZE);
		checksum = new Checksum();
		buffer = new char[BUFSIZE+ACKSIZE];
		seqNo = 0;
		old_seqNo = DEFAULT_SEQNO;
		errorCount = 0;
		bufsize_ = BUFSIZE+ACKSIZE;
	}

	/// <summary>
	/// Delete dynamics attribute before delete this object.
	/// </summary>
	Transport::~Transport()
	{
		if(link) delete link;
		if(checksum) delete checksum;
		if(buffer) delete [] buffer;
	}
	
	/// <summary>
	/// Receives the ack.
	/// </summary>
	/// <returns>
	/// The ack.
	/// </returns>
	bool Transport::receiveAck()
	{
		char buf[ACKSIZE];
		short size;
		do{
		 	size = link->receive(buf, ACKSIZE);
		 }while(size < 0);
		std::cout<<"ACK modtaget - size: " << size << " / " << ACKSIZE << std::endl;
		if (size != ACKSIZE) return false;
		if(!checksum->checkChecksum(buf, ACKSIZE) ||
				buf[SEQNO] != seqNo ||
				buf[TYPE] != ACK)
		{
			std::cout<<"ACK modtaget - error: " << (int)buf[SEQNO] << " seq_nr: " << (int)seqNo << " buf[TYPE] = " << (int)buf[TYPE] << std::endl;	
			return false;
		}
		seqNo = (buffer[SEQNO] + 1) % 2;
		std::cout<<"ACK modtaget: " << (int)buf[SEQNO] << " seq_nr: " << (int)seqNo << std::endl;	
		return true;
	}

	/// <summary>
	/// Sends the ack.
	/// </summary>
	/// <param name='ackType'>
	/// Ack type.
	/// </param>
	void Transport::sendAck (bool ackType)
	{
		char ackBuf[ACKSIZE];
		ackBuf [SEQNO] = (ackType ? buffer[SEQNO] : (buffer[SEQNO] + 1) % 2);
		ackBuf [TYPE] = ACK;
		checksum->calcChecksum (ackBuf, ACKSIZE);

		std::cout<<"ACK send: " << (int)ackBuf[SEQNO] << " seq_nr: " << (int)seqNo << std::endl;
		link->send(ackBuf, ACKSIZE);
	}

	/// <summary>
	/// Send the specified buffer and size.
	/// </summary>
	/// <param name='buffer'>
	/// Buffer.
	/// </param>
	/// <param name='size'>
	/// Size.
	/// </param>
	/*
	void Transport::send(const char buf[], short size)
	{
		//lol
		checksum->calcChecksum(buf,size);
		char header[28] = buf[CHKSUMHIGH]+buf[CHKSUMLOW]+seqNo+ackTYPE;

		//wait for 0 from above
		ackTYPE = 0;
		char size_packet = size+strlen(header);
		char packet[size_packet] = header+buf;

		link->send(packet,size_packet);
		while(!receiveAck());

		//.... if fail => send again.. timeouts

		//if ack received with right seqNR => done..
	}
	*/

	void Transport::send(const char buf[], short size)
	{

		if(size > bufsize_-ACKSIZE)
			return;
	
		for(int i = 0; i < size; i++)
		{
			buffer[i+ACKSIZE]=buf[i];
		}


		do{
			buffer[SEQNO] = seqNo;
			buffer[TYPE] = DATA;

			checksum->calcChecksum(buffer,size+ACKSIZE);
			std::cout << "send : seqNO " << (int)buffer[SEQNO] << " datatype : " << (int)buffer[TYPE] << std::endl;

			link->send(buffer,size+ACKSIZE);

		}while(!receiveAck());

	}

	/// <summary>
	/// Receive the specified buffer.
	/// </summary>
	/// <param name='buffer'>
	/// Buffer.
	/// </param>
	short Transport::receive(char buf[], short size)
	{

		int size_of_rcv;
		do{
			do
			{
				size_of_rcv = link->receive(buffer, size+ACKSIZE);
			}while(size_of_rcv < -1);
			
			if(size_of_rcv < -1)
				return size_of_rcv;
			
			if(size_of_rcv > -1 && (!checksum->checkChecksum(buffer, size_of_rcv) || buffer[TYPE]!=DATA || (buffer[SEQNO] != seqNo && old_seqNo != 2) || size_of_rcv-ACKSIZE > size))
			{
				buffer[SEQNO]=seqNo;
				sendAck(ACK);
				std::cout << "modtaget en oedelagt besked : " << (int)buffer[SEQNO] << " = " << (int)seqNo << " datatype : " << (int)buffer[TYPE] << std::endl;
				size_of_rcv = -1;
			}
			
			
		}while(size_of_rcv == -1);

		std::cout << "modtaget en besked : " << (int)buffer[SEQNO] << " datatype : " << (int)buffer[TYPE] << std::endl;
		old_seqNo=buffer[SEQNO];
		seqNo=(old_seqNo+1)%2;


		for(int i = ACKSIZE; i < size_of_rcv; i++)
		{
			buf[i-ACKSIZE]=buffer[i];
		}

		sendAck(ACK);

		return size_of_rcv-ACKSIZE;
			
	}



}


