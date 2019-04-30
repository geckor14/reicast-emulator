#pragma once

#include "build.h"


#if defined(TARGET_PS4) || USE_CUSTOM_ALLOCATORS

#include <cstdlib>

extern "C" void* zmalloc (unsigned long size);
extern "C" void* z_calloc(size_t nelem, size_t size);
extern "C" void* zrealloc(void* ptr, unsigned long size);
extern "C" int   zmemalign(void **ptr, unsigned long alignment, unsigned long size);
extern "C" void  zfree(void* ptr);
//extern "C" void  zfree2(void* ptr, unsigned long size);

void* operator new(size_t size);
void operator delete(void*);

#define memalign zmemalign
#define posix_memalign zmemalign
#define realloc zrealloc
#define malloc zmalloc
#define calloc z_calloc
#define free   zfree

#endif

#if !defined(TARGET_PS4) //&& !defined(_DEBUG)
# define zpf(...) __noop
#else
# define zpf(...) { fprintf(stdout,__VA_ARGS__); fflush(stdout); }
#endif




#if BUILD_COMPILER==COMPILER_VC
#define DECL_ALIGN(x) __declspec(align(x))
#else
#ifndef __forceinline
#define __forceinline inline
#endif
#define DECL_ALIGN(x) __attribute__((aligned(x)))
#ifndef _WIN32
#define __debugbreak
#endif
#endif


#if HOST_CPU == CPU_X86

	#if BUILD_COMPILER==COMPILER_VC
	#define DYNACALL  __fastcall
	#else
	//android defines fastcall as regparm(3), it doesn't work for us
	#undef fastcall
	#define DYNACALL __attribute__((fastcall))
	#endif
#else
	#define DYNACALL
#endif

#if BUILD_COMPILER==COMPILER_VC
#ifdef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES
#undef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES
#endif

#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1

#ifdef _CRT_SECURE_NO_DEPRECATE
#undef _CRT_SECURE_NO_DEPRECATE
#endif

#define _CRT_SECURE_NO_DEPRECATE
//unnamed struncts/unions
#pragma warning( disable : 4201)

//unused parameters
#pragma warning( disable : 4100)
#endif



#if BUILD_COMPILER==COMPILER_VC
//SHUT UP M$ COMPILER !@#!@$#
#ifdef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES
#undef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES
#endif

#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1

#ifdef _CRT_SECURE_NO_DEPRECATE
#undef _CRT_SECURE_NO_DEPRECATE
#endif
#define _CRT_SECURE_NO_DEPRECATE

#pragma warning(disable : 4482)		// Do not complain when i use enum::member
#pragma warning(disable : 4201)		// unnamed struncts/unions
#pragma warning(disable : 4100)		// unused parameters


#ifdef _Z_  // Annoying things, only disable for dev. builds in cmake so I can actually see something other than warnings easily //
  #pragma warning(disable : 4018)	// Signed/Unsigned , these _should_ be fixed, as with many other warnings the deps do it too so i'm not going to look @ them
  #pragma warning(disable : 4244)	// 
  #pragma warning(disable : 4267)	// : '=' : conversion from '' to '', possible loss of data
  #pragma warning(disable : 4312)	// ptr conv of greater size?  better than lesser ...
  #pragma warning(disable : 4333)	// 
  #pragma warning(disable : 4996)	// The POSIX name for this item is deprecated. Instead, use the ISO ...   << deps don't include types.h :|

  #pragma warning(disable : 4313)	// 'printf' : '%X' in format string conflicts with argument 2 of type 'DynarecCodeEntryPtr'
  #pragma warning(disable : 4477)	// 'printf' : format string '%06X' requires an argument of type 'unsigned int', but variadic argument 2 has type 'DynarecCodeEntryPtr'
#endif

#endif

#include <stdint.h>
#include <stddef.h>

//basic types
typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

#ifdef _M_X64
#undef X86
#define X64
#endif

