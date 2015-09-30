/*
 * FmiIpc.h
 *
 *  Created on: 25/09/2015
 *      Author: kel
 */

#ifndef FMIIPC_H_
#define FMIIPC_H_

#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>

#include "SharedFmiMessage.h"
using namespace sharedfmimemory;

// Definitions
#define IPC_BLOCK_COUNT			512
#define IPC_BLOCK_SIZE			4096

#define IPC_MAX_ADDR			256

class FmiIpc {
public:
	FmiIpc();
	virtual ~FmiIpc();

	class Server {
	public:
		// Construct / Destruct
		Server();
		~Server();

	private:
		// Internal variables
		char *m_sAddr;		// Address of this server
		HANDLE m_hMapFile;		// Handle to the mapped memory file
		HANDLE m_hSignal;		// Event used to signal when data exists
		HANDLE m_hAvail;// Event used to signal when some blocks become available
		SharedFmiMem *m_pBuf;		// Buffer that points to the shared memory
	public:
		// Create and destroy functions
		void create(void);
		void close(void);

		// TCHAR szName[];

		SharedFmiMessage* send(SharedFmiMessage* message, DWORD dwTimeout);
	};

	class Client {
	public:
		// Construct / Destruct
		Client(void);
		Client(const char *connectAddr);
		~Client();

	private:
		// Internal variables
		char *m_sAddr;		// Address of this server
		HANDLE m_hMapFile;		// Handle to the mapped memory file
		HANDLE m_hSignal;		// Event used to signal when data exists
		HANDLE m_hAvail;// Event used to signal when some blocks become available
		SharedFmiMem *m_pBuf;		// Buffer that points to the shared memory

		// Exposed functions
		//	DWORD					write(void *pBuff, DWORD amount, DWORD dwTimeout = INFINITE);	// Writes to the buffer
		bool waitAvailable(DWORD dwTimeout = INFINITE);	// Waits until some blocks become available
	public:
		//Block*					getBlock(DWORD dwTimeout = INFINITE);							// Gets a block
		//void					postBlock(Block *pBlock);										// Posts a block to be processed
		SharedFmiMessage* getMessage(DWORD dwTimeout = INFINITE);
		void sendReply(SharedFmiMessage* reply);

		// Functions
		BOOL IsOk(void) {
			if (m_pBuf)
				return true;
			else
				return false;
		}
		;
	};
};

//const TCHAR FmiIpc::Server::szName[]=TEXT("Local\\MyFileMappingObject");

#endif /* FMIIPC_H_ */
