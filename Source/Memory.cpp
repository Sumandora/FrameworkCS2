#include "Memory.hpp"

#include "BCRL.hpp"

#include "Interfaces.hpp"

void* RetAddrSpoofer::leaveRet;

void Memory::Create()
{
	// Set the address for the return address spoofer
	RetAddrSpoofer::leaveRet = BCRL::Session::arrayPointer(Interfaces::source2Client, 0) // random code piece
								   .nextByteOccurrence("c9 c3", true)
								   .getPointer()
								   .value();
}