typedef ptrdiff_t snat;
typedef size_t unat;

#ifdef X64
typedef u64 unat;
#endif

typedef char wchar;

#define EXPORT extern "C" __declspec(dllexport)



#ifndef CDECL
#define CDECL __cdecl
#endif



// constexpr only !
template<typename T> constexpr T KB(const T n) { return 1024 * n;  }
template<typename T> constexpr T MB(const T n) { return 1024 * KB(n); }
template<typename T> constexpr T GB(const T n) { return 1024 * MB(n); }
template<typename T> constexpr T TB(const T n) { return 1024 * GB(n); }

template<typename T> constexpr T KHz(const T n) { return 1000 * n;  }
template<typename T> constexpr T MHz(const T n) { return 1000 * KHz(n); }
template<typename T> constexpr T GHz(const T n) { return 1000 * MHz(n); }

// using KiB=KHz, MiB=MHz, GiB=GHz //


// Generic Alignment for non pow2, up is abusable

template<typename T> inline T Align(const T addr, const T align, unat up=0)
{
	return (addr / align + up) * align;
}

// Alignment for unsigned integers of any type, align must be pow2!

#define _POW2_MASK (align - static_cast<T>(1))

template<typename T> inline T AlignUp(const T addr, const T align)
{
	return (addr + _POW2_MASK) & ~_POW2_MASK;
}

template<typename T> inline T AlignDown(const T addr, const T align)
{
	return addr & ~_POW2_MASK;
}






//intc function pointer and enums
enum HollyInterruptType
{
	holly_nrm = 0x0000,
	holly_ext = 0x0100,
	holly_err = 0x0200,
};

enum HollyInterruptID
{
		// asic9a /sh4 external holly normal [internal]
		holly_RENDER_DONE_vd = holly_nrm | 0,	//bit 0 = End of Render interrupt : Video
		holly_RENDER_DONE_isp = holly_nrm | 1,	//bit 1 = End of Render interrupt : ISP
		holly_RENDER_DONE = holly_nrm | 2,		//bit 2 = End of Render interrupt : TSP

		holly_SCANINT1 = holly_nrm | 3,			//bit 3 = V Blank-in interrupt
		holly_SCANINT2 = holly_nrm | 4,			//bit 4 = V Blank-out interrupt
		holly_HBLank = holly_nrm | 5,			//bit 5 = H Blank-in interrupt

		holly_YUV_DMA = holly_nrm | 6,			//bit 6 = End of Transferring interrupt : YUV
		holly_OPAQUE = holly_nrm | 7,			//bit 7 = End of Transferring interrupt : Opaque List
		holly_OPAQUEMOD = holly_nrm | 8,		//bit 8 = End of Transferring interrupt : Opaque Modifier Volume List

		holly_TRANS = holly_nrm | 9,			//bit 9 = End of Transferring interrupt : Translucent List
		holly_TRANSMOD = holly_nrm | 10,		//bit 10 = End of Transferring interrupt : Translucent Modifier Volume List
		holly_PVR_DMA = holly_nrm | 11,			//bit 11 = End of DMA interrupt : PVR-DMA
		holly_MAPLE_DMA = holly_nrm | 12,		//bit 12 = End of DMA interrupt : Maple-DMA

		holly_MAPLE_VBOI = holly_nrm | 13,		//bit 13 = Maple V blank over interrupt
		holly_GDROM_DMA = holly_nrm | 14,		//bit 14 = End of DMA interrupt : GD-DMA
		holly_SPU_DMA = holly_nrm | 15,			//bit 15 = End of DMA interrupt : AICA-DMA

		holly_EXT_DMA1 = holly_nrm | 16,		//bit 16 = End of DMA interrupt : Ext-DMA1(External 1)
		holly_EXT_DMA2 = holly_nrm | 17,		//bit 17 = End of DMA interrupt : Ext-DMA2(External 2)
		holly_DEV_DMA = holly_nrm | 18,			//bit 18 = End of DMA interrupt : Dev-DMA(Development tool DMA)

