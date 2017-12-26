// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the TOTEMOEZHCN_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// TOTEMOEZHCN_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef TOTEMOEZHCN_EXPORTS
#define TOTEMOEZHCN_API __declspec(dllexport)
#else
#define TOTEMOEZHCN_API __declspec(dllimport)
#endif