		holly_CH2_DMA = holly_nrm | 19,			//bit 19 = End of DMA interrupt : ch2-DMA
		holly_PVR_SortDMA = holly_nrm | 20,		//bit 20 = End of DMA interrupt : Sort-DMA (Transferring for alpha sorting)
		holly_PUNCHTHRU = holly_nrm | 21,		//bit 21 = End of Transferring interrupt : Punch Through List

		// asic9c/sh4 external holly external [EXTERNAL]
		holly_GDROM_CMD = holly_ext | 0x00,	//bit 0 = GD-ROM interrupt
		holly_SPU_IRQ = holly_ext | 0x01,	//bit 1 = AICA interrupt
		holly_EXP_8BIT = holly_ext | 0x02,	//bit 2 = Modem interrupt
		holly_EXP_PCI = holly_ext | 0x03,	//bit 3 = External Device interrupt

		// asic9b/sh4 external holly err only error [error]
		//missing quite a few ehh ?
		//bit 0 = RENDER : ISP out of Cache(Buffer over flow)
		//bit 1 = RENDER : Hazard Processing of Strip Buffer
		holly_PRIM_NOMEM = holly_err | 0x02,	//bit 2 = TA : ISP/TSP Parameter Overflow
		holly_MATR_NOMEM = holly_err | 0x03		//bit 3 = TA : Object List Pointer Overflow
		//bit 4 = TA : Illegal Parameter
		//bit 5 = TA : FIFO Overflow
		//bit 6 = PVRIF : Illegal Address set
		//bit 7 = PVRIF : DMA over run
		//bit 8 = MAPLE : Illegal Address set
		//bit 9 = MAPLE : DMA over run
		//bit 10 = MAPLE : Write FIFO over flow
		//bit 11 = MAPLE : Illegal command
		//bit 12 = G1 : Illegal Address set
		//bit 13 = G1 : GD-DMA over run
		//bit 14 = G1 : ROM/FLASH access at GD-DMA
		//bit 15 = G2 : AICA-DMA Illegal Address set
		//bit 16 = G2 : Ext-DMA1 Illegal Address set
		//bit 17 = G2 : Ext-DMA2 Illegal Address set
		//bit 18 = G2 : Dev-DMA Illegal Address set
		//bit 19 = G2 : AICA-DMA over run
		//bit 20 = G2 : Ext-DMA1 over run
		//bit 21 = G2 : Ext-DMA2 over run
		//bit 22 = G2 : Dev-DMA over run
		//bit 23 = G2 : AICA-DMA Time out
		//bit 24 = G2 : Ext-DMA1 Time out
		//bit 25 = G2 : Ext-DMA2 Time out
		//bit 26 = G2 : Dev-DMA Time out
		//bit 27 = G2 : Time out in CPU accessing
};



struct vram_block
{
	u32 start;
	u32 end;
	u32 len;
	u32 type;

	void* userdata;
};

enum ndc_error_codes
{
	rv_ok = 0,			//no error
	rv_cli_finish=69,	//clean exit after -help or -version , should we just use rv_ok?

	rv_error=-2,		//error
	rv_serror=-1,		//silent error , it has been reported to the user
};

//Simple struct to store window rect  ;)
//Top is 0,0 & numbers are in pixels.
//Only client size
struct NDC_WINDOW_RECT
{
	u32 width;
	u32 height;
};



////////////////////////////////////////////////////////////////////////////////////////////////////////////

//******************************************************
//*********************** PowerVR **********************
//******************************************************

void libCore_vramlock_Unlock_block  (vram_block* block);
void libCore_vramlock_Unlock_block_wb  (vram_block* block);
vram_block* libCore_vramlock_Lock(u32 start_offset,u32 end_offset,void* userdata);



//******************************************************
//************************ GDRom ***********************
//******************************************************
enum DiscType
{
	CdDA=0x00,
	CdRom=0x10,
	CdRom_XA=0x20,
	CdRom_Extra=0x30,
	CdRom_CDI=0x40,
	GdRom=0x80,

	NoDisk=0x1,			//These are a bit hacky .. but work for now ...
	Open=0x2,			//tray is open :)
	Busy=0x3			//busy -> needs to be automatically done by gdhost
};

enum DiskArea
{
	SingleDensity,
	DoubleDensity
};

enum DriveEvent
{
	DiskChange=1	//disk ejected/changed
};

//******************************************************
//************************ AICA ************************
//******************************************************
void libARM_InterruptChange(u32 bits,u32 L);
void libCore_CDDA_Sector(s16* sector);


//passed on AICA init call


//Ram/Regs are managed by plugin , exept RTC regs (managed by main emu)

//******************************************************
//******************** ARM Sound CPU *******************
//******************************************************

//******************************************************
//****************** Maple devices ******************
//******************************************************


enum MapleDeviceCreationFlags
{
	MDCF_None=0,
	MDCF_Hotplug=1
};

struct maple_subdevice_instance;
struct maple_device_instance;

//buffer_out_len and responce need to be filled w/ proper info by the plugin
//buffer_in must not be edited (its direct pointer on ram)
//output buffer must contain the frame data , the frame header is generated by the maple routing code
//typedef u32 FASTCALL MapleSubDeviceDMAFP(void* device_instance,u32 Command,u32* buffer_in,u32 buffer_in_len,u32* buffer_out,u32& buffer_out_len);
typedef u32 MapleDeviceDMAFP(void* device_instance,u32 Command,u32* buffer_in,u32 buffer_in_len,u32* buffer_out,u32& buffer_out_len);

struct maple_subdevice_instance
{
	//port
	u8 port;
	//user data
	void* data;
	//MapleDeviceDMA
	MapleDeviceDMAFP* dma;
	bool connected;
	u32 reserved;	//reserved for the emu , DO NOT EDIT
};
struct maple_device_instance
{
	//port
	u8 port;
	//user data
	void* data;
	//MapleDeviceDMA
	MapleDeviceDMAFP* dma;
	bool connected;

	maple_subdevice_instance subdevices[5];
};


//includes from CRT
#include <stdlib.h>
#include <stdio.h>

#if defined(TARGET_NACL32)
	int nacl_printf(const wchar* Text,...);
	#define printf nacl_printf
	#define puts(X) printf("%s\n", X)
#endif

#if HOST_OS == OS_DARWIN
int darw_printf(const wchar* Text,...);
#define printf darw_printf
#define puts(X) printf("%s\n", X)
#endif

//includes from c++rt
#include <vector>
#include <string>
using namespace std;

//used for asm-olny functions
#if defined(X86) && COMPILER_VC==BUILD_COMPILER
#define naked   __declspec( naked )
#else
#define naked
#endif


#if DEBUG
//force
#define INLINE
//sugest
#define SINLINE
#else
//force
#define INLINE __forceinline
//sugest
#define SINLINE __inline
#endif

//no inline -- fixme
#if BUILD_COMPILER == COMPILER_VC
#define NOINLINE __declspec(noinline)
#else
#define NOINLINE __attribute__ ((noinline))
#endif

#if BUILD_COMPILER == COMPILER_VC
#define likely(x) x
#define unlikely(x) x
#else
#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)       __builtin_expect((x),0)
#endif

//basic includes
#include "stdclass.h"

#ifndef RELEASE
#define EMUERROR(format, ...) printf("Error in %20s:%s:%d: " format "\n", \
		__FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
//strlen(__FILE__) <= 20 ? __FILE__ : __FILE__ + strlen(__FILE__) - 20, 
#else
#define EMUERROR(format, ...)
#endif
#define EMUERROR2 EMUERROR
#define EMUERROR3 EMUERROR
#define EMUERROR4 EMUERROR

#ifndef NO_MMU
#define _X_x_X_MMU_VER_STR "/mmu"
#else
#define _X_x_X_MMU_VER_STR ""
#endif


#if DC_PLATFORM==DC_PLATFORM_DREAMCAST
	#define VER_EMUNAME		"reicast"
#elif DC_PLATFORM==DC_PLATFORM_DEV_UNIT
	#define VER_EMUNAME		"reicast-DevKit-SET5.21"
#elif DC_PLATFORM==DC_PLATFORM_NAOMI
	#define VER_EMUNAME		"reicast-Naomi"
#elif DC_PLATFORM==DC_PLATFORM_ATOMISWAVE
	#define VER_EMUNAME		"reicast-AtomisWave"
#else
	#error unknown target platform
#endif


#define VER_FULLNAME	VER_EMUNAME " git" _X_x_X_MMU_VER_STR " (built " __DATE__ "@" __TIME__ ")"
#define VER_SHORTNAME	VER_EMUNAME " git" _X_x_X_MMU_VER_STR


void os_DebugBreak();
#define dbgbreak os_DebugBreak()


#ifndef TARGET_NO_SSTATE

bool rc_serialize(void *src, unsigned int src_size, void **dest, unsigned int *total_size) ;
bool rc_unserialize(void *src, unsigned int src_size, void **dest, unsigned int *total_size);
bool dc_serialize(void **data, unsigned int *total_size);
bool dc_unserialize(void **data, unsigned int *total_size);

#define REICAST_S(v) rc_serialize(&(v), sizeof(v), data, total_size)
#define REICAST_US(v) rc_unserialize(&(v), sizeof(v), data, total_size)

#define REICAST_SA(v_arr,num) rc_serialize(v_arr, sizeof(v_arr[0])*num, data, total_size)
#define REICAST_USA(v_arr,num) rc_unserialize(v_arr, sizeof(v_arr[0])*num, data, total_size)

#else // let me tell you how glad these are sprinkled everywhere -Z

#define REICAST_S(v) __noop
#define REICAST_US(v) __noop

#define REICAST_SA(v_arr,num) __noop
#define REICAST_USA(v_arr,num) __noop

#endif //TARGET_NO_SSTATE


enum
{
	RN_CPSR      = 16,
	RN_SPSR      = 17,

	R13_IRQ      = 18,
	R14_IRQ      = 19,
	SPSR_IRQ     = 20,
	R13_USR      = 26,
	R14_USR      = 27,
	R13_SVC      = 28,
	R14_SVC      = 29,
	SPSR_SVC     = 30,
	R13_ABT      = 31,
	R14_ABT      = 32,
	SPSR_ABT     = 33,
	R13_UND      = 34,
	R14_UND      = 35,
	SPSR_UND     = 36,
	R8_FIQ       = 37,
	R9_FIQ       = 38,
	R10_FIQ      = 39,
	R11_FIQ      = 40,
	R12_FIQ      = 41,
	R13_FIQ      = 42,
	R14_FIQ      = 43,
	SPSR_FIQ     = 44,
	RN_PSR_FLAGS = 45,
	R15_ARM_NEXT = 46,
	INTR_PEND    = 47,
	CYCL_CNT     = 48,

	RN_ARM_REG_COUNT,
};

typedef union
{
	struct
	{
		u8 B0;
		u8 B1;
		u8 B2;
		u8 B3;
	} B;

	struct
	{
		u16 W0;
		u16 W1;
	} W;

	union
	{
		struct
		{
			u32 _pad0 : 28;
			u32 V     : 1; //Bit 28
			u32 C     : 1; //Bit 29
			u32 Z     : 1; //Bit 30
			u32 N     : 1; //Bit 31
		};

		struct
		{
			u32 _pad1 : 28;
			u32 NZCV  : 4; //Bits [31:28]
		};
	} FLG;

	struct
	{
		u32 M     : 5;  //mode, PSR[4:0]
		u32 _pad0 : 1;  //not used / zero
		u32 F     : 1;  //FIQ disable, PSR[6]
		u32 I     : 1;  //IRQ disable, PSR[7]
		u32 _pad1 : 20; //not used / zero
		u32 NZCV  : 4;  //Bits [31:28]
	} PSR;

	u32 I;
} reg_pair;




#if COMPILER_VC_OR_CLANG_WIN32
#pragma warning( disable : 4127 4996 /*4244*/)
#else
#define stricmp strcasecmp
#endif

#ifndef STRIP_TEXT
#define verify(x) if((x)==false){ msgboxf("Verify Failed  : " #x "\n in %s -> %s : %d \n",MBX_ICONERROR,(__FUNCTION__),(__FILE__),__LINE__); dbgbreak;}
#define die(reason) { msgboxf("Fatal error : %s\n in %s -> %s : %d \n",MBX_ICONERROR,(reason),(__FUNCTION__),(__FILE__),__LINE__); dbgbreak;}
#else
#define verify(x) if((x)==false) { dbgbreak; }
#define die(reason) { dbgbreak; }
#endif

#define fverify verify


//will be removed sometime soon
//This shit needs to be moved to proper headers
typedef u32  RegReadFP();
typedef u32  RegReadAddrFP(u32 addr);

typedef void RegWriteFP(u32 data);
typedef void RegWriteAddrFP(u32 addr, u32 data);

/*
	Read Write Const
	D    D     N      -> 0			-> RIO_DATA
	D    F     N      -> WF			-> RIO_WF
	F    F     N      -> RF|WF		-> RIO_FUNC
	D    X     N      -> RO|WF		-> RIO_RO
	F    X     N      -> RF|WF|RO	-> RIO_RO_FUNC
	D    X     Y      -> CONST|RO|WF-> RIO_CONST
	X    F     N      -> RF|WF|WO	-> RIO_WO_FUNC
*/
enum RegStructFlags
{
	//Basic :
	REG_ACCESS_8=1,
	REG_ACCESS_16=2,
	REG_ACCESS_32=4,

	REG_RF=8,
	REG_WF=16,
	REG_RO=32,
	REG_WO=64,
	REG_CONST=128,
	REG_NO_ACCESS=REG_RO|REG_WO,
};

enum RegIO
{
	RIO_DATA = 0,
	RIO_WF = REG_WF,
	RIO_FUNC = REG_WF | REG_RF,
	RIO_RO = REG_RO | REG_WF,
	RIO_RO_FUNC = REG_RO | REG_RF | REG_WF,
	RIO_CONST = REG_RO | REG_WF,
	RIO_WO_FUNC = REG_WF | REG_RF | REG_WO,
	RIO_NO_ACCESS = REG_WF | REG_RF | REG_NO_ACCESS
};

struct RegisterStruct
{
	union
	{
		u32 data32;					//stores data of reg variable [if used] 32b
		u16 data16;					//stores data of reg variable [if used] 16b
		u8  data8;					//stores data of reg variable [if used]	8b

		RegReadFP* readFunction;	//stored pointer to reg read function
		RegReadAddrFP* readFunctionAddr;
	};

	union
	{
		RegWriteFP* writeFunction;	//stored pointer to reg write function
		RegWriteAddrFP* writeFunctionAddr;
	};

	u32 flags;					//Access flags !
};

enum SmcCheckEnum {
	FullCheck = 0,
	FastCheck = 1,
	NoCheck = 2
};

struct settings_t
{
	struct {
		bool UseReios;
	} bios;

	struct {
		string ElfFile;
	} reios;

	struct
	{
		bool UseMipmaps;
		bool WideScreen;
		bool ShowFPS;
		bool RenderToTextureBuffer;
		int RenderToTextureUpscale;
		bool TranslucentPolygonDepthMask;
		bool ModifierVolumes;
		bool Clipping;
		int TextureUpscale;
		int MaxFilteredTextureSize;
		f32 ExtraDepthScale;
		bool CustomTextures;
		bool DumpTextures;
		int ScreenScaling;		// in percent. 50 means half the native resolution
		int ScreenStretching;	// in percent. 150 means stretch from 4/3 to 6/3
		bool Fog;
		bool FloatVMUs;
		bool Rotate90;			// Rotate the screen 90 deg CC
	} rend;

	struct
	{
		bool Enable;
		bool idleskip;
		bool unstable_opt;
		bool safemode;
		bool disable_nvmem;
		SmcCheckEnum SmcCheckLevel;
	} dynarec;

	struct
	{
		u32 run_counts;
	} profile;

	struct
	{
		u32 cable;			// 0 -> VGA, 1 -> VGA, 2 -> RGB, 3 -> TV
		u32 region;			// 0 -> JP, 1 -> USA, 2 -> EU, 3 -> default
		u32 broadcast;		// 0 -> NTSC, 1 -> PAL, 2 -> PAL/M, 3 -> PAL/N, 4 -> default
		u32 language;		// 0 -> JP, 1 -> EN, 2 -> DE, 3 -> FR, 4 -> SP, 5 -> IT, 6 -> default
		std::vector<std::string> ContentPath;
		bool FullMMU;
	} dreamcast;

	struct
	{
		u32 HW_mixing;		//(0) -> SW , 1 -> HW , 2 -> Auto
		u32 BufferSize;		//In samples ,*4 for bytes (1024)
		bool LimitFPS;		// defaults to true
		u32 GlobalFocus;	//0 -> only hwnd , (1) -> Global
		u32 BufferCount;	//BufferCount+2 buffers used , max 60 , default 0
		u32 CDDAMute;
		u32 GlobalMute;
		u32 DSPEnabled;		//0 -> no, 1 -> yes
		bool OldSyncronousDma;		// 1 -> sync dma (old behavior), 0 -> async dma (fixes some games, partial implementation)
		bool NoBatch;
		bool NoSound;
	} aica;

	struct{
		std::string backend;
	} audio;
#if USE_OMX
	struct
	{
		u32 Audio_Latency;
		bool Audio_HDMI;
	} omx;
#endif

#if SUPPORT_DISPMANX
	struct
	{
		u32 Width;
		u32 Height;
		bool Keep_Aspect;
	} dispmanx;
#endif

	struct
	{
		bool PatchRegion;
		bool LoadDefaultImage;
		char DefaultImage[512];
		char LastImage[512];
	} imgread;

	struct
	{
		u32 ta_skip;
		u32 rend;

		u32 MaxThreads;
		bool SynchronousRender;
	} pvr;

	struct {
		bool SerialConsole;
	} debug;

	struct {
		bool OpenGlChecks;
	} validate;

	struct {
		u32 MouseSensitivity;
		u32 JammaSetup;			// 0: standard, 1: 4-players, 2: rotary encoders, 3: Sega Marine Fishing,
								// 4: dual I/O boards (4P), 5: Namco JYU board (Ninja Assault)
		int maple_devices[4];
		int maple_expansion_devices[4][2];
		int VirtualGamepadVibration;
	} input;
};

extern settings_t settings;

void InitSettings();
void LoadSettings(bool game_specific);
void SaveSettings();
u32 GetRTC_now();
extern u32 patchRB;

inline bool is_s8(u32 v) { return (s8)v==(s32)v; }
inline bool is_u8(u32 v) { return (u8)v==(s32)v; }
inline bool is_s16(u32 v) { return (s16)v==(s32)v; }
inline bool is_u16(u32 v) { return (u16)v==(u32)v; }

#define verifyc(x) verify(!FAILED(x))

static inline void do_nada(...) { }

#ifdef _ANDROID
#include <android/log.h>

#ifdef printf
#undef printf
#endif

#ifdef puts
#undef puts
#endif

#define LOG_TAG   "reicast"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
	#ifdef STRIP_TEXT
		#define puts do_nada
		#define printf do_nada
	#else
		#define puts      LOGI
		#define printf    LOGI
	#endif
#define putinf    LOGI
#endif



#include "hw/sh4/sh4_if.h"

//more to come

extern sh4_if				  sh4_cpu;

//sh4 thread
s32 plugins_Init();
void plugins_Term();
void plugins_Reset(bool Manual);

//PVR
s32 libPvr_Init();
void libPvr_Reset(bool Manual);
void libPvr_Term();


//void DYNACALL libPvr_TaSQ(u32* data);				//size is 32 byte transfer counts
u32 libPvr_ReadReg(u32 addr,u32 size);
void libPvr_WriteReg(u32 addr,u32 data,u32 size);

void libPvr_LockedBlockWrite(vram_block* block,u32 addr);	//set to 0 if not used

void* libPvr_GetRenderTarget();
void* libPvr_GetRenderSurface();

//AICA
s32 libAICA_Init();
void libAICA_Reset(bool Manual);
void libAICA_Term();


u32  libAICA_ReadReg(u32 addr,u32 size);
void libAICA_WriteReg(u32 addr,u32 data,u32 size);

u32  libAICA_ReadMem_aica_ram(u32 addr,u32 size);
void libAICA_WriteMem_aica_ram(u32 addr,u32 data,u32 size);
void libAICA_Update(u32 cycles);				//called every ~1800 cycles, set to 0 if not used


//GDR
s32 libGDR_Init();
void libGDR_Reset(bool M);
void libGDR_Term();

void libCore_gdrom_disc_change();

//IO
void libGDR_ReadSector(u8 * buff,u32 StartSector,u32 SectorCount,u32 secsz);
void libGDR_ReadSubChannel(u8 * buff, u32 format, u32 len);
void libGDR_GetToc(u32* toc,u32 area);
u32 libGDR_GetDiscType();
void libGDR_GetSessionInfo(u8* pout,u8 session);


// 0x00600000 - 0x006007FF [NAOMI] (modem area for dreamcast)
u32  libExtDevice_ReadMem_A0_006(u32 addr,u32 size);
void libExtDevice_WriteMem_A0_006(u32 addr,u32 data,u32 size);

//Area 0 , 0x01000000- 0x01FFFFFF	[Ext. Device]
static u32 libExtDevice_ReadMem_A0_010(u32 addr,u32 size) { return 0; }
static void libExtDevice_WriteMem_A0_010(u32 addr,u32 data,u32 size) { }

//Area 5
static u32 libExtDevice_ReadMem_A5(u32 addr,u32 size){ return 0; }
static void libExtDevice_WriteMem_A5(u32 addr,u32 data,u32 size) { }

//ARM
s32 libARM_Init();
void libARM_Reset(bool M);
void libARM_Term();

void libARM_SetResetState(u32 State);
void libARM_Update(u32 cycles);


#define 	ReadMemArrRet(arr,addr,sz)				\
			{if (sz==1)								\
				return arr[addr];					\
			else if (sz==2)							\
				return *(u16*)&arr[addr];			\
			else if (sz==4)							\
				return *(u32*)&arr[addr];}

#define WriteMemArr(arr,addr,data,sz)				\
			{if(sz==1)								\
				{arr[addr]=(u8)data;}				\
			else if (sz==2)							\
				{*(u16*)&arr[addr]=(u16)data;}		\
			else if (sz==4)							\
			{*(u32*)&arr[addr]=data;}}

#define WriteMemArrRet(arr,addr,data,sz)				\
			{if(sz==1)								\
				{arr[addr]=(u8)data;return;}				\
			else if (sz==2)							\
				{*(u16*)&arr[addr]=(u16)data;return;}		\
			else if (sz==4)							\
			{*(u32*)&arr[addr]=data;return;}}

struct OnLoad
{
	typedef void OnLoadFP();
	OnLoad(OnLoadFP* fp) { fp(); }
};

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#endif

